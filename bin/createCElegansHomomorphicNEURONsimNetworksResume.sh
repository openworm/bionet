if [ "$TERM" = "cygwin" ]
then
./bionet -createHomomorphicNetworks  -neuronExec ../../nrn73/bin/neuron.exe \
-simDir ../sim -simHocFile CElegans.hoc -loadMorph morph0.txt \
-numGenerations 2 -crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 1
else
./bionet -createHomomorphicNetworks  -neuronExec /usr/local/bin/nrniv \
-simDir ../sim -simHocFile CElegans.hoc -loadMorph morph0.txt \
-numGenerations 2 -crossoverRate .8 -mutationRate .05 -synapseCrossoverBondStrength .5 \
-synapseOptimizedPathLength 2 -saveMorph morph1.txt -logMorph morph1.log -numThreads 1
fi

