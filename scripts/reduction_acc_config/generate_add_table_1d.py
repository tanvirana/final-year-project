import sys
source_code_dir = "/mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/reduction_acc/"

def generate_add_table(in_width):
  num_entries = 2**(2*in_width)
  dim_size = 2**in_width
  add_table = [0 for _ in range(num_entries)]

  for i in range(dim_size):
    for j in range(dim_size):
      add_table[(i << in_width) | j] = i + j

  with open(source_code_dir + "add_table_{}.h".format(in_width), "w") as file:
    file.write("const ap_uint<{}> add_table_{}[{}] = {{\n".format(in_width+1, in_width, num_entries))
    for i in range(num_entries):
      if i != 0 and i % dim_size == 0:
        file.write("\n")
      file.write(str(add_table[i]))
      if i != num_entries - 1:
        file.write(",")
    file.write("\n};")

  print("Generated add table for input bit width {}".format(in_width))