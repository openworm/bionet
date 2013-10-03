if [ $# -eq 0 ]
then
./bionet -createHomomorphicNetworks -loadBehaviors behaviors.txt -loadNetwork network.txt -synapseWeights 0.0 1.0 .1 0.05 -populationSize 25 -numMutants 5 -numOffspring 5 -numGenerations 1000 -fitnessQuorum 1 -saveNetworks
else
./bionet -createHomomorphicNetworks -loadBehaviors behaviors.txt -loadNetwork network.txt -synapseWeights 0.0 1.0 .1 0.05 -populationSize 25 -numMutants 5 -numOffspring 5 -numGenerations 1000 -fitnessQuorum 1 -saveNetworks -randomSeed ${@}
fi
