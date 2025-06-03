source_code_dir = "/mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/convolution/"

def generate_mul_table(in_width):
  num_entries = 2**(2*in_width)
  dim_size = 2**in_width
  mul_table = [0 for _ in range(num_entries)]

  for i in range(dim_size):
    for j in range(dim_size):
      mul_table[(i << in_width) | j] = i * j

  with open(source_code_dir + "mul_table_{}.h".format(in_width), "w") as file:
    file.write("const ap_uint<{}> mul_table[{}] = {{\n".format(2*in_width, num_entries))
    for i in range(num_entries):
      if i != 0 and i % dim_size == 0:
        file.write("\n")
      file.write(str(mul_table[i]))
      if i != num_entries - 1:
        file.write(",")
    file.write("\n};")
    
  print("Generated mul table for input bit width {}".format(in_width))