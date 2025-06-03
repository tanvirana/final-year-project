#if 1

#include <iostream>
#include "config.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include <ctime>
#include <cmath>
using namespace std;

void example(hls::stream<ap_axis<32,2,5,6> > &A, hls::stream<ap_axis<32,2,5,6> > &B);

int main()
{
  srand(time(0));

  ap_uint<CONFIG_IN_WIDTH> in1[CONFIG_N];
  ap_uint<CONFIG_IN_WIDTH> in2[CONFIG_N];
  for (int i = 0; i < CONFIG_N; i++) {
	  in1[i] = rand() % (int) pow(2, CONFIG_IN_WIDTH);
	  in2[i] = rand() % (int) pow(2, CONFIG_IN_WIDTH);

	  cout << "In1 " << i << " is " << in1[i] << endl;
	  cout << "In2 " << i << " is " << in2[i] << endl;
  }

  ap_axis<32,2,5,6> tmp1, tmp2;
  hls::stream<ap_axis<32,2,5,6> > A, B;

  ap_uint<32> out;
  ap_uint<32> expected_out[CONFIG_N];

  for (int i = 0; i < CONFIG_N; i++) {
	  expected_out[i] = in1[i] + in2[i];
  }

  ap_uint<32> tmp_data = 0;
  for (int i = 0; i < CONFIG_N; i++) {
	  tmp_data.range(CONFIG_IN_WIDTH - 1,0) = in1[i];
	  tmp_data.range(2 * CONFIG_IN_WIDTH - 1,CONFIG_IN_WIDTH) = in2[i];
    if (2 * CONFIG_IN_WIDTH < 32) {
      tmp_data.range(31,2 * CONFIG_IN_WIDTH) = 0;
    }
	  tmp1.data = tmp_data;
	  tmp1.keep = 1;
	  tmp1.strb = 1;
	  tmp1.user = 1;

	  if (i == CONFIG_N - 1) {
		  tmp1.last = 1;
	  } else {
		  tmp1.last = 0;
	  }

	  tmp1.id = 0;
	  tmp1.dest = 1;

	  A.write(tmp1);
  }

  cout << "Calling" << endl;

  bool all_matched = true;

  pointwise_add_example(A, B);

  while (B.empty()) {}

  for (int i = 0; i < CONFIG_N; i++) {
    B.read(tmp2);
    out = tmp2.data.to_uint();
    cout << "Expected " << expected_out[i].to_uint() << ", got " << out << endl;

    if (out != expected_out[i]) {
      cout << "ERROR: Mismatch" << endl;
      all_matched = false;
    }
  }

  if (!all_matched) {
	  return 1;
  }

  return 0;
}

#endif