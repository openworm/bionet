if [ $# -eq 0 ]
then
./bionet -createMorphicNetworks -loadBehaviors behaviors.txt -excitatoryNeurons 5 20 1 0.05 -inhibitoryNeurons 0 10 1 0.05 -synapsePropensities .1 .5 .05 .05 -synapseWeights .1 0.01 -populationSize 20 -numOffspring 5 -numGenerations 2 -saveNetworks
else
./bionet -createMorphicNetworks -loadBehaviors behaviors.txt -excitatoryNeurons 5 20 1 0.05 -inhibitoryNeurons 0 10 1 0.05 -synapsePropensities .1 .5 .05 .05 -synapseWeights .1 0.01 -populationSize 20 -numOffspring 5 -numGenerations 2 -saveNetworks -randomSeed ${@}
fi
