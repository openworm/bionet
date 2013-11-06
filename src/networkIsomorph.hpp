// An isomorphic network reproduces the sensory-motor behaviors of another network.

#ifndef __NET_ISOMORPH_HPP__
#define __NET_ISOMORPH_HPP__

#include "networkMorph.hpp"
#ifdef THREADS
#include <pthread.h>
#endif

// Network isomorph.
class NetworkIsomorph : NetworkMorph
{
public:

   // Constructors.
   NetworkIsomorph(MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                   MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                   int numSensors, int numMotors, Random *randomizer, int tag = 0);
   NetworkIsomorph(FILE *fp, Random *randomizer);

   // Destructor.
   ~NetworkIsomorph();

   MutableParm excitatoryNeuronsParm;
   MutableParm inhibitoryNeuronsParm;
   MutableParm synapsePropensitiesParm;
   MutableParm synapseWeightsParm;

   // Mutate.
   void mutate();

   // Clone.
   NetworkIsomorph *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);

private:

   void deleteIndexedNeuron(int index);
   void addIndexedNeuron(int index, bool excitatory);
};

// Network isomorphogenesis.
class NetworkIsomorphoGenesis : NetworkMorphoGenesis
{
public:

   // Constructors.
   NetworkIsomorphoGenesis(vector<Behavior *>& behaviors,
                           int populationSize, int numOffspring,
                           int behaveQuorum, int behaveQuorumMaxGenerations,
                           MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                           MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                           RANDOM randomSeed);

   NetworkIsomorphoGenesis(vector<Behavior *>& behaviors, char *filename);

   // Destructor.
   ~NetworkIsomorphoGenesis();

   // Morph networks.
#ifdef THREADS
   void morph(int numGenerations, int numThreads, char *logFile = NULL);

#else
   void morph(int numGenerations, char *logFile = NULL);
#endif

   // Mutate members.
   void mutate();

   // Prune less fit members.
   void prune();

   // Load.
   bool load(char *filename);

   // Save.
   bool save(char *filename);

   // Print.
   void print(bool printNetwork = false);

private:

   void mutate(int threadNum);

#ifdef THREADS
   // Threading.
   pthread_barrier_t morphBarrier;
   pthread_mutex_t   morphMutex;
   pthread_t         *threads;
   int               numThreads;
   struct ThreadInfo
   {
      NetworkIsomorphoGenesis *morphoGenesis;
      int                     threadNum;
   };
   static void *morphThread(void *threadInfo);

   bool terminate;
#endif
};
#endif
