if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -locomotionMovements 20 -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 1000 \
-crossoverRate .5 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph.txt -numThreads 2
else
./bionet -createHomomorphicNetworks -locomotionMovements 20 -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 1000 \
-crossoverRate .5 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph.txt -randomSeed ${@} -numThreads 2
fi
