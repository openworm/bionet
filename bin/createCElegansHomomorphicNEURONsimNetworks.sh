<<<<<<< HEAD
if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe \
-simDir ../sim -simHocFile CElegans.hoc -loadNetwork CElegans_network.txt \
-populationSize 4 -numOffspring 2 -numGenerations 1 -crossoverRate .8 \
-mutationRate .05 -synapseWeights 0.0 1.0 .1 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log \
-randomSeed 4518 -numThreads 2
else
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe \
-simDir ../sim -simHocFile CElegans.hoc -loadNetwork CElegans_network.txt \
-populationSize 4 -numOffspring 2 -numGenerations 1 -crossoverRate .8 \
-mutationRate .05 -synapseWeights 0.0 1.0 .1 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log \
-randomSeed ${@} -numThreads 2
fi

=======
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe -simDir ../sim -simHocFile CElegans.hoc -loadNetwork CElegans_network.txt -populationSize 500 -numOffspring 250 -numGenerations 100 -crossoverRate .9 -mutationRate .05 -synapseWeights 0.0 1.0 .1 -synapseCrossoverBondStrength .5 -synapseOptimizedPathLength 2 -saveMorph morph0.txt -logMorph morph0.log -randomSeed 4518 -numThreads 2
>>>>>>> f9f95897345835d82b6d61f6689f09d0d2167f27
