
#ifndef CONFIG
#define CONFIG

// General configurations
const int CONFIG_N = 10;
const int CONFIG_IN_WIDTH = 6;

// Pointwise mul configurations
const int DEFAULT_MULS = 3;
const int FABRIC_MULS = 3;
const int DSP_MULS = 0;
const int RAM_MULS = 4;
const bool MUL_DUAL_PORT = true;
const bool MUL_LUT_RAM = false;
const int MUL_BRAM_PARTITION_FACTOR= 4;

// Additive reduction configurations
const int CONFIG_NUM_LAYERS = 4;
const int CONFIG_NUM_PARTIAL_RESULTS = 21;

const bool CONFIG_DEFAULT_REDUCTION = false;
const bool CONFIG_HOMOGENEOUS_DEFAULT = false;
const bool CONFIG_HOMOGENEOUS_FABRIC = false;
const bool CONFIG_HOMOGENEOUS_DSP = false;
const bool CONFIG_HOMOGENEOUS_RAM = false;
const bool CONFIG_HOMOGENEOUS_DUAL_PORT = false;
const bool CONFIG_HOMOGENEOUS_LUT_RAM = false;
const int CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR = 1;


const int LAYER_N_0 = 10;
const int LAYER_IN_WIDTH_0 = 12;
const int LAYER_DEFAULT_ADDS_0 = 2;
const int LAYER_FABRIC_ADDS_0 = 0;
const int LAYER_DSP_ADDS_0 = 3;
const int LAYER_RAM_ADDS_0 = 0;
const bool LAYER_DUAL_PORT_0 = false;
const bool LAYER_LUT_RAM_0 = false;
const int LAYER_BRAM_PARTITION_FACTOR_0 = 1;
    
const int LAYER_N_1 = 5;
const int LAYER_IN_WIDTH_1 = 13;
const int LAYER_DEFAULT_ADDS_1 = 0;
const int LAYER_FABRIC_ADDS_1 = 1;
const int LAYER_DSP_ADDS_1 = 1;
const int LAYER_RAM_ADDS_1 = 0;
const bool LAYER_DUAL_PORT_1 = false;
const bool LAYER_LUT_RAM_1 = false;
const int LAYER_BRAM_PARTITION_FACTOR_1 = 1;
    
const int LAYER_N_2 = 3;
const int LAYER_IN_WIDTH_2 = 14;
const int LAYER_DEFAULT_ADDS_2 = 1;
const int LAYER_FABRIC_ADDS_2 = 0;
const int LAYER_DSP_ADDS_2 = 0;
const int LAYER_RAM_ADDS_2 = 0;
const bool LAYER_DUAL_PORT_2 = false;
const bool LAYER_LUT_RAM_2 = false;
const int LAYER_BRAM_PARTITION_FACTOR_2 = 1;
    
const int LAYER_N_3 = 2;
const int LAYER_IN_WIDTH_3 = 15;
const int LAYER_DEFAULT_ADDS_3 = 0;
const int LAYER_FABRIC_ADDS_3 = 1;
const int LAYER_DSP_ADDS_3 = 0;
const int LAYER_RAM_ADDS_3 = 0;
const bool LAYER_DUAL_PORT_3 = false;
const bool LAYER_LUT_RAM_3 = false;
const int LAYER_BRAM_PARTITION_FACTOR_3 = 1;
    

#endif
