#include "main.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "ap_int.h"

// Parallel memory-based multiplication of N IN_WIDTH-bit numbers 
// The implementation strategy of each parallel processing element is configurable

#ifdef MUL_TABLE_HEADER

#include MUL_TABLE_HEADER

ap_uint<2 * IN_WIDTH> bram_single_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=mul_table core=ROM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=mul_table cyclic factor=BRAM_PARTITION_FACTOR
	return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

ap_uint<2 * IN_WIDTH> bram_dual_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=mul_table core=ROM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=mul_table cyclic factor=BRAM_PARTITION_FACTOR
	return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

ap_uint<2 * IN_WIDTH> lutram_single_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=mul_table core=ROM_1P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=mul_table complete
    return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];  
}

ap_uint<2 * IN_WIDTH> lutram_dual_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2) {
#pragma HLS RESOURCE variable=mul_table core=ROM_2P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=mul_table complete
    return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

#endif

void calculate_value(ap_uint<IN_WIDTH> weights[N], ap_uint<IN_WIDTH> prev_in[N], ap_uint<2 * IN_WIDTH> result[N]) {
#pragma HLS inline off
#pragma HLS pipeline
#pragma HLS ARRAY_PARTITION variable=weights complete
#pragma HLS ARRAY_PARTITION variable=prev_in complete
#pragma HLS ARRAY_PARTITION variable=result complete

	for (int i = 0; i < DEFAULT_MULS; i++) {
#pragma HLS unroll
	    result[i] = weights[i] * prev_in[i];
	    //printf("Weight %d is %u\n", i, (unsigned int) weights[i]);
	    //printf("In %d is %u\n", i, (unsigned int) prev_in[i]);
		//mul_result = bram_mul(weights[i], prevIn[i]);
	    //printf("Result %d is %u\n", i, (unsigned int) result[i]);
	}

    // FABRIC: explicit fabric processing elements will use more LUTs than those
    // instantiated by default due to fewer automatic optimisations
    for (int i = DEFAULT_MULS; i < DEFAULT_MULS + FABRIC_MULS; i++) {
#pragma HLS unroll
    	unsigned int temp;
#pragma HLS RESOURCE variable=temp core=Mul_LUT
        temp = (unsigned int) weights[i] * (unsigned int) prev_in[i];
        result[i] = temp;
    }

    // DSP: note that DSPs are not instantiated for IN_WIDTH < 10
    for (int i =  DEFAULT_MULS + FABRIC_MULS; i < DEFAULT_MULS + FABRIC_MULS + DSP_MULS; i++) {
#pragma HLS unroll
    	unsigned int temp;
#pragma HLS RESOURCE variable=temp core=DSP48
        temp = weights[i] * prev_in[i];
        result[i] = temp;
    }

    // RAM: only supported for bit-widths where mul table has been defined

    #ifdef MUL_TABLE_HEADER
    for (int i =  DEFAULT_MULS + FABRIC_MULS + DSP_MULS; i < DEFAULT_MULS + FABRIC_MULS + DSP_MULS + RAM_MULS; i++) {
#pragma HLS unroll
        if (DUAL_PORT) {
            if (LUT_RAM) {
                result[i] = lutram_dual_port_mul(weights[i], prev_in[i]);
            } else {
                result[i] = bram_dual_port_mul(weights[i], prev_in[i]);
            }
        } else {
            if (LUT_RAM) {
                result[i] = lutram_single_port_mul(weights[i], prev_in[i]);
            } else {
                result[i] = bram_single_port_mul(weights[i], prev_in[i]);
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

	ap_uint<IN_WIDTH> weights[N];
	ap_uint<IN_WIDTH> prev_in[N];
#pragma HLS ARRAY_PARTITION variable=weights complete
#pragma HLS ARRAY_PARTITION variable=prev_in complete

    // Zero-initialise arrays
    for (int i = 0; i < N; i++) {
#pragma HLS unroll
		weights[i] = 0;
		prev_in[i] = 0;
	}

    // Dummy call to fill BRAM pipelines
	volatile ap_uint<32> dummy_result = calculate_value(weights, prev_in);

	ap_axis<32,2,5,6> tmp;
	ap_axis<32,2,5,6> tmp2;

    for (int i = 0; i < N; i++) {
    	A.read(tmp);

        weights[i] = tmp.data.range(IN_WIDTH - 1,0);
        prev_in[i] = tmp.data.range(2 * IN_WIDTH - 1, IN_WIDTH);
    }

    tmp2.keep = tmp.keep;
    tmp2.strb = tmp.strb;
    tmp2.user = tmp.user;
    tmp2.id = tmp.id;
    tmp2.last = 0;
    tmp2.dest = tmp.dest;

    ap_uint<2 * IN_WIDTH> to_return_arr[N];
#pragma HLS ARRAY_PARTITION variable=to_return_arr complete
    calculate_value(weights, prev_in, to_return_arr);

    for (int i = 0; i < N; i++) {
        ap_uint<32> to_return = to_return_arr[i];
        tmp2.data = to_return;
        if (i == N - 1) {
            tmp2.last = 1;
        } 
        B.write(tmp2);
    }
}