#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "ap_int.h"
#include "pointwise_add.h"

#ifndef MAIN_H
#define MAIN_H

ap_uint<32> calculate_value(ap_uint<CONFIG_IN_WIDTH> values[CONFIG_N]);
void example(hls::stream< ap_axis<32,2,5,6> > &A,
	     hls::stream< ap_axis<32,2,5,6> > &B);

template<int IN_WIDTH, int N, int DEFAULT_ADDS, int FABRIC_ADDS, int DSP_ADDS, int RAM_ADDS, int BRAM_PARTITION_FACTOR, bool DUAL_PORT, bool LUT_RAM>
void process_layer(int *layer_in_start, int *layer_out_start, ap_uint<CONFIG_IN_WIDTH + CONFIG_NUM_LAYERS> partial_sums[CONFIG_NUM_PARTIAL_RESULTS], const ap_uint<IN_WIDTH + 1> *add_table) {
#pragma HLS ARRAY PARTITION variable=partial_sums complete
#pragma HLS pipeline
	const int NUM_SUMS = N / 2;
	if (NUM_SUMS * 2 != N) {
		partial_sums[*layer_out_start + NUM_SUMS] = partial_sums[*layer_out_start - 1];
	}

	ap_uint<IN_WIDTH> in1[NUM_SUMS];
	ap_uint<IN_WIDTH> in2[NUM_SUMS];
	ap_uint<IN_WIDTH + 1> out[NUM_SUMS];
#pragma HLS ARRAY PARTITION variable=in1 complete
#pragma HLS ARRAY PARTITION variable=in2 complete
#pragma HLS ARRAY PARTITION variable=out complete

	for (int j = 0; j < NUM_SUMS; j++) {
#pragma HLS unroll
		in1[j] = partial_sums[*layer_in_start + j];
	}

	for (int j = 0; j < NUM_SUMS; j++) {
#pragma HLS unroll
		in2[j] = partial_sums[*layer_in_start + NUM_SUMS + j];
	}

	for (int j = 0; j < NUM_SUMS; j++) {
#pragma HLS unroll
		out[j] = 0;
	}

	// Populate in1, in2 and out
	pointwise_add<
	    IN_WIDTH,
		NUM_SUMS,
		DEFAULT_ADDS,
		FABRIC_ADDS,
		DSP_ADDS,
		RAM_ADDS,
		BRAM_PARTITION_FACTOR,
		DUAL_PORT,
		LUT_RAM
		> (in1, in2, out, add_table);

	for (int j = 0; j < NUM_SUMS; j++) {
		partial_sums[*layer_out_start + j] = out[j];
	}

	*layer_in_start = *layer_out_start;
	*layer_out_start += (N + 1) / 2;
}


#endif
