bionet
======

Artificial biological neural network

bionet simulates a biologically inspired neural network composed of
sensory neurons, interneurons, and motor neurons which can be connected
in arbitrary configurations. Signals propagate through the network one
synapse at a time. A network behavior is defined as a sequence of
sensory-motor neuron firings.

bionet also possesses a network morphogenesis capability:
An isomorphic network reproduces the behaviors of another network
without knowing its interneuron configuration. A homomorphic network
additionally mirrors the interneuron configuration of the other network.

Required packages:

The UNIX version requires the gcc compiler, make command, and
the bash shell. The Windows version requires the Microsoft Visual
C++ 2012 (or later) IDE.

To build:

UNIX: type 'make'
Windows: use VC++ solution and project files.

Commands:

Create network:

bionet
   -createNetwork
   -numNeurons <number of neurons>
   -numSensors <number of sensors>
   -numMotors <number of motors>
   [-inhibitorDensity <inhibitory neuron density [0.0,1.0]>]
   [-synapsePropensity <synapse generation propensity (0.0,1.0]>]
   [-randomSeed <random seed>]
   [-saveNetwork <network file name>]
   [-graphNetwork <graph file name ('dot' format)>]

Print network:

bionet
   -printNetwork
   -loadNetwork <network file name>

Graph network:

bionet
   -graphNetwork <graph file name ('dot' network)>
   [-title <graph title>]
   -loadNetwork <network file name>

Create network behaviors:

bionet
   -createNetworkBehaviors
   -loadNetwork <network file name>
   -behaviorLengths <sensory-motor sequence length list (blank separator)>
   [-randomSeed <random seed>]
   [-saveBehaviors <behaviors file name>]

Test network behaviors: 

bionet
   -testNetworkBehaviors
   -loadNetwork <network file name>
   -loadBehaviors <behaviors file name>

Print network behaviors:

bionet
   -printNetworkBehaviors
   -loadBehaviors <behaviors file name>

Create isomorphic/homomorphic networks:

bionet
   -createMorphicNetworks
   -loadBehaviors <behaviors file name>
   -excitatoryNeurons <minimum number> <maximum> <max delta> <probability of random change>
   -inhibitoryNeurons <minimum number> <maximum> <max delta> <probability of random change>
   -synapsePropensities <minimum> <maximum> <max delta> <probability of random change>
   -synapseWeights <max delta> <probability of random change>
   -populationSize <number population members>
   -numMutants <number mutants per generation>
   [-loadNetwork <homomorph network file name> (to create homomorphic networks)
      [-homomorphClones (start with population of homomorphic network clones)]
      [-numOffspring <number mating offspring per generation> (for homomorphic networks)]]
   -numGenerations <number of evolution generations>
   [-fitnessQuorum <fit member quorum required to advance behavior testing to next sensory-motor step>
      (defaults to immediate testing of entire behavior sequences)]
   -saveNetworks [<files prefix (default="network_")>]
   [-randomSeed <random seed>]
