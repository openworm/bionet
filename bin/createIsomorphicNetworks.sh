if [ $# -eq 0 ]
then
./bionet -createIsomorphicNetworks -loadBehaviors behaviors.txt -excitatoryNeurons 5 20 1 0.05 -inhibitoryNeurons 0 10 1 0.05 -synapsePropensities .1 .5 .05 .05 -synapseWeights 0.0 1.0 .1 0.01 -populationSize 20 -numMutants 5 -numGenerations 100 -saveNetworks
else
./bionet -createIsomorphicNetworks -loadBehaviors behaviors.txt -excitatoryNeurons 5 20 1 0.05 -inhibitoryNeurons 0 10 1 0.05 -synapsePropensities .1 .5 .05 .05 -synapseWeights 0.0 1.0 .1 0.01 -populationSize 20 -numMutants 5 -numGenerations 100 -saveNetworks -randomSeed ${@}
fi
