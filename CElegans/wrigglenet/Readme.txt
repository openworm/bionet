Wrigglenet:
 
Evolve C. elegans networks that exhibit "wriggling" behavior in response to touch stimuli.
Wriggling is a sinusoidal muscular activity controlled by the C. elegans connectome.
The sinusoidal parameters are: period, amplitude, phase, and speed.
A particular parameter vector generates a bionet behavior that will serve as a fitness
function for the evolution of a population of C. elegans networks.

Usage:

wrigglenet (new run)
   -loadNetwork <C. elegans network file name>
   -populationSize <number population members>
   -numOffspring <number offspring per generation>
   -numGenerations <number of evolution generations>
   -crossoverRate <probability>
   -mutationRate <probability>
   -movements <number of sinusoidal (wriggling) movements>
   -periodParm <minimum> <maximum> <max delta>
   -amplitudeParm <minimum> <maximum> <max delta>
   -phaseParm <minimum> <maximum> <max delta>
   -speedParm <minimum> <maximum> <max delta>
   -saveRun <run file name> and/or -saveNetworks
   [-randomSeed <random seed>]
   [-log <log file name>]
   [-numThreads <number of threads> (defaults to system capacity)]

wrigglenet (resume run)
   -loadRun <run file name>
   -numGenerations <number of evolution generations>
   [-crossoverRate <probability> (defaults to loaded value)]
   [-mutationRate <probability> (defaults to loaded value)]
   -saveRun <run file name> and/or -saveNetworks
   [-log <log file name>]
   [-numThreads <number of threads> (defaults to system capacity)]