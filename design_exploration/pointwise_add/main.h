#include "config.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_int.h"

ap_uint<IN_WIDTH + 1> bram_single_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
ap_uint<IN_WIDTH + 1> bram_dual_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
ap_uint<IN_WIDTH + 1> lutram_single_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
ap_uint<IN_WIDTH + 1> lutram_dual_port_add(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
void calculate_value(ap_uint<IN_WIDTH> weights[N], ap_uint<IN_WIDTH> prev_in[N], ap_uint<32> result[N]);
void example(hls::stream< ap_axis<32,2,5,6> > &A, hls::stream< ap_axis<32,2,5,6> > &B);
