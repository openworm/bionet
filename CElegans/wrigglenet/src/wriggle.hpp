// Wriggling sinusoidal behavior parameters and C. elegans nework that produces the behavior.

#ifndef __WRIGGLE_HPP__
#define __WRIGGLE_HPP__

#include "networkHomomorph.hpp"

class Wriggle
{
public:

   // Constructors.
   Wriggle(Network *homomorph,
           int movements,
           MutableParm& periodParm,
           MutableParm& amplitudeParm,
           MutableParm& phaseParm,
           MutableParm& speedParm,
           Random *randomizer, int tag = 0);
   Wriggle();
   Wriggle(FILE *fp, Network *homomorph, Random *randomizer);

   // Destructor.
   ~Wriggle();

   // Homomorphic network.
   Network *homomorph;

   // Parameters.
   int         movements;
   MutableParm periodParm;
   MutableParm amplitudeParm;
   MutableParm phaseParm;
   MutableParm speedParm;

   // Trained network.
   Network *network;

   // Tag.
   int tag;

#ifdef THREADS
   // Evaluation threads.
   int numEvalThreads;
#endif

   // Evaluate sinusoidal behavior.
   float error;
   void evaluate();

   // Mutate parameters.
   void mutate();

   // Optimize parameters.
   void optimize();

   // Clone.
   Wriggle *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);


   // Connectome dimensions.
   static const int NUM_SENSORS = 86;
   static const int NUM_MOTORS  = 97;
   static const int NUM_NEURONS = 396;

   // Network morphgenesis parameters.
   static const int   POPULATION_SIZE;
   static const int   NUM_OFFSPRING;
   static const int   NUM_GENERATIONS;
   static const float CROSSOVER_RATE;
   static const float MUTATION_RATE;
   static MutableParm SYNAPSE_WEIGHTS_PARM;
   static const float SYNAPSE_CROSSOVER_BOND_STRENGTH;
   static const int   SYNAPSE_OPTIMIZED_PATH_LENGTH;
   static const char  *LOG_FILE;

private:

   // Random numbers.
   Random *randomizer;

   // Connectome cell index.
   struct CellIndex
   {
      char *name;
      int  index;
   };

   // Sensor indices.
   static const struct CellIndex SensorIndices[];

   // Muscle indices.
   static const struct CellIndex MuscleIndices[];

   // Motor sequences.
   static const int       BODY_JOINTS = 12;
   vector<vector<float> > ventralMotorSequence;
   vector<vector<float> > dorsalMotorSequence;

   // Create wriggling movements.
   void createMovements();

   // Create wriggling behaviors.
   void createBehaviors(vector<Behavior *>& behaviors);

   // Create network trained to behaviors.
   void createNetwork(vector<Behavior *>& behaviors);

   void permuteValues(vector<vector<float> >& valueRanges,
                      vector<vector<float> >& permutations,
                      vector<float>& permutation,
                      int level, int depth);
};
#endif
