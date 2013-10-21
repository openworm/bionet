// A homomorphic network reproduces the sensory-motor behaviors of another network
// while mirroring its neuron configuration.

#ifndef __NET_HOMOMORPH_HPP__
#define __NET_HOMOMORPH_HPP__

#include "networkMorph.hpp"
#include <algorithm>
#ifdef THREADS
#include <pthread.h>
#endif

// Network homomorph.
class NetworkHomomorph : NetworkMorph
{
public:

   // Constructors.
   NetworkHomomorph(Network *homomorph, MutableParm& synapseWeightsParm,
                    Random *randomizer, int tag = 0);
   NetworkHomomorph(FILE *fp, Random *randomizer);

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
   void print();

private:

   // Permute weights.
   void permuteWeights(vector<vector<float> >& weightRanges,
                       vector<vector<float> >& permutations,
                       vector<float>& permutation,
                       int level, int depth);
};

// Network homomorphogenesis.
class NetworkHomomorphoGenesis : NetworkMorphoGenesis
{
public:

   // Constructors.
   NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity, vector<int>& fitnessMotorList,
                            int fitnessQuorum, float crossoverRate, float mutationRate, MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength, int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, char *filename);

   // Destructor.
   ~NetworkHomomorphoGenesis();

   // Homomorphic network.
   Network *homomorph;

   // Crossover and mutation rates.
   float crossoverRate;
   float mutationRate;

   // Synapse crossover bond strength.
   float synapseCrossoverBondStrength;

   // Synapse optimized path length.
   int synapseOptimizedPathLength;

   // Morph networks.
#ifdef THREADS
   void morph(int numGenerations, int numThreads, int behaveCutoff = -1);

#else
   void morph(int numGenerations, int behaveCutoff = -1);
#endif

   // Mate members.
   void mate();

   // Optimize offspring.
   void optimize();

   // Prune members.
   void prune();

   // Load.
   bool load(char *filename);

   // Save.
   bool save(char *filename);

   // Print.
   void print();

private:

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
};
#endif
