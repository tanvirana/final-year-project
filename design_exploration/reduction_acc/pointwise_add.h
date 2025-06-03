#ifndef POINTWISE_ADD_H

#define POINTWISE_ADD_H

#include "config.h"

template <int IN_WIDTH, int BRAM_PARTITION_FACTOR>
ap_uint<IN_WIDTH + 1> bram_single_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<IN_WIDTH + 1> *add_table) {
#pragma HLS RESOURCE variable=add_table core=ROM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=add_table cyclic factor=BRAM_PARTITION_FACTOR
	return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template <int IN_WIDTH, int BRAM_PARTITION_FACTOR>
ap_uint<IN_WIDTH + 1> bram_dual_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<IN_WIDTH + 1> *add_table) {
    #pragma HLS RESOURCE variable=add_table core=ROM_2P_BRAM
	#pragma HLS ARRAY_PARTITION variable=add_table cyclic factor=BRAM_PARTITION_FACTOR
   return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template <int IN_WIDTH>
ap_uint<IN_WIDTH + 1> lutram_single_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<IN_WIDTH + 1> *add_table) {
#pragma HLS RESOURCE variable=add_table core=ROM_1P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=add_table complete
    return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template<int IN_WIDTH>
ap_uint<IN_WIDTH + 1> lutram_dual_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<IN_WIDTH + 1> *add_table) {
#pragma HLS RESOURCE variable=add_table core=ROM_2P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=add_table complete
    return add_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template<int IN_WIDTH, int N, int DEFAULT_ADDS, int FABRIC_ADDS, int DSP_ADDS, int RAM_ADDS, int BRAM_PARTITION_FACTOR, bool DUAL_PORT, bool LUT_RAM>
void pointwise_add(ap_uint<IN_WIDTH> in1[N], ap_uint<IN_WIDTH> in2[N], ap_uint<IN_WIDTH + 1> result[N], const ap_uint<IN_WIDTH + 1> *add_table) {
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
    for (int i =  DEFAULT_ADDS + FABRIC_ADDS + DSP_ADDS; i < DEFAULT_ADDS + FABRIC_ADDS + DSP_ADDS + RAM_ADDS; i++) {
#pragma HLS unroll
        if (DUAL_PORT) {
            if (LUT_RAM) {
                result[i] = lutram_dual_port_add<IN_WIDTH>(in1[i], in2[i], add_table);
            } else {    
                result[i] = bram_dual_port_add<IN_WIDTH, BRAM_PARTITION_FACTOR>(in1[i], in2[i], add_table);  
            }
        } else {
            if (LUT_RAM) {
                result[i] = lutram_single_port_add<IN_WIDTH>(in1[i], in2[i], add_table);
            } else {
                result[i] = bram_single_port_add<IN_WIDTH, BRAM_PARTITION_FACTOR>(in1[i], in2[i], add_table);
            }
        }
    }
}

#endif
