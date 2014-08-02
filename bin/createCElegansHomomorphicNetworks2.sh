if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 2 -numOffspring 1 -numGenerations 2 -behaveCutoff 50 \
-crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morphx.txt -numThreads 1
else
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt \
-synapseWeights 0.0 .25 .1 -populationSize 2 -numOffspring 1 -numGenerations 2 -behaveCutoff 50 \
-crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morphx.txt -randomSeed ${@} -numThreads 1
fi

