#ifndef POINTWISE_MUL_H
#define POINTWISE_MUL_H

#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "ap_int.h"

// Parallel memory-based multiplication of N IN_WIDTH-bit numbers 
// The implementation strategy of each parallel processing element is configurable

template <int IN_WIDTH, int BRAM_PARTITION_FACTOR>
ap_uint<2 * IN_WIDTH> bram_single_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<2 * IN_WIDTH> *mul_table) {
#pragma HLS RESOURCE variable=mul_table core=ROM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=mul_table cyclic factor=BRAM_PARTITION_FACTOR
	return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template <int IN_WIDTH, int BRAM_PARTITION_FACTOR>
ap_uint<2 * IN_WIDTH> bram_dual_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<2 * IN_WIDTH> *mul_table) {
#pragma HLS RESOURCE variable=mul_table core=ROM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=mul_table cyclic factor=BRAM_PARTITION_FACTOR
	return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template <int IN_WIDTH>
ap_uint<2 * IN_WIDTH> lutram_single_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<2 * IN_WIDTH> *mul_table) {
#pragma HLS RESOURCE variable=mul_table core=ROM_1P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=mul_table complete
    return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];  
}

template <int IN_WIDTH>
ap_uint<2 * IN_WIDTH> lutram_dual_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2, const ap_uint<2 * IN_WIDTH> *mul_table) {
#pragma HLS RESOURCE variable=mul_table core=ROM_2P_LUTRAM
#pragma HLS ARRAY_PARTITION variable=mul_table complete
    return mul_table[(((ap_uint<2 * IN_WIDTH>) in1) << IN_WIDTH) | in2];
}

template<int IN_WIDTH, int N, int DEFAULT_MULS, int FABRIC_MULS, int DSP_MULS, int RAM_MULS, int BRAM_PARTITION_FACTOR, bool DUAL_PORT, bool LUT_RAM>
void pointwise_mul(ap_uint<IN_WIDTH> weights[N], ap_uint<IN_WIDTH> prev_in[N], ap_uint<2 * IN_WIDTH> result[N], const ap_uint<2 * IN_WIDTH> *mul_table) {
#pragma HLS inline off
#pragma HLS pipeline
#pragma HLS ARRAY_PARTITION variable=weights complete
#pragma HLS ARRAY_PARTITION variable=prev_in complete
#pragma HLS ARRAY_PARTITION variable=result complete

	for (int i = 0; i < DEFAULT_MULS; i++) {
#pragma HLS unroll
	    result[i] = weights[i] * prev_in[i];
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
    for (int i =  DEFAULT_MULS + FABRIC_MULS + DSP_MULS; i < DEFAULT_MULS + FABRIC_MULS + DSP_MULS + RAM_MULS; i++) {
#pragma HLS unroll
        if (DUAL_PORT) {
            if (LUT_RAM) {
                result[i] = lutram_dual_port_mul<IN_WIDTH>(weights[i], prev_in[i], mul_table);
            } else {
                result[i] = bram_dual_port_mul<IN_WIDTH, BRAM_PARTITION_FACTOR>(weights[i], prev_in[i], mul_table);
            }
        } else {
            if (LUT_RAM) {
                result[i] = lutram_single_port_mul<IN_WIDTH>(weights[i], prev_in[i], mul_table);
            } else {
                result[i] = bram_single_port_mul<IN_WIDTH, BRAM_PARTITION_FACTOR>(weights[i], prev_in[i], mul_table);
            }
        }
    }
}

#endif
