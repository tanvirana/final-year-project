import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--in_width", type=int, required=True)
parser.add_argument("--N", type=int, required=True)
parser.add_argument("--default_muls", type=int, required=True)
parser.add_argument("--fabric_muls", type=int, required=True)
parser.add_argument("--dsp_muls", type=int, required=True)
parser.add_argument("--ram_muls", type=int, required=True)
parser.add_argument("--ram_partition_factor", type=int, required=True)
parser.add_argument("--dual_port", required=True)
parser.add_argument("--lut_ram", required=True)

args = parser.parse_args()

mulTableImport = '#define MUL_TABLE_HEADER "mul_table_{}.h"'.format(args.in_width) if args.ram_muls > 0 else ""
configText = """#ifndef CONFIG
#define CONFIG

const int N = {};
const int IN_WIDTH = {};
const int DEFAULT_MULS = {};
const int FABRIC_MULS = {};
const int DSP_MULS = {};
const int RAM_MULS = {};
const int RAM_PARTITION_FACTOR = {};
const bool DUAL_PORT = {};
const bool LUT_RAM = {};

{}
#endif""".format(args.N, 
           args.in_width, 
           args.default_muls, 
           args.fabric_muls, 
           args.dsp_muls, 
           args.ram_muls, 
           args.ram_partition_factor,
           args.dual_port,
           args.lut_ram,
           mulTableImport)

with open("/mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/pointwise_mul/config.h", "w") as file:
    file.write(configText)