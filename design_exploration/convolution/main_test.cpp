#if 1

#include <iostream>
#include "config.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include <ctime>
#include "config.h"
using namespace std;

void example(hls::stream<ap_axis<32,2,5,6> > &A, hls::stream<ap_axis<32,2,5,6> > &B);

int main()
{

  srand(time(0));

  ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N];
  for (int i = 0; i < CONFIG_N; i++) {
	  weights[i] = rand() % (int) pow(2, CONFIG_IN_WIDTH);;

	  cout << "Weight " << i << " is " << weights[i] << endl;
  }

//   int in_size = rand() % 1000 + 1;
	int in_size = 3;

  ap_uint<CONFIG_IN_WIDTH> in[in_size];
  for (int i = 0; i < in_size; i++) {
	  in[i] = rand() % (int) pow(2, CONFIG_IN_WIDTH);
	  cout << "In " << i << " is " << in[i] << endl;
  }

  ap_axis<32,2,5,6> tmp1, tmp2;
  hls::stream<ap_axis<32,2,5,6> > A, B;

  ap_uint<32> out[in_size];
  ap_uint<32> expected_out[in_size];

  ap_uint<CONFIG_IN_WIDTH> cur_weights[CONFIG_N];
  for (int i = 0; i < CONFIG_N; i++) {
	  cur_weights[i] = 0;
  }

  ap_uint<CONFIG_IN_WIDTH> cur_data[CONFIG_N];
  for (int i = 0; i < CONFIG_N; i++) {
	  cur_data[i] = 0;
  }

  ap_uint<32> total;

  for (int i = 0; i < in_size; i++) {

	  if (i < CONFIG_N) {
		  cur_weights[i] = weights[i];
	  }

	  for (int j = 1; j <CONFIG_N; j++) {
		  cur_data[CONFIG_N - j] = cur_data[CONFIG_N - j - 1];
	  }
	  cur_data[0] = in[i];

	  total = 0;
	  for (int j = 0; j < CONFIG_N; j++) {
		  total += cur_weights[j] * cur_data[j];
	  }
	  expected_out[i] = total;
  }

  ap_uint<32> tmp_data = 0;
  for (int i = 0; i < in_size; i++) {
	  if (i < CONFIG_N) {
		  tmp_data.range(CONFIG_IN_WIDTH - 1,0) = weights[i];
	  } else  {
		  tmp_data.range(CONFIG_IN_WIDTH - 1,0) = 0;
	  }
	  tmp_data.range(2 * CONFIG_IN_WIDTH - 1, CONFIG_IN_WIDTH) = in[i];
	  if (2 * CONFIG_IN_WIDTH < 32) {
		tmp_data.range(31, 2 * CONFIG_IN_WIDTH) = 0;
	  }
	  tmp1.data = tmp_data;
	  tmp1.keep = 1;
	  tmp1.strb = 1;
	  tmp1.user = 1;

	  if (i == in_size - 1) {
		  tmp1.last = 1;
	  } else {
		  tmp1.last = 0;
	  }

	  tmp1.id = 0;
	  tmp1.dest = 1;

	  A.write(tmp1);
  }

  cout << "Calling with in_size " << in_size << endl;

  example(A, B);

  for (int i = 0; i < in_size; i++) {
      while (B.empty()) {}  // optional wait
      B.read(tmp2);
      out[i] = tmp2.data.to_uint();
      cout << "Expected " << expected_out[i] << ", got " << out[i] << endl;
  }

  bool match = true;
  for (int i = 0; i < in_size; i++) {
	  if (out[i] != expected_out[i]) {
		  cout << "Mismatch at index " << i << endl;
		  cout << "Got " << out[i] << " but expected " << expected_out[i] << endl;
		  match = false;
	  }
  }

  if (!match) {
	  cout << "ERROR: Results mismatch" << endl;
	  return 1;
  }
  cout << "SUCCESS: Results match" << endl;

  return 0;
}

#endif
