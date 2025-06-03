from pynq import Overlay, allocate
import numpy as np
import sys
import random
import math
import time
from ref_convolver import convolve
ol = Overlay("/mnt/usb/Tanvi/configurable_convolver/configurable_convolver.bit")

print("N is: " + sys.argv[1])
N = int(sys.argv[1])
IN_WIDTH = int(sys.argv[2])
dma = ol.axi_dma
dma_send = ol.axi_dma.sendchannel
dma_recv = ol.axi_dma.recvchannel

hls_ip = ol.example_0

CONTROL_REGISTER = 0x0
print("Starting IP")
hls_ip.write(CONTROL_REGISTER, 0x81)

print("Generating weights and in data")
weights = [random.randint(0, math.pow(2,IN_WIDTH) - 1) for i in range(N)]
#weights = [1,2,3,4,5]
#in_data = [1,2,3,4,5,6,7,8,9,10]
data_size = 10
in_data = [random.randint(0, math.pow(2,IN_WIDTH) - 1) for i in range(data_size)]
input_buffer = allocate(shape=(data_size,), dtype=np.uint32)

print("Populating input buffer")
for i in range(data_size):
    input_buffer[i] = (in_data[i] << IN_WIDTH) | (weights[i] if i < len(weights) else 0)

print("Defining output buffer")
output_buffer = allocate(shape=(data_size,), dtype=np.uint32)

print("Sending data to IP")
start_time = time.perf_counter()
dma_send.transfer(input_buffer)
dma_recv.transfer(output_buffer)
dma_recv.wait()
end_time = time.perf_counter()
print("Reached end")
elapsed_time_ns = (end_time - start_time) * 1e9  # Convert seconds to nanoseconds
print(f"IP processing took {elapsed_time_ns:.0f} nanoseconds.")

print("Calculating expected output")
start_time = time.perf_counter()
expected_out = convolve(weights, in_data)
end_time = time.perf_counter()
elapsed_time_ns = (end_time - start_time) * 1e9
print(f"CPU processing took {elapsed_time_ns:.0f} nanoseconds.")

matched = True
for i in range(data_size):
    if (output_buffer[i] != expected_out[i]):
        matched = False
#        print("Expected " + str(expected_out[i]) + ", got " + str(output_buffer[i]))
#        print("^ on loop " + str(i))
#        print("Weight is " + str(weights[0]))
#        print("Data is " + str(in_data[i]))
#        print("Input buffer is " + str(input_buffer[i]))

print("SUCCESS" if matched else "FAILED")

print("Weights: {}".format(weights))
print("In: {}".format(in_data))
print("Actual out: {},\n expected out: {}".format(output_buffer, expected_out))
print("Arrays are equal {}".format(np.array_equal(output_buffer, expected_out)))

del input_buffer, output_buffer
