#include "config.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_int.h"
#include "pointwise_add.h"

void pointwise_add_example(hls::stream< ap_axis<32,2,5,6> > &A,
	     hls::stream< ap_axis<32,2,5,6> > &B) {
#pragma HLS INTERFACE axis port=A
#pragma HLS INTERFACE axis port=B

#pragma HLS INTERFACE s_axilite port=return

	ap_uint<CONFIG_IN_WIDTH> in1[CONFIG_N];
	ap_uint<CONFIG_IN_WIDTH> in2[CONFIG_N];
#pragma HLS ARRAY_PARTITION variable=in1 complete
#pragma HLS ARRAY_PARTITION variable=in2 complete

    // Zero-initialise arrays
    for (int i = 0; i < CONFIG_N; i++) {
#pragma HLS unroll
		in1[i] = 0;
		in2[i] = 0;
	}

	ap_axis<32,2,5,6> tmp;
	ap_axis<32,2,5,6> tmp2;

    for (int i = 0; i < CONFIG_N; i++) {
    	A.read(tmp);

        in1[i] = tmp.data.range(CONFIG_IN_WIDTH - 1,0);
        in2[i] = tmp.data.range(2 * CONFIG_IN_WIDTH - 1, CONFIG_IN_WIDTH);
    }

    tmp2.keep = tmp.keep;
    tmp2.strb = tmp.strb;
    tmp2.user = tmp.user;
    tmp2.id = tmp.id;
    tmp2.last = 0;
    tmp2.dest = tmp.dest;

    ap_uint<CONFIG_IN_WIDTH + 1> to_return_arr[CONFIG_N];
#pragma HLS ARRAY_PARTITION variable=to_return_arr complete
    pointwise_add<CONFIG_IN_WIDTH, CONFIG_N, 3, 0, 0, 0, false, false>(in1, in2, to_return_arr);

    for (int i = 0; i < CONFIG_N; i++) {
        ap_uint<32> to_return = to_return_arr[i];
        tmp2.data = to_return;
        if (i == CONFIG_N - 1) {
            tmp2.last = 1;
        } 
        B.write(tmp2);
    }
}