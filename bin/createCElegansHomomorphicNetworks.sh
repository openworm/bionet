if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 250 -behaveCutoff 50 \
-crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph0.txt -numThreads 256
else
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 500 -numOffspring 250 -numGenerations 250 -behaveCutoff 50 \
-crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph0.txt -numThreads 256 -randomSeed ${@}
fi

