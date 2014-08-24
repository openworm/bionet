if [ "$TERM" = "cygwin" ]
then
./bionet -createHomomorphicNetworks  -jnmlCmd ../../jNeuroML/jnml.bat \
-simDir ../sim/CElegansNeuroML -loadMorph morph0.txt \
-numGenerations 2 -crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 1
else
./bionet -createHomomorphicNetworks  -jnmlCmd ../../jNeuroML/jnml \
-simDir ../sim/CElegansNeuroML -loadMorph morph0.txt \
-numGenerations 2 -crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 1
fi



