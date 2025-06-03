#include "main.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "ap_int.h"

#ifdef ADD_TABLE_HEADER

#include ADD_TABLE_HEADER

ap_uint<IN_WIDTH + 1> bram_single_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=add_table core=ROM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=add_table cyclic factor=BRAM_PARTITION_FACTOR
	return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

ap_uint<IN_WIDTH + 1> bram_dual_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=add_table core=ROM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=add_table cyclic factor=BRAM_PARTITION_FACTOR
	return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

ap_uint<IN_WIDTH + 1> lutram_single_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=add_table core=ROM_1P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=add_table complete
    return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];  
}

ap_uint<IN_WIDTH + 1> lutram_dual_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=add_table core=ROM_2P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=add_table complete
    return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

#endif

void calculate_value(ap_uint<IN_WIDTH> in1[N], ap_uint<IN_WIDTH> in2[N], ap_uint<IN_WIDTH + 1> result[N]) {
#pragma HLS inline off
#pragma HLS pipeline
#pragma HLS ARRAY_PARTITION variable=in1 complete
#pragma HLS ARRAY_PARTITION variable=in2 complete
#pragma HLS ARRAY_PARTITION variable=result complete

	for (int i = 0; i < DEFAULT_ADDS; i++) {
#pragma HLS unroll
	    result[i] = in1[i] + in2[i];
	}

    // FABRIC: explicit fabric processing elements will use more LUTs than those
    // instantiated by default due to fewer automatic optimisations
    for (int i = DEFAULT_ADDS; i < DEFAULT_ADDS + FABRIC_ADDS; i++) {
#pragma HLS unroll
    	unsigned int temp;
#pragma HLS RESOURCE variable=temp core=AddSub
        temp = in1[i] + in2[i];
        result[i] = temp;
    }

    for (int i =  DEFAULT_ADDS + FABRIC_ADDS; i < DEFAULT_ADDS + FABRIC_ADDS + DSP_ADDS; i++) {
#pragma HLS unroll
    	unsigned int temp;
#pragma HLS RESOURCE variable=temp core=AddSub_DSP
        temp = in1[i] + in2[i];
        result[i] = temp;
    }

    // RAM: only supported for bit-widths where add table has been defined
    #ifdef ADD_TABLE_HEADER
    for (int i =  DEFAULT_ADDS + FABRIC_ADDS + DSP_ADDS; i < DEFAULT_ADDS + FABRIC_ADDS + DSP_ADDS + RAM_ADDS; i++) {
#pragma HLS unroll
        if (DUAL_PORT) {
            if (LUT_RAM) {
                result[i] = lutram_dual_port_add(in1[i], in2[i]);
            } else {
                result[i] = bram_dual_port_add(in1[i], in2[i]);
            }
        } else {
            if (LUT_RAM) {
                result[i] = lutram_single_port_add(in1[i], in2[i]);
            } else {
                result[i] = bram_single_port_add(in1[i], in2[i]);
            }
        }
    }
    #endif
}

void example(hls::stream< ap_axis<32,2,5,6> > &A,
	     hls::stream< ap_axis<32,2,5,6> > &B) {
#pragma HLS INTERFACE axis port=A
#pragma HLS INTERFACE axis port=B

#pragma HLS INTERFACE s_axilite port=return

	ap_uint<IN_WIDTH> in1[N];
	ap_uint<IN_WIDTH> in2[N];
#pragma HLS ARRAY_PARTITION variable=in1 complete
#pragma HLS ARRAY_PARTITION variable=in2 complete

    // Zero-initialise arrays
    for (int i = 0; i < N; i++) {
#pragma HLS unroll
		in1[i] = 0;
		in2[i] = 0;
	}
    
    // Dummy call to fill BRAM pipelines
	volatile ap_uint<32> dummy_result = calculate_value(weights, prev_in);

	ap_axis<32,2,5,6> tmp;
	ap_axis<32,2,5,6> tmp2;

    for (int i = 0; i < N; i++) {
    	A.read(tmp);

        in1[i] = tmp.data.range(IN_WIDTH - 1,0);
        in2[i] = tmp.data.range(2 * IN_WIDTH - 1, IN_WIDTH);
    }

    tmp2.keep = tmp.keep;
    tmp2.strb = tmp.strb;
    tmp2.user = tmp.user;
    tmp2.id = tmp.id;
    tmp2.last = 0;
    tmp2.dest = tmp.dest;

    ap_uint<32> to_return_arr[N];
#pragma HLS ARRAY_PARTITION variable=to_return_arr complete
    calculate_value(in1, in2, to_return_arr);

    for (int i = 0; i < N; i++) {
        ap_uint<32> to_return = to_return_arr[i];
        tmp2.data = to_return;
        if (i == N - 1) {
            tmp2.last = 1;
        } 
        B.write(tmp2);
    }
}
