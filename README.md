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

Bionet was inspired by OpenWorm (openworm.org), a project to model the
C Elegans nematode worm in a high fidelity artificial creature. Although
the connectome (neural network) of the worm is known, it is not known
how it produces sensory-motor behaviors. Bionet is an attempt to do this.
A network containing the C Elegans connectome is supplied with
the distribution: CElegans_network.txt. This network has random weights
and can be used to produce sensory-motor behavior sequences. The goal
is to evolve other networks that perform the same behaviors, not
knowing the original network weights. Since C Elegans has close to
4000 synapses, this is a daunting task. However, accomplishing it would
be a huge step toward programming an artificial C Elegans.

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
   [-minSynapseWeight <minimum synapse weight (defaults to 0.0)>]
   [-maxSynapseWeight <maximum synapse weight (defaults to 1.0)>]
   [-randomSeed <random seed>]
   [-saveNetwork <network file name>]
   [-graphNetwork <graph file name ('dot' format)>]

Print network:

bionet
   -printNetwork
   [-connectivity (also print connectivity)]   
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
   [-motorDeltaTolerance <minimum motor delta to print>]

Print network behaviors:

bionet
   -printNetworkBehaviors
   -loadBehaviors <behaviors file name>

Create homomorphic networks:

bionet (new morph)
   -createHomomorphicNetworks
   -loadBehaviors <behaviors file name>
   -loadNetwork <homomorph network file name>
   -populationSize <number population members>
   -numOffspring <number offspring per generation>
   [-parentLongevity <parent dies after this many offspring>]
   -numGenerations <number of evolution generations>
   [-behaveCutoff <stop evolution when this many members behave>]
   [-fitnessMotorList <list of motor outputs evaluated for fitness (0-n, comma-separated)>
      (defaults to fitness evaluation of all motor outputs)]
   [-fitnessQuorum <fit member quorum required to advance behavior testing to next sensory-motor step>
      (defaults to immediate testing of entire behavior sequences)]
   -crossoverRate <probability>
   -mutationRate <probability>
   -synapseWeights <minimum> <maximum> <max delta>
   -synapseCrossoverBondStrength <probability of connected neurons crossing over together>
   -synapseOptimizedPathLength <synapse path length optimized as a group>
   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default="network_")>]
   [-randomSeed <random seed>]
   [-logMorph <morph log file name> (instead of standard output)]
   [-numThreads <number of threads> (defaults to system capacity)]

bionet (resume morph)
   -createHomomorphicNetworks
   -loadBehaviors <behaviors file name>
   -loadMorph <morph file name>
   -numGenerations <number of evolution generations>
   [-behaveCutoff <stop evolution when this many members behave>]
   [-fitnessMotorList <list of motor outputs evaluated for fitness (0-n, comma-separated)>
      (defaults to loaded list)]
   [-crossoverRate <probability> (defaults to loaded value)]
   [-mutationRate <probability> (defaults to loaded value)]
   [-synapseCrossoverBondStrength <probability of connected neurons crossing over together>]
   [-synapseOptimizedPathLength <synapse path length optimized as a group>]
   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default="network_")>]
   [-logMorph <morph log file name> (instead of standard output)]
   [-numThreads <number of threads> (defaults to system capacity)]

Merge homomorphic network populations:

bionet   
   -mergeHomomorphicNetworks
   -loadMorph <morph file name> <morph file name>
   -saveMorph <morph file name>
   [-randomSeed <random seed>]

Create isomorphic networks:

bionet (new morph)   
   -createIsomorphicNetworks
   -loadBehaviors <behaviors file name>
   -populationSize <number population members>
   -numMutants <number mutants per generation>
   -numGenerations <number of evolution generations>
   [-fitnessQuorum <fit member quorum required to advance behavior testing to next sensory-motor step>
      (defaults to immediate testing of entire behavior sequences)]
   -excitatoryNeurons <minimum number> <maximum> <max delta> <probability of random change>
   -inhibitoryNeurons <minimum number> <maximum> <max delta> <probability of random change>
   -synapsePropensities <minimum> <maximum> <max delta> <probability of random change>
   -synapseWeights <minimum> <maximum> <max delta> <probability of random change>
   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default="network_")>]
   [-randomSeed <random seed>]
   [-logMorph <morph log file name> (instead of standard output)]
   [-numThreads <number of threads> (defaults to system capacity)]

bionet (resume morph)
   -createIsomorphicNetworks
   -loadBehaviors <behaviors file name>
   -loadMorph <morph file name>
   -numGenerations <number of evolution generations>
   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default="network_")>]
   [-logMorph <morph log file name> (instead of standard output)]
   [-numThreads <number of threads> (defaults to system capacity)]
