
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "pointwise_mul.h"
#include "reduction_acc.h"

#include "mul_table_6.h"


int calculate_value(ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N], ap_uint<CONFIG_IN_WIDTH> prev_in[CONFIG_N]) {
#pragma HLS inline off
#pragma HLS pipeline
	// Define an intermediate array to store multiplication results
	ap_uint<2 * CONFIG_IN_WIDTH> mul_results[CONFIG_N];
#pragma HLS ARRAY_PARTITION variable=mul_results complete

	pointwise_mul<CONFIG_IN_WIDTH, CONFIG_N, DEFAULT_MULS, FABRIC_MULS, DSP_MULS, RAM_MULS, MUL_BRAM_PARTITION_FACTOR, MUL_DUAL_PORT, MUL_LUT_RAM>(weights, prev_in, mul_results, mul_table);
	ap_uint<32> total = reduction_acc(mul_results);
	return total;
}

void example(hls::stream< ap_axis<32,2,5,6> > &A,
	     hls::stream< ap_axis<32,2,5,6> > &B) {
#pragma HLS INTERFACE axis port=A
#pragma HLS INTERFACE axis port=B

#pragma HLS INTERFACE s_axilite port=return

	ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N];
	ap_uint<CONFIG_IN_WIDTH> prev_in[CONFIG_N];
#pragma HLS ARRAY_PARTITION variable=weights complete
#pragma HLS ARRAY_PARTITION variable=prev_in complete

    // Zero-initialise arrays
	for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
		weights[i] = 0;
		prev_in[i] = 0;
	}

	// Dummy call to fill BRAM pipelines
	volatile ap_uint<32> dummy_result = calculate_value(weights, prev_in);

	int inCount = 0;

	ap_axis<32,2,5,6> tmp;
	ap_axis<32,2,5,6> tmp2;
    while(1) {
    	A.read(tmp);

    	if (inCount < CONFIG_N) {
    		weights[inCount] = tmp.data.range(CONFIG_IN_WIDTH - 1, 0);
	    	inCount++;
	    }

    	for (int i = CONFIG_N - 1; i > 0; i--) {
#pragma HLS unroll
    		prev_in[i] = prev_in[i - 1];
    	}
    	prev_in[0] = tmp.data.range(2 * CONFIG_IN_WIDTH - 1, CONFIG_IN_WIDTH);

    	tmp2.keep = tmp.keep;
    	tmp2.strb = tmp.strb;
    	tmp2.user = tmp.user;
    	tmp2.id = tmp.id;
    	tmp2.last = tmp.last;
    	tmp2.dest = tmp.dest;


    	ap_uint<32> to_return = calculate_value(weights, prev_in);
    	tmp2.data = to_return;
    	B.write(tmp2);

    	if (tmp2.last) {
    	    break;
    	}
    }
}
