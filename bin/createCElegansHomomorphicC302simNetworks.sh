if [ "$TERM" = "cygwin" ]
then
if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks  -jnmlCmd ../../jNeuroML/jnml.bat \
-simDir ../sim/CElegansNeuroML -loadNetwork CElegans_network.txt \
-populationSize 4 -numOffspring 2 -numGenerations 2 -crossoverRate .8 \
-mutationRate .05 -synapseWeights 0.0 1.0 0.1 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log \
-randomSeed 4518 -numThreads 1
else
./bionet -createHomomorphicNetworks  -jnmlCmd ../../jNeuroML/jnml.bat \
-simDir ../sim/CElegansNeuroML -loadNetwork CElegans_network.txt \
-populationSize 4 -numOffspring 2 -numGenerations 2 -crossoverRate .8 \
-mutationRate .05 -synapseWeights 0.0 1.0 0.1 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log \
-randomSeed ${@} -numThreads 1
fi
else
if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks  -jnmlCmd ../../jNeuroML/jnml \
-simDir ../sim/CElegansNeuroML -loadNetwork CElegans_network.txt \
-populationSize 4 -numOffspring 2 -numGenerations 2 -crossoverRate .8 \
-mutationRate .05 -synapseWeights 0.0 1.0 0.1 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log \
-randomSeed 4518 -numThreads 1
else
./bionet -createHomomorphicNetworks  -jnmlCmd ../../jNeuroML/jnml \
-simDir ../sim/CElegansNeuroML -loadNetwork CElegans_network.txt \
-populationSize 4 -numOffspring 2 -numGenerations 2 -crossoverRate .8 \
-mutationRate .05 -synapseWeights 0.0 1.0 0.1 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log \
-randomSeed ${@} -numThreads 1
fi
fi


