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

  ap_uint<IN_WIDTH> weights[N];
  ap_uint<IN_WIDTH> in_data[N];
  for (int i = 0; i < N; i++) {
	  weights[i] = rand() % (int) pow(2, IN_WIDTH);
	  in_data[i] = rand() % (int) pow(2, IN_WIDTH);

	  cout << "Weight " << i << " is " << weights[i] << endl;
	  cout << "In " << i << " is " << in_data[i] << endl;
  }

  ap_axis<32,2,5,6> tmp1, tmp2;
  hls::stream<ap_axis<32,2,5,6> > A, B;

  ap_uint<32> out;
  ap_uint<32> expected_out[N];

  for (int i = 0; i < N; i++) {
	  expected_out[i] = weights[i] * in_data[i];
  }

  ap_uint<32> tmp_data = 0;
  for (int i = 0; i < N; i++) {
	  tmp_data.range(IN_WIDTH - 1,0) = weights[i];
	  tmp_data.range(2 * IN_WIDTH - 1,IN_WIDTH) = in_data[i];
    if (2 * IN_WIDTH < 32) {
      tmp_data.range(31,2 * IN_WIDTH) = 0;
    }
	  tmp1.data = tmp_data;
	  tmp1.keep = 1;
	  tmp1.strb = 1;
	  tmp1.user = 1;

	  if (i == N - 1) {
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

  example(A, B);

  while (B.empty()) {}

  for (int i = 0; i < N; i++) {
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
