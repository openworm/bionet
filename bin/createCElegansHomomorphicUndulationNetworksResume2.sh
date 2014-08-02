if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -undulationMovements 100 -loadMorph morph_twitch100_0.txt \
-numGenerations 0 \
-crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph1.txt -numThreads 1
else
./bionet -createHomomorphicNetworks -undulationMovements 100 -loadMorph morph_head0.txt \
-numGenerations 0 \
-crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 4 -saveMorph morph1.txt -randomSeed ${@} -numThreads 1
fi
