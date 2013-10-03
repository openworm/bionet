// An isomorphic network reproduces the sensory-motor behaviors of another network.
// A homomorphic network additionally mirrors the neuron configuration of the other network.

#ifndef __MORPHNET_HPP__
#define __MORPHNET_HPP__

#include "network.hpp"
#include "behavior.hpp"

// Mutable parameter.
class MutableParm
{
public:

   MutableParm();
   MutableParm(float minimum, float maximum, float maxDelta,
               float randomProbability);
   void init(float minimum, float maximum, float maxDelta,
             float randomProbability);

   float value;
   float minimum;
   float maximum;
   float maxDelta;
   float randomProbability;

   void initValue(Random *randomizer);
   void setValue(float value);
   void mutateValue(Random *randomizer);
   void load(FILE *fp);
   void save(FILE *fp);
   void print();
};

// Network isomorph/homomorph.
class NetworkMorph
{
public:

   // Maximum tolerated motor output error.
   static const float MAX_ERROR_TOLERANCE;

   // Constructors.
   NetworkMorph(MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                int numSensors, int numMotors, Random *randomizer, int tag = 0);
   NetworkMorph(Network *homomorph, MutableParm& synapseWeightsParm,
                Random *randomizer, int tag = 0);
   NetworkMorph(FILE *fp, Random *randomizer);

   // Destructor.
   ~NetworkMorph();

   MutableParm excitatoryNeuronsParm;
   MutableParm inhibitoryNeuronsParm;
   MutableParm synapsePropensitiesParm;
   MutableParm synapseWeightsParm;

   bool    homomorphic;
   Network *network;
   int     tag;
   float   error;
   bool    behaves;

   // Evaluate behavior.
   void evaluate(vector<Behavior *>& behaviors, int maxStep);

   // Mutate.
   void mutate();

   // Clone.
   NetworkMorph *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print();

private:

   Random *randomizer;

   void deleteIndexedNeuron(int index);
   void addIndexedNeuron(int index, bool excitatory);
};

// Network morphogenesis.
class NetworkMorphoGenesis
{
public:

   // Isomorph constructor.
   NetworkMorphoGenesis(vector<Behavior *>& behaviors,
                        int populationSize, int numMutants, int fitnessQuorum,
                        MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                        MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                        RANDOM randomSeed);

   // Homomorph constructor.
   NetworkMorphoGenesis(vector<Behavior *>& behaviors, Network *homomorph,
                        int populationSize, int numMutants, int numOffspring,
                        int fitnessQuorum, MutableParm& synapseWeightsParm, RANDOM randomSeed);

   // Load constructor.
   NetworkMorphoGenesis(vector<Behavior *>& behaviors, char *filename);

   // Destructor.
   ~NetworkMorphoGenesis();

   // Behaviors.
   vector<Behavior *> behaviors;

   // Homomorphic network.
   Network *homomorph;

   // Population size.
   int populationSize;

   // Mutants created per generation.
   int numMutants;

   // Number of homomorphic offspring per generation.
   int numOffspring;

   // Network population.
   vector<NetworkMorph *> population;
   vector<NetworkMorph *> mutants;
   vector<NetworkMorph *> offspring;

   // Fitness quorum.
   int fitnessQuorum;
   int behaviorStep;

   // Random seed.
   RANDOM randomSeed;

   // Generation.
   int generation;

   // Morph networks.
   void morph(int numGenerations);

   // Mutate members.
   void mutate();

   // Mate homomorphic members.
   void mate();

   // Prune less fit members.
   void prune();

   // Sort population by fitness.
   void sort();

   // Load.
   bool load(char *filename);

   // Save.
   bool save(char *filename);

   // Save networks.
   void saveNetworks(char *filePrefix);

   // Print.
   void print();

private:

   Random *randomizer;
};
#endif
