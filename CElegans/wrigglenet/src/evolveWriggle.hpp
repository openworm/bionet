// Evolve C. elegans networks that produce wriggling sinusoidal behavior.

#ifndef __EVOLVE_WRIGGLE_HPP__
#define __EVOLVE_WRIGGLE_HPP__

#include "wriggle.hpp"

class EvolveWriggle
{
public:

   // Constructors.
   EvolveWriggle(Network *homomorph,
                 int populationSize, int numOffspring,
                 float crossoverRate, float mutationRate,
                 int movements,
                 MutableParm& periodParm,
                 MutableParm& amplitudeParm,
                 MutableParm& phaseParm,
                 MutableParm& speedParm,
                 RANDOM randomSeed);
   EvolveWriggle(char *filename);

   // Destructor.
   ~EvolveWriggle();

   // Homomorphic network.
   Network *homomorph;

   // Population.
   vector<Wriggle *> population;

   // Generation.
   int generation;

   // Evolution parameters.
   int    populationSize;
   int    numOffspring;
   float  crossoverRate;
   float  mutationRate;
   RANDOM randomSeed;

   // Evolve networks.
#ifdef THREADS
   void evolve(int numGenerations, int numEvalThreads, char *logFile = NULL);

#else
   void evolve(int numGenerations, char *logFile = NULL);
#endif

   // Load.
   bool load(char *filename);

   // Save.
   bool save(char *filename);

   // Save networks.
   bool saveNetworks();

   // Print.
   void print(bool printNetwork = false);

private:

   // Random number generator.
   Random *randomizer;

   // Offspring.
   vector<Wriggle *> offspring;

   // Mate.
   void mate();

   // Mutate.
   void mutate();

   // Optimize.
   void optimize();

   // Prune members.
   void prune();

   // Sort by fitness.
   void sort();

   // Logging.
   FILE *logfp;
   bool startLog(char *logFile);
   void stopLog();
};
#endif
