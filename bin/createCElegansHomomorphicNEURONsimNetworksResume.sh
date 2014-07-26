<<<<<<< HEAD
if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe \
-simDir ../sim -simHocFile CElegans.hoc -loadMorph morph0.txt \
-numGenerations 1 -crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 2
else
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe \
-simDir ../sim -simHocFile CElegans.hoc -loadMorph morph0.txt \
-numGenerations 1 -crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 2 -randomSeed ${@}
fi
=======
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe -simDir ../sim -simHocFile CElegans.hoc -loadMorph morph0.txt -numGenerations 100 -crossoverRate .9 -mutationRate .05 -synapseCrossoverBondStrength .5 -synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 2
>>>>>>> f9f95897345835d82b6d61f6689f09d0d2167f27
