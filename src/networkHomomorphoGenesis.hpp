// A homomorphic network reproduces the sensory-motor behaviors of another network
// while mirroring its neuron configuration.

#ifndef __NETWORK_HOMOMORPHOGENESIS_HPP__
#define __NETWORK_HOMOMORPHOGENESIS_HPP__

#include "networkMorphoGenesis.hpp"
#include "undulationNetworkHomomorph.hpp"
#include "neuronSimNetworkHomomorph.hpp"
#include "neuronSim.hpp"
#include "c302SimNetworkHomomorph.hpp"
#include "c302Sim.hpp"
#ifdef THREADS
#include <pthread.h>
#endif

// Network homomorphogenesis.
class NetworkHomomorphoGenesis : public NetworkMorphoGenesis
{
public:

   // Storage format.
   enum { FORMAT = 2 };

   // Constructor.
   NetworkHomomorphoGenesis(vector<Behavior *>& behaviors,
                            Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity,
                            vector<int>& fitnessMotorList,
                            int behaveQuorum, int behaveQuorumMaxGenerations,
                            float crossoverRate, float mutationRate,
                            MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength,
                            int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   // Undulation behavior constructor.
   NetworkHomomorphoGenesis(int undulationMovements,
                            Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity,
                            float crossoverRate, float mutationRate,
                            MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength,
                            int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   // Constructor with NEURON simulator fitness evaluation.
   NetworkHomomorphoGenesis(char *neuronExecPath, char *simDir, char *simHocFile,
                            Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity,
                            float crossoverRate, float mutationRate,
                            MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength,
                            int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   // Constructor with c302 simulator fitness evaluation.
   NetworkHomomorphoGenesis(char *jnmlCmdPath, char *simDir,
                            Network *homomorph,
                            int populationSize, int numOffspring, int parentLongevity,
                            float crossoverRate, float mutationRate,
                            MutableParm& synapseWeightsParm,
                            float synapseCrossoverBondStrength,
                            int synapseOptimizedPathLength,
                            RANDOM randomSeed);

   NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, char *filename, bool binary = false);
   NetworkHomomorphoGenesis(int undulationMovements, char *filename, bool binary = false);
   NetworkHomomorphoGenesis(char *neuronExecPath, char *simDir, char *simHocFile,
                            char *filename, bool binary = false);
   NetworkHomomorphoGenesis(char *jnmlCmdPath, char *simDir, char *filename, bool binary = false);

   // Destructor.
   ~NetworkHomomorphoGenesis();

   // Homomorphic network.
   Network *homomorph;

   // Undulation behavior.
   bool undulationBehavior;
   int  undulationMovements;

   // NEURON simulation.
   bool                neuronSimulation;
   string              neuronExecPath;
   string              neuronSimDir;
   string              neuronSimHocFile;
   NeuronSim           *neuronModelSim;
   vector<NeuronSim *> neuronEvaluationSims;

   // c302 simulation.
   static const string c302RelativePath;
   bool                c302Simulation;
   string              jnmlCmdPath;
   string              c302SimDir;
   c302Sim             *c302ModelSim;
   vector<c302Sim *>   c302EvaluationSims;

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
              int behaveCutoff = -1, char *logFile = NULL,
              char *saveFile = NULL);

#else
   void morph(int numGenerations,
              int behaveCutoff = -1, char *logFile = NULL,
              char *saveFile = NULL);
#endif

   // Mate members.
   void mate();

   // Mutate offspring.
   void mutate();

   // Optimize offspring.
   void optimize();

   // Prune members.
   void prune();

   // Evaluate behavior.
   void evaluate();

   // Sort population by fitness.
   void sort();

   // Load.
   bool load(char *filename, bool binary = false);

   // Save.
   bool save(char *filename, bool binary = false);

   // Print.
   void print(bool printNetwork = false);

   // Neurons connected to motors (indices).
   vector<vector<pair<int, int> > > motorConnections;

   // Termination signal.
   bool sigterm;

private:

   void init(Network *homomorph,
             int populationSize, int numOffspring, int parentLongevity,
             float crossoverRate, float mutationRate,
             MutableParm& synapseWeightsParm,
             float synapseCrossoverBondStrength,
             int synapseOptimizedPathLength,
             RANDOM randomSeed);

   void initNeuronEvaluationSims(int numSims);
   void initc302EvaluationSims(int numSims);

   void mate(int threadNum);
   void mutate(int threadNum);
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
