
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "ap_int.h"
#include "pointwise_add.h"
#include "reduction_acc.h"



ap_uint<32> reduction_acc(ap_uint<CONFIG_IN_WIDTH * 2> values[CONFIG_N]) {
#pragma HLS inline off
#pragma HLS pipeline
#pragma HLS ARRAY_PARTITION variable=values complete

    if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_DEFAULT) {
        ap_uint<32> total = 0;
        for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
		    total += values[i];
	    }
	    return total;
    } else if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_FABRIC) {
        ap_uint<32> total_fabric = 0;
#pragma HLS RESOURCE variable=total_fabric core=AddSub
        for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
		    total_fabric += values[i];
	    }
	    return total_fabric;
    } else if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_DSP) {
        ap_uint<32> total_dsp = 0;
#pragma HLS RESOURCE variable=total_dsp core=AddSub_DSP
        for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
		    total_dsp += values[i];
	    }
	    return total_dsp;
    } else if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_RAM) {
        ap_uint<32> total = 0;
        for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
            if (CONFIG_HOMOGENEOUS_DUAL_PORT) {
                if (CONFIG_HOMOGENEOUS_LUT_RAM) {
                    total = lutram_dual_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1>(total, values[i], 0);
                } else {
                    total = bram_dual_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1, CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR>(total, values[i], 0);
                }
            } else {
                if (CONFIG_HOMOGENEOUS_LUT_RAM) {
                    total = lutram_single_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1>(total, values[i], 0);
                } else {
                    total = bram_single_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1, CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR>(total, values[i], 0);
                }
            }
        }
        return total;
    }

    // Flattened array for output values, all given the largest width
    ap_uint<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS> partial_sums[CONFIG_NUM_PARTIAL_RESULTS];
#pragma HLS ARRAY_PARTITION variable=partial_sums complete

    for (int i = 0; i < CONFIG_NUM_PARTIAL_RESULTS; i++) {
#pragma HLS unroll
        partial_sums[i] = 0;
    }

    for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
            partial_sums[i] = values[i];
    }

    int layer_in_start = 0;
    int layer_out_start = CONFIG_N;

    process_layer<LAYER_IN_WIDTH_0, LAYER_N_0, LAYER_DEFAULT_ADDS_0, LAYER_FABRIC_ADDS_0, LAYER_DSP_ADDS_0, LAYER_RAM_ADDS_0, LAYER_BRAM_PARTITION_FACTOR_0, LAYER_DUAL_PORT_0, LAYER_LUT_RAM_0>(&layer_in_start, &layer_out_start, partial_sums, 0);
	process_layer<LAYER_IN_WIDTH_1, LAYER_N_1, LAYER_DEFAULT_ADDS_1, LAYER_FABRIC_ADDS_1, LAYER_DSP_ADDS_1, LAYER_RAM_ADDS_1, LAYER_BRAM_PARTITION_FACTOR_1, LAYER_DUAL_PORT_1, LAYER_LUT_RAM_1>(&layer_in_start, &layer_out_start, partial_sums, 0);
	process_layer<LAYER_IN_WIDTH_2, LAYER_N_2, LAYER_DEFAULT_ADDS_2, LAYER_FABRIC_ADDS_2, LAYER_DSP_ADDS_2, LAYER_RAM_ADDS_2, LAYER_BRAM_PARTITION_FACTOR_2, LAYER_DUAL_PORT_2, LAYER_LUT_RAM_2>(&layer_in_start, &layer_out_start, partial_sums, 0);
	process_layer<LAYER_IN_WIDTH_3, LAYER_N_3, LAYER_DEFAULT_ADDS_3, LAYER_FABRIC_ADDS_3, LAYER_DSP_ADDS_3, LAYER_RAM_ADDS_3, LAYER_BRAM_PARTITION_FACTOR_3, LAYER_DUAL_PORT_3, LAYER_LUT_RAM_3>(&layer_in_start, &layer_out_start, partial_sums, 0);
	

    return partial_sums[CONFIG_NUM_PARTIAL_RESULTS - 1];
}
