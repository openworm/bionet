bionet.exe -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt -synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 1000 -crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 -synapseOptimizedPathLength 4 -saveMorph morph.txt
@pause


