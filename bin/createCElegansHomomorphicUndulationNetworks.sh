if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -undulationMovements 25 -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 250 \
-crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph0.txt -numThreads 2
else
./bionet -createHomomorphicNetworks -undulationMovements 25 -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 250 \
-crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph0.txt -randomSeed ${@} -numThreads 2
fi
