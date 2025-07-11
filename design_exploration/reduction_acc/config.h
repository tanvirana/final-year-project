
#ifndef CONFIG
#define CONFIG

#include <cmath>

const bool CONFIG_DEFAULT_REDUCTION = false;
const bool CONFIG_HOMOGENEOUS_DEFAULT = false;
const bool CONFIG_HOMOGENEOUS_FABRIC = false;
const bool CONFIG_HOMOGENEOUS_DSP = false;
const bool CONFIG_HOMOGENEOUS_RAM = false;
const bool CONFIG_HOMOGENEOUS_DUAL_PORT = false;
const bool CONFIG_HOMOGENEOUS_LUT_RAM = false;
const int CONFIG_HOMOGENEOUS_BRAM_PARTITION_FACTOR = 1;

const int CONFIG_N = 5;
const int CONFIG_IN_WIDTH = 6;
const int CONFIG_NUM_LAYERS = 3;

// NON-HOMOGENEOUS CONFIGS
const int CONFIG_NUM_PARTIAL_RESULTS = 11;


const int LAYER_N_0 = 5;
const int LAYER_IN_WIDTH_0 = 6;
const int LAYER_DEFAULT_ADDS_0 = 2;
const int LAYER_FABRIC_ADDS_0 = 0;
const int LAYER_DSP_ADDS_0 = 0;
const int LAYER_RAM_ADDS_0 = 0;
const int LAYER_BRAM_PARTITION_FACTOR_0 = 1;
const bool LAYER_DUAL_PORT_0 = false;
const bool LAYER_LUT_RAM_0 = false;
    
const int LAYER_N_1 = 3;
const int LAYER_IN_WIDTH_1 = 7;
const int LAYER_DEFAULT_ADDS_1 = 0;
const int LAYER_FABRIC_ADDS_1 = 1;
const int LAYER_DSP_ADDS_1 = 0;
const int LAYER_RAM_ADDS_1 = 0;
const int LAYER_BRAM_PARTITION_FACTOR_1 = 1;
const bool LAYER_DUAL_PORT_1 = false;
const bool LAYER_LUT_RAM_1 = false;
    
const int LAYER_N_2 = 2;
const int LAYER_IN_WIDTH_2 = 8;
const int LAYER_DEFAULT_ADDS_2 = 0;
const int LAYER_FABRIC_ADDS_2 = 0;
const int LAYER_DSP_ADDS_2 = 0;
const int LAYER_RAM_ADDS_2 = 1;
const int LAYER_BRAM_PARTITION_FACTOR_2 = 1;
const bool LAYER_DUAL_PORT_2 = true;
const bool LAYER_LUT_RAM_2 = false;
    

#endif
