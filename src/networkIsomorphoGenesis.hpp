// An isomorphic network reproduces the sensory-motor behaviors of another network.

#ifndef __NETWORK_ISOMORPHOGENESIS_HPP__
#define __NETWORK_ISOMORPHOGENESIS_HPP__

#include "networkMorphoGenesis.hpp"
#include "networkIsomorph.hpp"
#ifdef THREADS
#include <pthread.h>
#endif

// Network isomorphogenesis.
class NetworkIsomorphoGenesis : NetworkMorphoGenesis
{
public:

   // Storage format.
   enum { FORMAT = 1 };

   // Constructors.
   NetworkIsomorphoGenesis(vector<Behavior *>& behaviors,
                           int populationSize, int numOffspring,
                           int behaveQuorum, int behaveQuorumMaxGenerations,
                           MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                           MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                           RANDOM randomSeed);

   NetworkIsomorphoGenesis(vector<Behavior *>& behaviors, char *filename, bool binary = false);

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
   bool load(char *filename, bool binary = false);

   // Save.
   bool save(char *filename, bool binary = false);

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
