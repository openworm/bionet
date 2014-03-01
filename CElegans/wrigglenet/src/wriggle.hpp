// Wriggling sinusoidal behavior parameters and C. elegans nework that produces the behavior.

#ifndef __WRIGGLE_HPP__
#define __WRIGGLE_HPP__

#include "mutableParm.hpp"
#include "network.hpp"
#include "random.hpp"

class Wriggle
{
public:

   // Constructors.
   Wriggle(int movements,
           MutableParm& periodParm,
           MutableParm& amplitudeParm,
           MutableParm& phaseParm,
           MutableParm& speedParm,
           MutableParm& delayParm,
           Random *randomizer, int tag = 0);
   Wriggle();
   Wriggle(FILE *fp, int movements, Random *randomizer);

   // Destructor.
   ~Wriggle();

   // Parameters.
   int         movements;
   MutableParm periodParm;
   MutableParm amplitudeParm;
   MutableParm phaseParm;
   MutableParm speedParm;
   MutableParm delayParm;

   // C. elegans network.
   Network *network;

   // Tag.
   int tag;

   // Evaluate sinusoidal behavior.
   float fitness;
   void evaluate();

   // Mutate parameters.
   void mutate();

   // Clone.
   Wriggle *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print();

private:

   Random *randomizer;
};
#endif
