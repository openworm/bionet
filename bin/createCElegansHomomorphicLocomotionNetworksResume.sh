if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -locomotionMovements 20 -loadMorph morph0.txt \
-numGenerations 1000 \
-crossoverRate .5 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph1.txt -numThreads 2
else
./bionet -createHomomorphicNetworks -locomotionMovements 20 -loadMorph morph0.txt \
-numGenerations 1000 \
-crossoverRate .5 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph1.txt -randomSeed ${@} -numThreads 2
fi
