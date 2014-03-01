// Evolve C. elegans networks that produce wriggling sinusoidal behavior.

#ifndef __EVOLVE_WRIGGLENET_HPP__
#define __EVOLVE_WRIGGLENET_HPP__

#include "wriggle.hpp"

class EvolveWrigglenet
{
public:

   // Constructors.
   EvolveWrigglenet(
      int populationSize, int numOffspring,
      float crossoverRate, float mutationRate,
      int movements,
      MutableParm& periodParm,
      MutableParm& amplitudeParm,
      MutableParm& phaseParm,
      MutableParm& speedParm,
      MutableParm& delayParm,
      RANDOM randomSeed);
   EvolveWrigglenet();
   EvolveWrigglenet(char *filename);

   // Destructor.
   ~EvolveWrigglenet();

   // Population.
   vector<Wriggle *> population;

   // Evolution parameters.
   int    populationSize;
   int    numOffspring;
   float  crossoverRate;
   float  mutationRate;
   RANDOM randomSeed;

   // Evolve networks.
   void evolve(int numGenerations);

   // Mate.
   void mate();

   // Mutate.
   void mutate();

   // Evaluate fitness.
   void evaluate();

   // Prune members.
   void prune();

   // Save networks.
   void saveNetworks();

   // Load.
   void load(char *filename);

   // Save.
   void save(char *filename);

   // Print.
   void print();

private:

   Random *randomizer;
};
#endif
