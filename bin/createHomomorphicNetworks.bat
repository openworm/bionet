bionet.exe -createHomomorphicNetworks -loadBehaviors behaviors.txt -loadNetwork network.txt -synapseWeights 0.0 1.0 .1 -populationSize 25 -numOffspring 10 -numGenerations 1000 -behaveCutoff 5 -crossoverRate .5 -mutationRate .05 -synapseCrossoverBondStrength .5 -synapseOptimizedPathLength 4 -saveMorph morph.txt
@pause

