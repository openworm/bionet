if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt -synapseWeights 0.0 .25 .1 0.05 -populationSize 25 -numMutants 5 -numOffspring 5 -numGenerations 50 -fitnessQuorum 1 -saveNetworks
else
./bionet -createHomomorphicNetworks -loadBehaviors CElegans_behaviors.txt -loadNetwork CElegans_network.txt -synapseWeights 0.0 .25 .1 0.05 -populationSize 25 -numMutants 5 -numOffspring 5 -numGenerations 50 -fitnessQuorum 1 -saveNetworks -randomSeed ${@}
fi
