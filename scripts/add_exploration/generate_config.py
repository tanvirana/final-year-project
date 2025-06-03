import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--in_width", type=int, required=True)
parser.add_argument("--N", type=int, required=True)
parser.add_argument("--default_adds", type=int, required=True)
parser.add_argument("--fabric_adds", type=int, required=True)
parser.add_argument("--dsp_adds", type=int, required=True)
parser.add_argument("--ram_adds", type=int, required=True)
parser.add_argument("--ram_partition_factor", type=int, required=True)
parser.add_argument("--dual_port", required=True)
parser.add_argument("--lut_ram", required=True)

args = parser.parse_args()

addTableImport = '#define ADD_TABLE_HEADER "add_table_{}.h"'.format(args.in_width) if args.ram_adds > 0 else ""
configText = """#ifndef CONFIG
#define CONFIG

const int N = {};
const int IN_WIDTH = {};
const int DEFAULT_ADDS = {};
const int FABRIC_ADDS = {};
const int DSP_ADDS = {};
const int RAM_ADDS = {};
const int RAM_PARTITION_FACTOR= {};
const bool DUAL_PORT = {};
const bool LUT_RAM = {};

{}
#endif""".format(args.N, 
           args.in_width, 
           args.default_adds, 
           args.fabric_adds, 
           args.dsp_adds, 
           args.ram_adds, 
           args.ram_partition_factor,
           args.dual_port,
           args.lut_ram,
           addTableImport)

with open("/mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/pointwise_add/config.h", "w") as file:
    file.write(configText)