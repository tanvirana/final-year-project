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

  ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N];
  for (int i = 0; i < CONFIG_N; i++) {
	  weights[i] = rand() % (int) pow(2, CONFIG_IN_WIDTH);

	  cout << "Weight " << i << " is " << weights[i] << endl;
  }

  ap_axis<32,2,5,6> tmp1, tmp2;
  hls::stream<ap_axis<32,2,5,6> > A, B;

  ap_uint<32> out;
  ap_uint<32> expected_out = 0;

  for (int i = 0; i < CONFIG_N; i++) {
	  expected_out += weights[i];
  }


  ap_uint<32> tmp_data = 0;
  for (int i = 0; i < CONFIG_N; i++) {
	  tmp_data.range(7,0) = weights[i];
	  tmp_data.range(15,8) = 0;
	  tmp_data.range(31,16) = 0;
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

  example(A, B);

  while (B.empty()) {}
  B.read(tmp2);
  out = tmp2.data.to_uint();
  cout << "Expected " << expected_out << ", got " << out << endl;


  if (out != expected_out) {
	  cout << "ERROR: Mismatch" << endl;
	  return 1;
  }

  cout << "SUCCESS: Match" << endl;

  return 0;
}

#endif
