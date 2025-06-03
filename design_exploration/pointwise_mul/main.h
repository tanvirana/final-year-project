#include "config.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_int.h"

ap_uint<2 * IN_WIDTH> bram_single_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
ap_uint<2 * IN_WIDTH> bram_dual_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
ap_uint<2 * IN_WIDTH> lutram_single_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
ap_uint<2 * IN_WIDTH> lutram_dual_port_mul(ap_uint<IN_WIDTH> in1, ap_uint<IN_WIDTH> in2);
void calculate_value(ap_uint<IN_WIDTH> weights[N], ap_uint<IN_WIDTH> prev_in[N], ap_uint<2 * IN_WIDTH> result[N]);
void example(hls::stream< ap_axis<32,2,5,6> > &A, hls::stream< ap_axis<32,2,5,6> > &B);