import sys
import json
from generate_add_table_1d import generate_add_table
from generate_mul_table_1d import generate_mul_table

sourceCodeDir = "/mnt/ccnas2/tdp/tr720/final-year-project-workspace/final-year-project/design_exploration/convolution/"
print("Generating code from {}".format(sys.argv[1]))

with open(sys.argv[1], "r") as file:
    contents = file.read()
config = json.loads(contents)

def generateLayerConfig(config, i):
    layerConfig = config["Layer{}".format(i)]
    return """
const int LAYER_N_{} = {};
const int LAYER_IN_WIDTH_{} = {};
const int LAYER_DEFAULT_ADDS_{} = {};
const int LAYER_FABRIC_ADDS_{} = {};
const int LAYER_DSP_ADDS_{} = {};
const int LAYER_RAM_ADDS_{} = {};
const bool LAYER_DUAL_PORT_{} = {};
const bool LAYER_LUT_RAM_{} = {};
const int LAYER_BRAM_PARTITION_FACTOR_{} = {};
    """.format(i, layerConfig["N"], 
               i, layerConfig["InWidth"], 
               i, layerConfig["DefaultAdds"],
               i, layerConfig["FabricAdds"],
               i, layerConfig["DSPAdds"],
               i, layerConfig['RAMAdds'],
               i, "true" if layerConfig["DualPort"] else "false",
               i, "true" if layerConfig["LUTRAM"] else "false",
               i, layerConfig["BRAMPartitionFactor"])

layerConfigs=""
if not config["DefaultReductionOn"]:
    for i in range(config["NumLayers"]):
        layerConfigs += generateLayerConfig(config, i)

# Generate configuration
configH="""
#ifndef CONFIG
#define CONFIG

// General configurations
const int CONFIG_N = {};
const int CONFIG_IN_WIDTH = {};

// Pointwise mul configurations
const int DEFAULT_MULS = {};
const int FABRIC_MULS = {};
const int DSP_MULS = {};
const int RAM_MULS = {};
const bool MUL_DUAL_PORT = {};
const bool MUL_LUT_RAM = {};
const int MUL_BRAM_PARTITION_FACTOR= {};

// Additive reduction configurations
const int CONFIG_NUM_LAYERS = {};
const int CONFIG_NUM_PARTIAL_RESULTS = {};

const bool CONFIG_DEFAULT_REDUCTION = {};
const bool CONFIG_HOMOGENEOUS_DEFAULT = {};
const bool CONFIG_HOMOGENEOUS_FABRIC = {};
const bool CONFIG_HOMOGENEOUS_DSP = {};
const bool CONFIG_HOMOGENEOUS_RAM = {};
const bool CONFIG_HOMOGENEOUS_DUAL_PORT = {};
const bool CONFIG_HOMOGENEOUS_LUT_RAM = {};
const int CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR = {};

{}

#endif
""".format(
    config["N"],
    config["InWidth"],
    config["DefaultMuls"],
    config["FabricMuls"],
    config["DSPMuls"],
    config["RAMMuls"],
    "true" if config["MulRAMSettings"]["DualPort"] else "false",
    "true" if config["MulRAMSettings"]["LUTRAM"] else "false",
    config["MulRAMSettings"]["BRAMPartitionFactor"],
    config["NumLayers"],
    config["PartialResults"],
    "true" if config["DefaultReductionOn"] else "false",
    "true" if config["DefaultReductionSettings"]["Default"] else "false",
    "true" if config["DefaultReductionSettings"]["Fabric"] else "false",
    "true" if config["DefaultReductionSettings"]["DSP"] else "false",
    "true" if config["DefaultReductionSettings"]["RAM"] else "false",
    "true" if config["DefaultReductionSettings"]["RAMSettings"]["DualPort"] else "false",
    "true" if config["DefaultReductionSettings"]["RAMSettings"]["LUTRAM"] else "false",
    config["DefaultReductionSettings"]["RAMSettings"]["BRAMPartitionFactor"],
    layerConfigs
)

mulTableVar = "0"
mulTableInclude = ""
if config["RAMMuls"] > 0:
    mulTableVar = "mul_table"
    mulTableInclude = '#include "mul_table_{}.h"\n'.format(config["InWidth"])
    generate_mul_table(config["InWidth"])

homogeneousAddTable = None
layerAddTables = []
if config["DefaultReductionOn"] and config["DefaultReductionSettings"]["RAM"]:
   homogeneousAddTable = config["InWidth"] * 2 + config["NumLayers"] - 1
else:
    for i in range(config["NumLayers"]):
        if config["Layer{}".format(i)]["RAMAdds"] > 0:
            layerAddTables.append(config["Layer{}".format(i)]["InWidth"])
        else:
            layerAddTables.append(None)

homogeneousAddTableVar = "add_table_{}".format(homogeneousAddTable) if homogeneousAddTable else "0"

addTableIncludes = ""
if homogeneousAddTable:
    addTableIncludes += '#include "add_table_{}.h"\n'.format(homogeneousAddTable)
    generate_add_table(homogeneousAddTable)

for layerAddTable in layerAddTables:
    if layerAddTable:
        addTableIncludes += '#include "add_table_{}.h"\n'.format(layerAddTable)
        generate_add_table(layerAddTable)

layerProcessingCode = ""
if not config["DefaultReductionOn"]:
    for i in range(config["NumLayers"]):
        layerProcessingCode += "process_layer<LAYER_IN_WIDTH_{}, LAYER_N_{}, LAYER_DEFAULT_ADDS_{}, LAYER_FABRIC_ADDS_{}, LAYER_DSP_ADDS_{}, LAYER_RAM_ADDS_{}, LAYER_BRAM_PARTITION_FACTOR_{}, LAYER_DUAL_PORT_{}, LAYER_LUT_RAM_{}>(&layer_in_start, &layer_out_start, partial_sums, {});\n\t".format(
            i, i, i, i, i, i, i, i, i,
            "add_table_{}".format(layerAddTables[i]) if layerAddTables[i] else "0" 
        )

mainCpp = """
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "pointwise_mul.h"
#include "reduction_acc.h"

{}

int calculate_value(ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N], ap_uint<CONFIG_IN_WIDTH> prev_in[CONFIG_N]) {{
#pragma HLS inline off
#pragma HLS pipeline
	// Define an intermediate array to store multiplication results
	ap_uint<2 * CONFIG_IN_WIDTH> mul_results[CONFIG_N];
#pragma HLS ARRAY_PARTITION variable=mul_results complete

	pointwise_mul<CONFIG_IN_WIDTH, CONFIG_N, DEFAULT_MULS, FABRIC_MULS, DSP_MULS, RAM_MULS, MUL_BRAM_PARTITION_FACTOR, MUL_DUAL_PORT, MUL_LUT_RAM>(weights, prev_in, mul_results, {});
	ap_uint<32> total = reduction_acc(mul_results);
	return total;
}}

void example(hls::stream< ap_axis<32,2,5,6> > &A,
	     hls::stream< ap_axis<32,2,5,6> > &B) {{
#pragma HLS INTERFACE axis port=A
#pragma HLS INTERFACE axis port=B

#pragma HLS INTERFACE s_axilite port=return

	ap_uint<CONFIG_IN_WIDTH> weights[CONFIG_N];
	ap_uint<CONFIG_IN_WIDTH> prev_in[CONFIG_N];
#pragma HLS ARRAY_PARTITION variable=weights complete
#pragma HLS ARRAY_PARTITION variable=prev_in complete

    // Zero-initialise arrays
	for (int i = 0; i < CONFIG_N; i++) {{
#pragma HLS unroll
		weights[i] = 0;
		prev_in[i] = 0;
	}}

	// Dummy call to fill BRAM pipelines
	volatile ap_uint<32> dummy_result = calculate_value(weights, prev_in);

	int inCount = 0;

	ap_axis<32,2,5,6> tmp;
	ap_axis<32,2,5,6> tmp2;
    while(1) {{
    	A.read(tmp);

    	if (inCount < CONFIG_N) {{
    		weights[inCount] = tmp.data.range(CONFIG_IN_WIDTH - 1, 0);
	    	inCount++;
	    }}

    	for (int i = CONFIG_N - 1; i > 0; i--) {{
#pragma HLS unroll
    		prev_in[i] = prev_in[i - 1];
    	}}
    	prev_in[0] = tmp.data.range(2 * CONFIG_IN_WIDTH - 1, CONFIG_IN_WIDTH);

    	tmp2.keep = tmp.keep;
    	tmp2.strb = tmp.strb;
    	tmp2.user = tmp.user;
    	tmp2.id = tmp.id;
    	tmp2.last = tmp.last;
    	tmp2.dest = tmp.dest;


    	ap_uint<32> to_return = calculate_value(weights, prev_in);
    	tmp2.data = to_return;
    	B.write(tmp2);

    	if (tmp2.last) {{
    	    break;
    	}}
    }}
}}
""".format(mulTableInclude, mulTableVar)

reductionAccCpp = """
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "config.h"
#include "ap_int.h"
#include "pointwise_add.h"
#include "reduction_acc.h"

{}

ap_uint<32> reduction_acc(ap_uint<CONFIG_IN_WIDTH * 2> values[CONFIG_N]) {{
#pragma HLS inline off
#pragma HLS pipeline
#pragma HLS ARRAY_PARTITION variable=values complete

    if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_DEFAULT) {{
        ap_uint<32> total = 0;
        for (int i = 0; i < CONFIG_N; i++) {{
#pragma HLS unroll
		    total += values[i];
	    }}
	    return total;
    }} else if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_FABRIC) {{
        ap_uint<32> total_fabric = 0;
#pragma HLS RESOURCE variable=total_fabric core=AddSub
        for (int i = 0; i < CONFIG_N; i++) {{
#pragma HLS unroll
		    total_fabric += values[i];
	    }}
	    return total_fabric;
    }} else if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_DSP) {{
        ap_uint<32> total_dsp = 0;
#pragma HLS RESOURCE variable=total_dsp core=AddSub_DSP
        for (int i = 0; i < CONFIG_N; i++) {{
#pragma HLS unroll
		    total_dsp += values[i];
	    }}
	    return total_dsp;
    }} else if (CONFIG_DEFAULT_REDUCTION && CONFIG_HOMOGENEOUS_RAM) {{
        ap_uint<32> total = 0;
        for (int i = 0; i < CONFIG_N; i++) {{
#pragma HLS unroll
            if (CONFIG_HOMOGENEOUS_DUAL_PORT) {{
                if (CONFIG_HOMOGENEOUS_LUT_RAM) {{
                    total = lutram_dual_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1>(total, values[i], {});
                }} else {{
                    total = bram_dual_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1, CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR>(total, values[i], {});
                }}
            }} else {{
                if (CONFIG_HOMOGENEOUS_LUT_RAM) {{
                    total = lutram_single_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1>(total, values[i], {});
                }} else {{
                    total = bram_single_port_add<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS - 1, CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR>(total, values[i], {});
                }}
            }}
        }}
        return total;
    }}

    // Flattened array for output values, all given the largest width
    ap_uint<CONFIG_IN_WIDTH * 2 + CONFIG_NUM_LAYERS> partial_sums[CONFIG_NUM_PARTIAL_RESULTS];
#pragma HLS ARRAY_PARTITION variable=partial_sums complete

    for (int i = 0; i < CONFIG_NUM_PARTIAL_RESULTS; i++) {{
#pragma HLS unroll
        partial_sums[i] = 0;
    }}

    for (int i = 0; i < CONFIG_N; i++) {{
#pragma HLS unroll
            partial_sums[i] = values[i];
    }}

    int layer_in_start = 0;
    int layer_out_start = CONFIG_N;

    {}

    return partial_sums[CONFIG_NUM_PARTIAL_RESULTS - 1];
}}
""".format(addTableIncludes, homogeneousAddTableVar, homogeneousAddTableVar, homogeneousAddTableVar, homogeneousAddTableVar, layerProcessingCode)


with open(sourceCodeDir + "config.h", "w") as file:
    file.write(configH)

print("Updated config.h")

with open(sourceCodeDir + "main.cpp", "w") as file:
    file.write(mainCpp)

print("Updated main.cpp")

with open(sourceCodeDir + "reduction_acc.cpp", "w") as file:
    file.write(reductionAccCpp)

print("Updated reduction_acc.cpp")