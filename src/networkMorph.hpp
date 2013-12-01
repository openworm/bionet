// Network morph.

#ifndef __NETMORPH_HPP__
#define __NETMORPH_HPP__

#include "network.hpp"
#include "behavior.hpp"
#include "mutableParm.hpp"

// Network isomorph/homomorph.
class NetworkMorph
{
public:

   // Maximum tolerated motor output error.
   static const float MAX_ERROR_TOLERANCE;

   // Constructor.
   NetworkMorph();

   Network      *network;
   int          tag;
   float        error;
   vector<bool> motorErrors;
   bool         behaves;
   int          offspringCount;

   // Evaluate behavior.
   void evaluate(vector<Behavior *>& behaviors, int maxStep);
   void evaluate(vector<Behavior *>& behaviors,
                 vector<bool>& fitnessMotorList, int maxStep);

protected:

   Random *randomizer;
};

// Network morphogenesis.
class NetworkMorphoGenesis
{
public:

   // Constructor.
   NetworkMorphoGenesis();

   // Behaviors.
   vector<Behavior *> behaviors;

   // Population size.
   int populationSize;
   vector<NetworkMorph *> population;

   // Offspring per generation.
   int numOffspring;
   vector<NetworkMorph *> offspring;

   // Parent longevity.
   int parentLongevity;

   // Fitness motor list.
   vector<bool> fitnessMotorList;

   // Behave quorum.
   int behaveQuorum;
   int behaveQuorumMaxGenerations;
   int behaviorStep;
   int behaveQuorumGenerationCount;

   // Random seed.
   RANDOM randomSeed;

   // Generation.
   int generation;

   // Evaluate behavior.
   void evaluate();

   // Sort population by fitness.
   void sort();

   // Save networks.
   void saveNetworks(char *filePrefix);

   bool startMorphLog(char *logFile);
   void stopMorphLog();

protected:

   Random *randomizer;
   FILE   *morphfp;
};
#endif
