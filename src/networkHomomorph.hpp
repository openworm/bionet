// A homomorphic network reproduces the sensory-motor behaviors of another network
// while mirroring its neuron configuration.

#ifndef __NET_HOMOMORPH_HPP__
#define __NET_HOMOMORPH_HPP__

#include "networkMorph.hpp"
#ifdef THREADS
#include <pthread.h>
#endif

// Network homomorph.
class NetworkHomomorph : public NetworkMorph
{
public:

   // Constructors.
   NetworkHomomorph(Network *homomorph,
                    MutableParm& synapseWeightsParm,
                    vector<vector<pair<int, int> > > *motorConnections,
                    Random *randomizer, int tag = 0);
   NetworkHomomorph();
   NetworkHomomorph(FILE *fp,
                    vector<vector<pair<int, int> > > *motorConnections,
                    Random *randomizer);

   // Destructor.
   ~NetworkHomomorph();

   // Synapse weights parameter.
   MutableParm synapseWeightsParm;

   // Mutate synapses.
   void mutate();

   // Harmonize synapses.
   void harmonize(vector<Behavior *>& behaviors, vector<bool>& fitnessMotorList,
                  int synapseOptimizedPathLength, int maxStep);

   // Clone.
   NetworkHomomorph *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);

   // Neurons connected to motors (indices).
   vector<vector<pair<int, int> > > *motorConnections;

protected:

   // Select random neuron.
   int randomNeuron(bool nonMotor = false);

   // Permute weights.
   void permuteWeights(vector<vector<float> >& weightRanges,
                       vector<vector<float> >& permutations,
                       vector<float>& permutation,
                       int level, int depth);
};

// Locomotion behavior network homomorph.
class LocomotionNetworkHomomorph : public NetworkHomomorph
{
public:

   // Constructors.
   LocomotionNetworkHomomorph(int locomotionMovements, Network *homomorph,
                              MutableParm& synapseWeightsParm,
                              vector<vector<pair<int, int> > > *motorConnections,
                              Random *randomizer, int tag = 0);
   LocomotionNetworkHomomorph(FILE *fp, int locomotionMovements,
                              vector<vector<pair<int, int> > > *motorConnections,
                              Random *randomizer);

   // Harmonize synapses.
   void harmonize(int synapseOptimizedPathLength);

   // Fitness.
   float fitness;

   // Evaluate locomotion behavior fitness.
   int locomotionMovements;
   void evaluate();

   // Clone.
   LocomotionNetworkHomomorph *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);

   // Connectome cell index.
   struct CellIndex
   {
      char *name;
      int  index;
   };

   // Sensor indices.
   static const struct CellIndex sensorIndices[];

   // Muscle indices.
   static const struct CellIndex muscleIndices[];

   // Body joint.
   struct BodyJoint
   {
      int dorsalMuscles[4];
      int ventralMuscles[4];
   };

   // Body joints.
   static const enum { NUM_BODY_JOINTS=12 };
   static const struct BodyJoint bodyJoints[NUM_BODY_JOINTS];
};

// Network homomorphogenesis.
class NetworkHomomorphoGenesis : public NetworkMorphoGenesis
{
public:

   // Constructor.
   NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity,
                            vector<int>& fitnessMotorList,
                            int behaveQuorum, int behaveQuorumMaxGenerations,
                            float crossoverRate, float mutationRate,
                            MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength,
                            int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   // Locomotion behavior constructor.
   NetworkHomomorphoGenesis(int locomotionMovements, Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity,
                            float crossoverRate, float mutationRate,
                            MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength,
                            int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, char *filename);
   NetworkHomomorphoGenesis(int locomotionMovements, char *filename);

   // Destructor.
   ~NetworkHomomorphoGenesis();

   // Homomorphic network.
   Network *homomorph;

   // Locomotion behavior.
   bool locomotionBehavior;
   int  locomotionMovements;

   // Crossover and mutation rates.
   float crossoverRate;
   float mutationRate;

   // Synapse crossover bond strength.
   float synapseCrossoverBondStrength;

   // Synapse optimized path length.
   int synapseOptimizedPathLength;

   // Morph networks.
#ifdef THREADS
   void morph(int numGenerations, int numThreads,
              int behaveCutoff = -1, char *logFile = NULL);

#else
   void morph(int numGenerations,
              int behaveCutoff = -1, char *logFile = NULL);
#endif

   // Mate members.
   void mate();

   // Optimize offspring.
   void optimize();

   // Prune members.
   void prune();

   // Evaluate behavior.
   void evaluate();

   // Sort population by fitness.
   void sort();

   // Load.
   bool load(char *filename);

   // Save.
   bool save(char *filename);

   // Print.
   void print(bool printNetwork = false);

   // Neurons connected to motors (indices).
   vector<vector<pair<int, int> > > motorConnections;

private:

   void init(Network *homomorph,
             int populationSize, int numOffspring, int parentLongevity,
             float crossoverRate, float mutationRate,
             MutableParm& synapseWeightsParm,
             float synapseCrossoverBondStrength,
             int synapseOptimizedPathLength,
             RANDOM randomSeed);

   void mate(int threadNum);
   void optimize(int threadNum);

#ifdef THREADS
   // Threading.
   pthread_barrier_t morphBarrier;
   pthread_mutex_t   morphMutex;
   pthread_t         *threads;
   int               numThreads;
   struct ThreadInfo
   {
      NetworkHomomorphoGenesis *morphoGenesis;
      int                      threadNum;
   };
   static void *morphThread(void *threadInfo);

   bool terminate;
#endif

   // Crossover neurons.
   void crossover(Network *child, Network *parent, int index, int distance);

   // Get motor connections.
   void getMotorConnections();
   void getMotorConnectionsSub(queue<pair<Neuron *, int> >& open,
                               vector<bool>& closed,
                               vector<pair<int, int> >& connections);
};
#endif
