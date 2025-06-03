import argparse
import math
import json

config = {
    "N": 0,
    "NumLayers": 0,
    "InWidth": 0,
    "DefaultMuls": 0,
    "FabricMuls": 0,
    "DSPMuls": 0,
    "RAMMuls": 0,
    "MulRAMSettings": {
        "DualPort": False,
        "LUTRAM": False,
        "BRAMPartitionFactor": 1
    },
    "PartialResults": 0,
    "DefaultReductionOn": False,
    "DefaultReductionSettings": {
        "Default": False,
        "Fabric": False,
        "DSP": False,
        "RAM": False,
        "RAMSettings": {
           "DualPort": False,
           "LUTRAM": False,
           "BRAMPartitionFactor": 1
        }
    }
}
layerConfig = {
    "N": 0,
    "InWidth": 0,
    "DefaultAdds": 0,
    "FabricAdds": 0,
    "DSPAdds": 0,
    "RAMAdds": 0,
    "DualPort": False,
    "LUTRAM": False,
    "BRAMPartitionFactor": 1
}

def getNumLayers(N):
    numLayers = int(math.ceil(math.log(N, 2)))
    return numLayers

parser = argparse.ArgumentParser(
                prog='Heterogeneous Accumulative Reduction Configuration Tool',
                description="""Build informed heterogeneous strategies for an accumulative reduction module.""")
parser.add_argument("--json", help="See example_config.json for configuration structure. Without this argument, the interactive tool will guide you in building a configuration")
args = parser.parse_args()

while True:
    try:
        N = int(input("Enter N (number of values to sum): "))
        numLayers = getNumLayers(N)
        print("With {} input values, {} reduction tree layers".format(N, numLayers))
        print("")
        break
    except:
        print("N must be an integer.")
config["N"] = N
config["NumLayers"] = numLayers

while True:
    try:
        inWidth = int(input("Enter bit width of input values: "))
        print("")
        break
    except:
        print("Input bitwidth must be an integer.")
config["InWidth"] = inWidth

print("There are {} two-input multiplication units.".format(N))
print("")

while True:
    totalMuls = 0
    numDefault = 0
    numFabric = 0
    numDsp = 0
    numRam = 0
    try:
        numDefault = int(input("Enter number of default muls: "))
        totalMuls += numDefault
        assert(totalMuls <= N)
        numFabric = int(input("Enter number of fabric muls: "))
        totalMuls += numFabric
        assert(totalMuls <= N)
        numDsp = int(input("Enter number of DSP muls: "))
        totalMuls += numDsp
        assert(totalMuls <= N)
        numRam = int(input("Enter number of RAM muls: "))
        totalMuls += numRam
        assert(totalMuls == N)
        break
    except:
        print("Try again - inputs must be ints and number of muls must sum to {}".format(N))
        print("")
        
config["DefaultMuls"] = numDefault
config["FabricMuls"] = numFabric
config["DSPMuls"] = numDsp
config["RAMMuls"] = numRam

if numRam > 0:
    dualPort = ""
    while dualPort not in ["y", "n"]:
        dualPort = raw_input("Dual port? (y/n) ")
    config["MulRAMSettings"]["DualPort"] = dualPort == "y"
    lutRam = ""
    while lutRam not in ["y", "n"]:
        lutRam = raw_input("LUT RAM? (y/n) ")
    lutRam = lutRam == "y"
    config["MulRAMSettings"]["LUTRAM"] = lutRam

if not lutRam:
    while True:
        try:
            partitionFactor = int(input("Enter BRAM partition factor (1 for no partitioning): "))
            print("")
            assert(partitionFactor > 0)
            break
        except:
            print("Partition factor must be an integer.")
    config["MulRAMSettings"]["BRAMPartitionFactor"] = partitionFactor 

print("Add layer structure is as follows.")
layerNs = []
layerAdds = []
valuesLeft = N
for i in range(numLayers):
    print("Layer {}".format(i))
    layerNs.append(valuesLeft)
    print("N: {}".format(valuesLeft))
    curLayerAdds = valuesLeft // 2
    layerAdds.append(curLayerAdds)
    print("Adds: {}".format(curLayerAdds))
    valuesLeft -= curLayerAdds
    print("")

print("There are {} two-input addition units in the reduction tree.".format(sum(layerAdds)))
print("")

partialResults = sum(layerNs) + 1
config["PartialResults"] = partialResults

layerInWidths = [inWidth * 2 + i for i in range(numLayers)]
print("Add layer bitwidths are {}".format(layerInWidths))
print("")

defaultReduction = ""
while defaultReduction not in ["y", "n"]:
    defaultReduction = raw_input("Use homogeneous default reduction? (y/n) ")
    print("")
defaultReduction = defaultReduction == "y"
config["DefaultReductionOn"] = defaultReduction

if defaultReduction:
    while True:
        try:
            print("Homogenous stragies")
            print("0 - Default")
            print("1 - Fabric")
            print("2 - DSPs")
            print("3 - RAM")
            homogeneousStrategySelection = int(input("Select a strategy (0/1/2/3): "))
            print("")
            if homogeneousStrategySelection in range(0,4):
                break
        except:
            print("Input bitwidth must be an integer.")

    config["DefaultReductionSettings"]["Default"] = homogeneousStrategySelection == 0
    config["DefaultReductionSettings"]["Fabric"] = homogeneousStrategySelection == 1
    config["DefaultReductionSettings"]["DSP"] = homogeneousStrategySelection == 2

    if homogeneousStrategySelection == 3:

        config["DefaultReductionSettings"]["RAM"] = True
        dualPort = ""
        while dualPort not in ["y", "n"]:
            dualPort = raw_input("Dual port? (y/n) ")
        config["DefaultReductionSettings"]["RAMSettings"]["DualPort"] = dualPort == "y"
        lutRam = ""
        while lutRam not in ["y", "n"]:
            lutRam = raw_input("LUT RAM? (y/n) ")
        lutRam = lutRam == "y"
        config["DefaultReductionSettings"]["RAMSettings"]["LUTRAM"] = lutRam

        if not lutRam:
            while True:
                try:
                    partitionFactor = int(input("Enter BRAM partition factor (1 for no partitioning): "))
                    print("")
                    assert(partitionFactor > 0)
                    break
                except:
                    print("Partition factor must be an integer.")
            config["DefaultReductionSettings"]["RAMSettings"]["BRAMPartitionFactor"] = partitionFactor 
            
else:

    for i in range(numLayers):
        curLayerConfig = layerConfig.copy()
        config["Layer{}".format(i)] = curLayerConfig
        curLayerConfig["N"] = layerNs[i]
        curLayerConfig["InWidth"] = layerInWidths[i]
        
        print("Layer {}, N = {}, In Width = {}, Adds = {}".format(i, layerNs[i], layerInWidths[i], layerAdds[i]))
        print("")

        while True:
            totalAdds = 0
            numDefault = 0
            numFabric = 0
            numDsp = 0
            numRam = 0
            try:
                numDefault = int(input("Enter number of default adds: "))
                totalAdds += numDefault
                assert(totalAdds <= layerAdds[i])
                numFabric = int(input("Enter number of fabric adds: "))
                totalAdds += numFabric
                assert(totalAdds <= layerAdds[i])
                numDsp = int(input("Enter number of DSP adds: "))
                totalAdds += numDsp
                assert(totalAdds <= layerAdds[i])
                numRam = int(input("Enter number of RAM adds: "))
                totalAdds += numRam
                assert(totalAdds == layerAdds[i])
                break
            except:
                print("Try again - inputs must be ints and number of adds must sum to {}".format(layerAdds[i]))
                print("")
        
        curLayerConfig["DefaultAdds"] = numDefault
        curLayerConfig["FabricAdds"] = numFabric
        curLayerConfig["DSPAdds"] = numDsp
        curLayerConfig["RAMAdds"] = numRam

        if numRam > 0:
            dualPort = ""
            while dualPort not in ["y", "n"]:
                dualPort = raw_input("Dual port? (y/n) ")
            curLayerConfig["DualPort"] = dualPort == "y"
            lutRam = ""
            while lutRam not in ["y", "n"]:
                lutRam = raw_input("LUT RAM? (y/n) ")
            lutRam = lutRam == "y"
            curLayerConfig["LUTRAM"] = lutRam

            if not lutRam:
                while True:
                    try:
                        partitionFactor = int(input("Enter BRAM partition factor (1 for no partitioning): "))
                        print("")
                        assert(partitionFactor > 0)
                        break
                    except:
                        print("Partition factor must be an integer.")
                curLayerConfig["BRAMPartitionFactor"] = partitionFactor 

with open("generated_config.json", "w") as file:
    file.write(json.dumps(config, indent=2))     