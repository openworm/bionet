// Network morphogenesis.

#ifndef __NETWORK_MORPHOGENESIS_HPP__
#define __NETWORK_MORPHOGENESIS_HPP__

#include "networkMorph.hpp"

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
