if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -undulationMovements 25 -loadMorph morph0.txt \
-numGenerations 250 \
-crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph1.txt -numThreads 2
else
./bionet -createHomomorphicNetworks -undulationMovements 25 -loadMorph morph0.txt \
-numGenerations 250 \
-crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph1.txt -randomSeed ${@} -numThreads 2
fi
