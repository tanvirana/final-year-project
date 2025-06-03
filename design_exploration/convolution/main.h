#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "pointwise_mul.h"
#include "pointwise_add.h"
#include "reduction_acc.h"

int calculate_value(ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N], ap_uint<CONFIG_IN_WIDTH> prev_in[CONFIG_N]);
void example(hls::stream< ap_axis<32,2,5,6> > &A,
	     hls::stream< ap_axis<32,2,5,6> > &B);