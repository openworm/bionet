if [ $# -eq 0 ]
then
./bionet -createMorphicNetworks -loadBehaviors behaviors.txt -excitatoryNeurons 8 8 0 0.0 -inhibitoryNeurons 4 4 0 0.0 -synapsePropensities .13 .13 .0 .0 -synapseWeights .1 0.05 -populationSize 20 -numOffspring 5 -numGenerations 1000 -saveNetworks -homomorphClones -loadNetwork network.txt
else
./bionet -createMorphicNetworks -loadBehaviors behaviors.txt -excitatoryNeurons 8 8 0 0.0 -inhibitoryNeurons 4 4 0 0.0 -synapsePropensities .13 .13 .0 .0 -synapseWeights .1 0.05 -populationSize 20 -numOffspring 5 -numGenerations 1000 -saveNetworks -homomorphClones -loadNetwork network.txt -randomSeed ${@}
fi
