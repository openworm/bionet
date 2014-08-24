// Network homomorphogenesis implementation.

#include "networkHomomorphoGenesis.hpp"
#ifdef WIN32
#include <direct.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fstream>
#endif

// c302 relative path.
const string NetworkHomomorphoGenesis::c302RelativePath = "CElegans/pythonScripts";

// Constructor.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, Network *homomorph,
                                                   int populationSize, int numOffspring, int parentLongevity,
                                                   vector<int>& fitnessMotorList,
                                                   int behaveQuorum, int behaveQuorumMaxGenerations,
                                                   float crossoverRate, float mutationRate,
                                                   MutableParm& synapseWeightsParm,
                                                   float synapseCrossoverBondStrength,
                                                   int synapseOptimizedPathLength,
                                                   RANDOM randomSeed)
{
   int i, j, k, n;

   undulationBehavior  = false;
   undulationMovements = -1;
   neuronSimulation    = false;
   neuronModelSim      = NULL;
   c302Simulation      = false;
   c302ModelSim        = NULL;
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   if (fitnessMotorList.size() > 0)
   {
      n = homomorph->numMotors;
      this->fitnessMotorList.resize(n, false);
      for (i = 0, j = (int)fitnessMotorList.size(); i < j; i++)
      {
         k = fitnessMotorList[i];
         assert(k >= 0 && k < n);
         this->fitnessMotorList[k] = true;
      }
   }
   this->behaveQuorum = behaveQuorum;
   if (behaveQuorum == -1)
   {
      behaviorStep = -1;
   }
   else
   {
      behaviorStep = 0;
   }
   this->behaveQuorumMaxGenerations = behaveQuorumMaxGenerations;
   if (behaveQuorumMaxGenerations == -1)
   {
      behaveQuorumGenerationCount = -1;
   }
   else
   {
      behaveQuorumGenerationCount = 0;
   }
   init(homomorph,
        populationSize, numOffspring, parentLongevity,
        crossoverRate, mutationRate,
        synapseWeightsParm,
        synapseCrossoverBondStrength,
        synapseOptimizedPathLength,
        randomSeed);
}


// Constructor for undulation homomorphogenesis.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(int undulationMovements, Network *homomorph,
                                                   int populationSize, int numOffspring, int parentLongevity,
                                                   float crossoverRate, float mutationRate,
                                                   MutableParm& synapseWeightsParm,
                                                   float synapseCrossoverBondStrength,
                                                   int synapseOptimizedPathLength,
                                                   RANDOM randomSeed)
{
   undulationBehavior        = true;
   this->undulationMovements = undulationMovements;
   neuronSimulation          = false;
   neuronModelSim            = NULL;
   c302Simulation            = false;
   c302ModelSim = NULL;
   behaveQuorum = -1;
   behaviorStep = -1;
   behaveQuorumMaxGenerations  = -1;
   behaveQuorumGenerationCount = -1;
   init(homomorph,
        populationSize, numOffspring, parentLongevity,
        crossoverRate, mutationRate,
        synapseWeightsParm,
        synapseCrossoverBondStrength,
        synapseOptimizedPathLength,
        randomSeed);
}


// Constructor with NEURON simulator fitness evaluation.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(
   char *neuronExecPath, char *simDir, char *simHocFile,
   Network *homomorph,
   int populationSize, int numOffspring, int parentLongevity,
   float crossoverRate, float mutationRate,
   MutableParm& synapseWeightsParm,
   float synapseCrossoverBondStrength,
   int synapseOptimizedPathLength,
   RANDOM randomSeed)
{
   neuronSimulation = true;
   char buf[BUFSIZ];
#ifdef WIN32
   this->neuronExecPath = _fullpath(buf, neuronExecPath, BUFSIZ);
#else
   this->neuronExecPath = realpath(neuronExecPath, buf);
#endif
   neuronSimDir                = simDir;
   neuronSimHocFile            = simHocFile;
   undulationBehavior          = false;
   undulationMovements         = -1;
   c302Simulation              = false;
   c302ModelSim                = NULL;
   behaveQuorum                = -1;
   behaviorStep                = -1;
   behaveQuorumMaxGenerations  = -1;
   behaveQuorumGenerationCount = -1;

   // Initialize model simulation.
   string workDir = neuronSimDir;
   workDir.append("/model");
   neuronModelSim = new NeuronSim(this->neuronExecPath, workDir, neuronSimHocFile);
   assert(neuronModelSim != NULL);
   neuronModelSim->exportSynapses(homomorph);
   neuronModelSim->importSynapseWeights(homomorph);

   // Common initialization.
   init(homomorph,
        populationSize, numOffspring, parentLongevity,
        crossoverRate, mutationRate,
        synapseWeightsParm,
        synapseCrossoverBondStrength,
        synapseOptimizedPathLength,
        randomSeed);

   // Run model simulation to produce activations.
   neuronModelSim->run();
}


// Constructor with c302 simulator fitness evaluation.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(
   char *jnmlCmdPath, char *simDir,
   Network *homomorph,
   int populationSize, int numOffspring, int parentLongevity,
   float crossoverRate, float mutationRate,
   MutableParm& synapseWeightsParm,
   float synapseCrossoverBondStrength,
   int synapseOptimizedPathLength,
   RANDOM randomSeed)
{
   c302Simulation = true;
   char buf[BUFSIZ];
#ifdef WIN32
   this->jnmlCmdPath = _fullpath(buf, jnmlCmdPath, BUFSIZ);
#else
   this->jnmlCmdPath = realpath(jnmlCmdPath, buf);
#endif
   c302SimDir                  = simDir;
   undulationBehavior          = false;
   undulationMovements         = -1;
   neuronSimulation            = false;
   neuronModelSim              = NULL;
   behaveQuorum                = -1;
   behaviorStep                = -1;
   behaveQuorumMaxGenerations  = -1;
   behaveQuorumGenerationCount = -1;

   // Initialize model simulation.
   string workDir = c302SimDir;
   workDir.append("/");
   workDir.append(c302RelativePath);
   workDir.append("/c302");
   c302ModelSim = new c302Sim(this->jnmlCmdPath, workDir);
   assert(c302ModelSim != NULL);
   c302ModelSim->exportSynapses(homomorph);
   c302ModelSim->importSynapseWeights(homomorph);

   // Common initialization.
   init(homomorph,
        populationSize, numOffspring, parentLongevity,
        crossoverRate, mutationRate,
        synapseWeightsParm,
        synapseCrossoverBondStrength,
        synapseOptimizedPathLength,
        randomSeed);

   // Run model simulation to produce activations.
   c302ModelSim->run();
}


void NetworkHomomorphoGenesis::init(Network *homomorph,
                                    int populationSize, int numOffspring, int parentLongevity,
                                    float crossoverRate, float mutationRate,
                                    MutableParm& synapseWeightsParm,
                                    float synapseCrossoverBondStrength,
                                    int synapseOptimizedPathLength,
                                    RANDOM randomSeed)
{
   int          i, j, k, n, p, q;
   float        r;
   NetworkMorph *networkMorph;
   Network      *network;
   Synapse      *synapse;

   this->randomSeed = randomSeed;
   randomizer       = new Random(randomSeed);
   assert(randomizer != NULL);
   this->homomorph = homomorph;
   assert(numOffspring <= populationSize);
   this->populationSize  = populationSize;
   this->numOffspring    = numOffspring;
   this->parentLongevity = parentLongevity;
   this->crossoverRate   = crossoverRate;
   this->mutationRate    = mutationRate;
   this->synapseCrossoverBondStrength = synapseCrossoverBondStrength;
   this->synapseOptimizedPathLength   = synapseOptimizedPathLength;
   getMotorConnections();
   generation = 0;
   for (i = 0; i < populationSize; i++)
   {
      if (undulationBehavior)
      {
         networkMorph = (NetworkMorph *)new UndulationNetworkHomomorph(
            undulationMovements, homomorph, synapseWeightsParm,
            &motorConnections, randomizer, NetworkMorphoGenesis::tagGenerator++);
      }
      else if (neuronSimulation)
      {
         networkMorph = (NetworkMorph *)new NeuronSimNetworkHomomorph(
            homomorph, synapseWeightsParm,
            &motorConnections, randomizer, NetworkMorphoGenesis::tagGenerator++);
      }
      else if (c302Simulation)
      {
         networkMorph = (NetworkMorph *)new c302SimNetworkHomomorph(
            homomorph, synapseWeightsParm,
            &motorConnections, randomizer, NetworkMorphoGenesis::tagGenerator++);
      }
      else
      {
         networkMorph = (NetworkMorph *)new NetworkHomomorph(
            homomorph, synapseWeightsParm,
            &motorConnections, randomizer, NetworkMorphoGenesis::tagGenerator++);
      }
      assert(networkMorph != NULL);
      network = networkMorph->network;
      n       = network->numNeurons;
      for (j = 0; j < n; j++)
      {
         for (k = 0; k < n; k++)
         {
            // Assume that multiple synapses weights between a source and target are equal.
            r = (float)randomizer->RAND_INTERVAL(
               synapseWeightsParm.minimum, synapseWeightsParm.maximum);
            for (p = 0, q = (int)network->synapses[j][k].size(); p < q; p++)
            {
               synapse = network->synapses[j][k][p];
               synapse->setWeight(r);
            }
         }
      }
      population.push_back(networkMorph);
   }
   sigterm = false;
}


// Load constructor.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(
   vector<Behavior *>& behaviors, char *filename, bool binary)
{
   int i, j;

   undulationBehavior  = false;
   undulationMovements = -1;
   neuronSimulation    = false;
   neuronModelSim      = NULL;
   c302Simulation      = false;
   c302ModelSim        = NULL;
   randomizer          = NULL;
   sigterm             = false;
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
   getMotorConnections();
}


// Load constructor for undulation homomorphogenesis.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(int undulationMovements,
                                                   char *filename, bool binary)
{
   undulationBehavior        = true;
   this->undulationMovements = undulationMovements;
   neuronSimulation          = false;
   neuronModelSim            = NULL;
   c302Simulation            = false;
   c302ModelSim = NULL;
   randomizer   = NULL;
   sigterm      = false;
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
   getMotorConnections();
}


// Load constructor for homomorphogenesis with NEURON simulator evaluation.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(char *neuronExecPath, char *simDir,
                                                   char *simHocFile, char *filename, bool binary)
{
   neuronSimulation = true;
   char buf[BUFSIZ];
#ifdef WIN32
   this->neuronExecPath = _fullpath(buf, neuronExecPath, BUFSIZ);
#else
   this->neuronExecPath = realpath(neuronExecPath, buf);
#endif
   neuronSimDir        = simDir;
   neuronSimHocFile    = simHocFile;
   undulationBehavior  = false;
   undulationMovements = -1;
   c302Simulation      = false;
   c302ModelSim        = NULL;
   randomizer          = NULL;
   sigterm             = false;
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
   getMotorConnections();
   string workDir = neuronSimDir;
   workDir.append("/model");
   neuronModelSim = new NeuronSim(this->neuronExecPath, workDir, neuronSimHocFile);
   assert(neuronModelSim != NULL);
   neuronModelSim->importSynapseWeights(homomorph);
   neuronModelSim->run();
}


// Load constructor for homomorphogenesis with c302 simulator evaluation.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(char *jnmlCmdPath, char *simDir,
                                                   char *filename, bool binary)
{
   c302Simulation = true;
   char buf[BUFSIZ];
#ifdef WIN32
   this->jnmlCmdPath = _fullpath(buf, jnmlCmdPath, BUFSIZ);
#else
   this->jnmlCmdPath = realpath(jnmlCmdPath, buf);
#endif
   c302SimDir          = simDir;
   undulationBehavior  = false;
   undulationMovements = -1;
   neuronSimulation    = false;
   neuronModelSim      = NULL;
   randomizer          = NULL;
   sigterm             = false;
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
   getMotorConnections();
   string workDir = c302SimDir;
   workDir.append("/");
   workDir.append(c302RelativePath);
   workDir.append("/c302");
   c302ModelSim = new c302Sim(this->jnmlCmdPath, workDir);
   assert(c302ModelSim != NULL);
   c302ModelSim->importSynapseWeights(homomorph);
   c302ModelSim->run();
}


// Destructor.
NetworkHomomorphoGenesis::~NetworkHomomorphoGenesis()
{
   if (undulationBehavior)
   {
      for (int i = 0, j = (int)population.size(); i < j; i++)
      {
         delete (UndulationNetworkHomomorph *)population[i];
      }
   }
   else if (neuronSimulation)
   {
      for (int i = 0, j = (int)population.size(); i < j; i++)
      {
         delete (NeuronSimNetworkHomomorph *)population[i];
      }
      delete neuronModelSim;
      for (int i = 0, j = (int)neuronEvaluationSims.size(); i < j; i++)
      {
         delete neuronEvaluationSims[i];
      }
      neuronEvaluationSims.clear();
   }
   else if (c302Simulation)
   {
      for (int i = 0, j = (int)population.size(); i < j; i++)
      {
         delete (c302SimNetworkHomomorph *)population[i];
      }
      delete c302ModelSim;
      for (int i = 0, j = (int)c302EvaluationSims.size(); i < j; i++)
      {
         delete c302EvaluationSims[i];
      }
      c302EvaluationSims.clear();
   }
   else
   {
      for (int i = 0, j = (int)population.size(); i < j; i++)
      {
         delete (NetworkHomomorph *)population[i];
      }
   }
   population.clear();
   delete randomizer;
}


// Get motor connections.
void NetworkHomomorphoGenesis::getMotorConnections()
{
   int    i, j, k, m, n;
   Neuron *motor;

   queue<pair<Neuron *, int> > open;
   vector<bool>                closed;
   vector<pair<int, int> >     connections, culledConnections;
   pair<int, int>              node;

   motorConnections.clear();
   for (i = homomorph->numSensors, j = homomorph->numSensors + homomorph->numMotors; i < j; i++)
   {
      motor = homomorph->neurons[i];
      while (!open.empty())
      {
         open.pop();
      }
      open.push(pair<Neuron *, int>(motor, 0));
      closed.clear();
      closed.resize(homomorph->numNeurons, false);
      connections.clear();
      getMotorConnectionsSub(open, closed, connections);
      for (k = m = 0, n = (int)connections.size(); k < n; k++)
      {
         node = connections[k];
         m    = node.second;
         if (node.first < homomorph->numSensors)
         {
            break;
         }
      }
      culledConnections.clear();
      for (k = 0; k < n; k++)
      {
         node = connections[k];
         if (node.second <= m)
         {
            culledConnections.push_back(node);
         }
      }
      motorConnections.push_back(culledConnections);
   }
}


void NetworkHomomorphoGenesis::getMotorConnectionsSub(
   queue<pair<Neuron *, int> >& open,
   vector<bool>& closed,
   vector<pair<int, int> >& connections)
{
   int i, j;

   if (open.empty())
   {
      return;
   }
   pair<Neuron *, int> current = open.front();
   open.pop();
   Neuron *neuron = current.first;
   int    index   = neuron->index;
   int    depth   = current.second;
   closed[neuron->index] = true;
   for (i = 0, j = (int)connections.size(); i < j; i++)
   {
      if (connections[i].first == index)
      {
         break;
      }
   }
   if (i == j)
   {
      connections.push_back(pair<int, int>(index, depth));
   }
   if (index >= homomorph->numSensors)
   {
      for (i = 0; i < homomorph->numNeurons; i++)
      {
         if (homomorph->synapses[i][index].size() != 0)
         {
            neuron = homomorph->neurons[i];
            if (!closed[neuron->index])
            {
               open.push(pair<Neuron *, int>(neuron, depth + 1));
            }
         }
      }
   }
   getMotorConnectionsSub(open, closed, connections);
}


// Morph networks.
#ifdef THREADS
void NetworkHomomorphoGenesis::morph(int numGenerations, int numThreads,
                                     int behaveCutoff, char *logFile, char *saveFile)
#else
void NetworkHomomorphoGenesis::morph(int numGenerations, int behaveCutoff,
                                     char *logFile, char *saveFile)
#endif
{
   int i, g, n;
   int behaveCount;
   int maxBehaviorStep;
   UndulationNetworkHomomorph *undulationNetworkMorph;
   NeuronSimNetworkHomomorph  *neuronSimNetworkMorph;
   c302SimNetworkHomomorph    *c302SimNetworkMorph;

   if (logFile != NULL)
   {
      startMorphLog(logFile);
   }
#ifdef THREADS
   if (neuronSimulation)
   {
      // Initialize NEURON evaluation simulations.
      initNeuronEvaluationSims(numThreads);
   }
   if (c302Simulation)
   {
      // Initialize c302 evaluation simulations.
      initc302EvaluationSims(numThreads);
   }

   // Start additional morph threads.
   assert(numThreads > 0);
   terminate        = false;
   this->numThreads = numThreads;
   if (numThreads > 1)
   {
      if (pthread_barrier_init(&morphBarrier, NULL, numThreads) != 0)
      {
         fprintf(stderr, "pthread_barrier_init failed, errno=%d\n", errno);
         exit(1);
      }
      if (pthread_mutex_init(&morphMutex, NULL) != 0)
      {
         fprintf(stderr, "pthread_mutex_init failed, errno=%d\n", errno);
         exit(1);
      }
      threads = new pthread_t[numThreads - 1];
      assert(threads != NULL);
      struct ThreadInfo *info;
      for (int i = 0; i < numThreads - 1; i++)
      {
         info = new struct ThreadInfo;
         assert(info != NULL);
         info->morphoGenesis = this;
         info->threadNum     = i + 1;
         if (pthread_create(&threads[i], NULL, morphThread, (void *)info) != 0)
         {
            fprintf(stderr, "pthread_create failed, errno=%d\n", errno);
            exit(1);
         }
      }
   }
   fprintf(morphfp, "Threads=%d\n", numThreads);
#else
   if (neuronSim)
   {
      // Initialize NEURON evaluation simulation.
      initEvaluationSims(1);
   }
#endif

   if (behaviorStep != -1)
   {
      for (i = maxBehaviorStep = 0, n = (int)behaviors.size(); i < n; i++)
      {
         if (((int)behaviors[i]->sensorSequence.size() - 1) > maxBehaviorStep)
         {
            maxBehaviorStep = (int)behaviors[i]->sensorSequence.size() - 1;
         }
      }
   }
   evaluate();
   sort();
   fprintf(morphfp, "Generation=%d\n", generation);
   fprintf(morphfp, "Population:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   if (undulationBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         undulationNetworkMorph = (UndulationNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
      }
   }
   else if (neuronSimulation)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n", i, neuronSimNetworkMorph->tag,
                 neuronSimNetworkMorph->error, neuronSimNetworkMorph->meanError);
      }
   }
   else if (c302Simulation)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         c302SimNetworkMorph = (c302SimNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n", i, c302SimNetworkMorph->tag,
                 c302SimNetworkMorph->error, c302SimNetworkMorph->meanError);
      }
   }
   else
   {
      for (i = behaveCount = 0, n = (int)population.size(); i < n; i++)
      {
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
         if (population[i]->behaves)
         {
            behaveCount++;
         }
      }
      if (behaviorStep != -1)
      {
         fprintf(morphfp, "Behavior testing step=%d\n", behaviorStep);
      }
   }
   fflush(morphfp);
   for (g = 0; g < numGenerations; g++)
   {
      if (sigterm)
      {
         fprintf(morphfp, "Termination signal\n");
         fflush(morphfp);
         break;
      }
      if ((behaveCutoff != -1) && ((behaviorStep == -1) || (behaviorStep == maxBehaviorStep)))
      {
         if (behaveCount >= behaveCutoff)
         {
            fprintf(morphfp, "Reached behaving member cutoff=%d\n", behaveCutoff);
            fflush(morphfp);
            break;
         }
      }
      generation++;
      fprintf(morphfp, "Generation=%d\n", generation);
      offspring.resize(numOffspring);
      mate();
      mutate();
      optimize();
      prune();
      fprintf(morphfp, "Population:\n");
      fprintf(morphfp, "Member\tid\t\tfitness\n");
      if (undulationBehavior)
      {
         for (i = 0, n = (int)population.size(); i < n; i++)
         {
            undulationNetworkMorph = (UndulationNetworkHomomorph *)population[i];
            fprintf(morphfp, "%d\t%d\t\t%f\n", i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
         }
      }
      else if (neuronSimulation)
      {
         for (i = 0, n = (int)population.size(); i < n; i++)
         {
            neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n", i, neuronSimNetworkMorph->tag,
                    neuronSimNetworkMorph->error, neuronSimNetworkMorph->meanError);
         }
      }
      else if (c302Simulation)
      {
         for (i = 0, n = (int)population.size(); i < n; i++)
         {
            c302SimNetworkMorph = (c302SimNetworkHomomorph *)population[i];
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n", i, c302SimNetworkMorph->tag,
                    c302SimNetworkMorph->error, c302SimNetworkMorph->meanError);
         }
      }
      else
      {
         for (i = behaveCount = 0, n = (int)population.size(); i < n; i++)
         {
            fprintf(morphfp, "%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
            if (population[i]->behaves)
            {
               behaveCount++;
            }
         }
         if (behaviorStep != -1)
         {
            fprintf(morphfp, "Behavior testing step=%d\n", behaviorStep);
            bool maxGenerations = false;
            if (behaveQuorumMaxGenerations != -1)
            {
               behaveQuorumGenerationCount++;
               if (behaveQuorumGenerationCount >= behaveQuorumMaxGenerations)
               {
                  maxGenerations = true;
                  behaveQuorumGenerationCount = 0;
               }
            }
            if (((behaveCount >= behaveQuorum) || maxGenerations) &&
                (behaviorStep < maxBehaviorStep))
            {
               behaviorStep++;
               evaluate();
               sort();
               for (i = behaveCount = 0, n = (int)population.size(); i < n; i++)
               {
                  if (population[i]->behaves)
                  {
                     behaveCount++;
                  }
               }
            }
         }
      }
      fflush(morphfp);
      if (saveFile != NULL)
      {
         save(saveFile);
      }
   }

#ifdef THREADS
   // Terminate threads.
   if (numThreads > 1)
   {
      // Unblock threads waiting on morph barrier.
      terminate = true;
      mate(0);
      for (int i = 0; i < numThreads - 1; i++)
      {
         pthread_join(threads[i], NULL);
         pthread_detach(threads[i]);
      }
      delete threads;
      pthread_mutex_destroy(&morphMutex);
      pthread_barrier_destroy(&morphBarrier);
   }
#endif

   if (logFile != NULL)
   {
      stopMorphLog();
   }
}


#ifdef THREADS
// Morphogenesis thread.
void *NetworkHomomorphoGenesis::morphThread(void *arg)
{
   struct ThreadInfo        *info          = (struct ThreadInfo *)arg;
   NetworkHomomorphoGenesis *morphoGenesis = info->morphoGenesis;
   int threadNum = info->threadNum;

   delete info;
   while (true)
   {
      morphoGenesis->mate(threadNum);
      morphoGenesis->mutate(threadNum);
      morphoGenesis->optimize(threadNum);
   }
   return(NULL);
}


#endif

// Initialize NEURON evaluation simulations.
void NetworkHomomorphoGenesis::initNeuronEvaluationSims(int numSims)
{
   int       n;
   NeuronSim *evalSim;
   char      evalDir[BUFSIZ], modelDir[BUFSIZ];
   char      evalFile[BUFSIZ], modelFile[BUFSIZ];

   // Remove extra simulations.
   for (n = (int)neuronEvaluationSims.size(); n > numSims; n--)
   {
      delete neuronEvaluationSims[n - 1];
      neuronEvaluationSims.pop_back();
   }

#ifdef WIN32
   WIN32_FIND_DATA fileinfo;
   HANDLE          hp;
   sprintf(modelDir, "%s/model", neuronSimDir.c_str());
   for (int i = n; i < numSims; i++)
   {
      sprintf(evalDir, "%s/eval%d", neuronSimDir.c_str(), i);

      // Remove possible stale simulation files.
      sprintf(evalFile, "%s\\*.*", evalDir);
      hp = FindFirstFile(evalFile, &fileinfo);
      if (hp != INVALID_HANDLE_VALUE)
      {
         while (FindNextFile(hp, &fileinfo))
         {
            if (strcmp(fileinfo.cFileName, "..") != 0)
            {
               sprintf(evalFile, "%s\\%s", evalDir, fileinfo.cFileName);
               DeleteFile(evalFile);
            }
         }
         FindClose(hp);
         _rmdir(evalDir);
      }

      // Copy fresh model files.
      _mkdir(evalDir);
      sprintf(modelFile, "%s\\*.*", modelDir);
      hp = FindFirstFile(modelFile, &fileinfo);
      if (hp != INVALID_HANDLE_VALUE)
      {
         while (FindNextFile(hp, &fileinfo))
         {
            if (strcmp(fileinfo.cFileName, "..") != 0)
            {
               sprintf(modelFile, "%s/%s", modelDir, fileinfo.cFileName);
               sprintf(evalFile, "%s/%s", evalDir, fileinfo.cFileName);
               CopyFile(modelFile, evalFile, FALSE);
            }
         }
         FindClose(hp);
      }
#else
   char buf[BUFSIZ];
   sprintf(modelDir, "%s/model", neuronSimDir.c_str());
   for (int i = n; i < numSims; i++)
   {
      sprintf(evalDir, "%s/eval%d", neuronSimDir.c_str(), i);

      // Remove possible stale simulation.
      sprintf(buf, "/bin/rm -fr %s", evalDir);
      int result = system(buf);

      // Copy fresh model files.
      mkdir(evalDir, 0755);
      sprintf(buf, "/bin/cp -r %s/* %s", modelDir, evalDir);
      result = system(buf);
#endif
      // Add simulation.
      evalSim = new NeuronSim(neuronExecPath, evalDir, neuronSimHocFile);
      assert(evalSim != NULL);
      neuronEvaluationSims.push_back(evalSim);
   }
}


// Initialize c302 evaluation simulations.
void NetworkHomomorphoGenesis::initc302EvaluationSims(int numSims)
{
   int     n;
   c302Sim *evalSim;
   char    evalDir[BUFSIZ], modelDir[BUFSIZ];
   char    evalFile[BUFSIZ], modelFile[BUFSIZ];

   // Remove extra simulations.
   for (n = (int)c302EvaluationSims.size(); n > numSims; n--)
   {
      delete c302EvaluationSims[n - 1];
      c302EvaluationSims.pop_back();
   }

#ifdef WIN32
   WIN32_FIND_DATA fileinfo;
   HANDLE          hp;
   sprintf(modelDir, "%s/%s/c302", c302SimDir.c_str(), c302RelativePath.c_str());
   for (int i = n; i < numSims; i++)
   {
      sprintf(evalDir, "%s/%s/eval%d", c302SimDir.c_str(), c302RelativePath.c_str(), i);

      // Remove possible stale simulation files.
      sprintf(evalFile, "%s\\*.*", evalDir);
      hp = FindFirstFile(evalFile, &fileinfo);
      if (hp != INVALID_HANDLE_VALUE)
      {
         while (FindNextFile(hp, &fileinfo))
         {
            if (strcmp(fileinfo.cFileName, "..") != 0)
            {
               sprintf(evalFile, "%s\\%s", evalDir, fileinfo.cFileName);
               DeleteFile(evalFile);
            }
         }
         FindClose(hp);
         _rmdir(evalDir);
      }

      // Copy fresh model files.
      _mkdir(evalDir);
      sprintf(modelFile, "%s\\*.*", modelDir);
      hp = FindFirstFile(modelFile, &fileinfo);
      if (hp != INVALID_HANDLE_VALUE)
      {
         while (FindNextFile(hp, &fileinfo))
         {
            if ((strcmp(fileinfo.cFileName, "..") != 0) &&
                (strcmp(fileinfo.cFileName, "images") != 0))
            {
               sprintf(modelFile, "%s/%s", modelDir, fileinfo.cFileName);
               sprintf(evalFile, "%s/%s", evalDir, fileinfo.cFileName);
               CopyFile(modelFile, evalFile, FALSE);
            }
         }
         FindClose(hp);
      }
#else
   char buf[BUFSIZ];
   sprintf(modelDir, "%s/%s/c302", c302SimDir.c_str(), c302RelativePath.c_str());
   for (int i = n; i < numSims; i++)
   {
      sprintf(evalDir, "%s/%s/eval%d", c302SimDir.c_str(), c302RelativePath.c_str(), i);

      // Remove possible stale simulation.
      sprintf(buf, "/bin/rm -fr %s", evalDir);
      int result = system(buf);

      // Copy fresh model files.
      mkdir(evalDir, 0755);
      sprintf(buf, "/bin/cp -r %s/* %s", modelDir, evalDir);
      result = system(buf);
#endif
      // Add simulation.
      evalSim = new c302Sim(jnmlCmdPath, evalDir);
      assert(evalSim != NULL);
      c302EvaluationSims.push_back(evalSim);
   }
}


// Mate members.
void NetworkHomomorphoGenesis::mate()
{
   fprintf(morphfp, "Mate:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\t\tparents\n");
   mate(0);
}


void NetworkHomomorphoGenesis::mate(int threadNum)
{
   int     i, j, k, n, p1, p2;
   Network *parent1, *parent2, *parent, *child;
   UndulationNetworkHomomorph *undulationNetworkMorph;
   NeuronSimNetworkHomomorph  *neuronSimNetworkMorph;
   c302SimNetworkHomomorph    *c302SimNetworkMorph;

#ifdef THREADS
   // Synchronize threads.
   if (numThreads > 1)
   {
      i = pthread_barrier_wait(&morphBarrier);
      if ((i != PTHREAD_BARRIER_SERIAL_THREAD) && (i != 0))
      {
         fprintf(stderr, "pthread_barrier_wait failed, errno=%d\n", errno);
         exit(1);
      }
      if (terminate)
      {
         if (threadNum == 0)
         {
            return;
         }
         pthread_exit(NULL);
      }
   }
#endif

   assert(populationSize > 1);
   for (i = 0; i < numOffspring; i++)
   {
#ifdef THREADS
      // Divide work among threads.
      if ((i % numThreads) != threadNum)
      {
         continue;
      }
#endif

      p1      = randomizer->RAND_CHOICE(populationSize);
      parent1 = population[p1]->network;
#ifdef THREADS
      if (numThreads > 1)
      {
         pthread_mutex_lock(&morphMutex);
      }
#endif
      if (undulationBehavior)
      {
         offspring[i] = (NetworkMorph *)((UndulationNetworkHomomorph *)population[p1])->clone(
            NetworkMorphoGenesis::tagGenerator++);
      }
      else if (neuronSimulation)
      {
         offspring[i] = (NetworkMorph *)((NeuronSimNetworkHomomorph *)population[p1])->clone(
            NetworkMorphoGenesis::tagGenerator++);
      }
      else if (c302Simulation)
      {
         offspring[i] = (NetworkMorph *)((c302SimNetworkHomomorph *)population[p1])->clone(
            NetworkMorphoGenesis::tagGenerator++);
      }
      else
      {
         offspring[i] = (NetworkMorph *)((NetworkHomomorph *)population[p1])->clone(
            NetworkMorphoGenesis::tagGenerator++);
      }
      population[p1]->offspringCount++;
#ifdef THREADS
      if (numThreads > 1)
      {
         pthread_mutex_unlock(&morphMutex);
      }
#endif

      // Mate parents?
      if (randomizer->RAND_CHANCE(crossoverRate))
      {
         while ((p2 = randomizer->RAND_CHOICE(populationSize)) == p1)
         {
         }
         population[p2]->offspringCount++;
         parent2 = population[p2]->network;
         child   = offspring[i]->network;
         n       = child->numNeurons;
         for (k = 0; k < n; k++)
         {
            child->neurons[k]->index = -1;
         }

         // Crossover connected neurons.
         while (true)
         {
            j = randomizer->RAND_CHOICE(child->numNeurons);
            for (k = 0; k < n; k++)
            {
               if (child->neurons[j]->index == -1)
               {
                  if (randomizer->RAND_BOOL())
                  {
                     parent = parent1;
                  }
                  else
                  {
                     parent = parent2;
                  }
                  break;
               }
               else
               {
                  j++;
                  j = (j % n);
               }
            }
            if (k == n)
            {
               break;
            }
            crossover(child, parent, j, 0);
         }
         if (undulationBehavior)
         {
            undulationNetworkMorph = (UndulationNetworkHomomorph *)offspring[i];
            undulationNetworkMorph->evaluate();
            fprintf(morphfp, "%d\t%d\t\t%f\t%d %d\n",
                    i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness,
                    population[p1]->tag, population[p2]->tag);
         }
         else if (neuronSimulation)
         {
            neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
            neuronSimNetworkMorph->evaluate(neuronModelSim, neuronEvaluationSims[threadNum]);
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\t%d %d\n",
                    i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error,
                    neuronSimNetworkMorph->meanError, population[p1]->tag, population[p2]->tag);
         }
         else if (c302Simulation)
         {
            c302SimNetworkMorph = (c302SimNetworkHomomorph *)offspring[i];
            c302SimNetworkMorph->evaluate(c302ModelSim, c302EvaluationSims[threadNum]);
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\t%d %d\n",
                    i, c302SimNetworkMorph->tag, c302SimNetworkMorph->error,
                    c302SimNetworkMorph->meanError, population[p1]->tag, population[p2]->tag);
         }
         else
         {
            offspring[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
            fprintf(morphfp, "%d\t%d\t\t%f\t%d %d\n", i, offspring[i]->tag, offspring[i]->error,
                    population[p1]->tag, population[p2]->tag);
         }
      }
      else   // No crossover.
      {
         if (undulationBehavior)
         {
            undulationNetworkMorph = (UndulationNetworkHomomorph *)offspring[i];
            fprintf(morphfp, "%d\t%d\t\t%f\t%d\n",
                    i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness, population[p1]->tag);
         }
         else if (neuronSimulation)
         {
            neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\t%d\n",
                    i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error,
                    neuronSimNetworkMorph->meanError, population[p1]->tag);
         }
         else if (c302Simulation)
         {
            c302SimNetworkMorph = (c302SimNetworkHomomorph *)offspring[i];
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\t%d\n",
                    i, c302SimNetworkMorph->tag, c302SimNetworkMorph->error,
                    c302SimNetworkMorph->meanError, population[p1]->tag);
         }
         else
         {
            fprintf(morphfp, "%d\t%d\t\t%f\t%d\n", i, offspring[i]->tag, offspring[i]->error, population[p1]->tag);
         }
      }
   }

#ifdef THREADS
   // Re-group threads.
   if (numThreads > 1)
   {
      pthread_barrier_wait(&morphBarrier);
   }
#endif
}


// Crossover neurons.
void NetworkHomomorphoGenesis::crossover(Network *child, Network *parent,
                                         int index, int distance)
{
   int   i, j, k, n;
   float b;

   child->neurons[index]->index      = index;
   child->neurons[index]->excitatory = parent->neurons[index]->excitatory;
   child->neurons[index]->function   = parent->neurons[index]->function;
   child->neurons[index]->bias       = parent->neurons[index]->bias;
   child->neurons[index]->activation = parent->neurons[index]->activation;
   for (i = 0, n = child->numNeurons; i < n; i++)
   {
      for (j = 0, k = (int)child->synapses[index][i].size(); j < k; j++)
      {
         child->synapses[index][i][j]->weight = parent->synapses[index][i][j]->weight;
         child->synapses[index][i][j]->signal = parent->synapses[index][i][j]->signal;
      }
   }

   // Crossover neighboring neurons.
   b = synapseCrossoverBondStrength;
   for (i = 0; i < distance; i++)
   {
      b *= synapseCrossoverBondStrength;
   }
   j = randomizer->RAND_CHOICE(n);
   for (i = 0; i < n; i++)
   {
      if (child->neurons[j]->index == -1)
      {
         if (child->synapses[index][j].size() != 0)
         {
            if (randomizer->RAND_CHANCE(b))
            {
               crossover(child, parent, j, distance + 1);
            }
         }
         if (child->synapses[j][index].size() != 0)
         {
            if (randomizer->RAND_CHANCE(b))
            {
               crossover(child, parent, j, distance + 1);
            }
         }
      }
      j++;
      j = (j % n);
   }
}


// Mutate offspring.
void NetworkHomomorphoGenesis::mutate()
{
   fprintf(morphfp, "Mutate:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   mutate(0);
}


void NetworkHomomorphoGenesis::mutate(int threadNum)
{
   int                        i;
   NetworkHomomorph           *networkMorph;
   UndulationNetworkHomomorph *undulationNetworkMorph;
   NeuronSimNetworkHomomorph  *neuronSimNetworkMorph;
   c302SimNetworkHomomorph    *c302SimNetworkMorph;

#ifdef THREADS
   // Re-group threads.
   if (numThreads > 1)
   {
      pthread_barrier_wait(&morphBarrier);
   }
#endif

   for (i = 0; i < numOffspring; i++)
   {
#ifdef THREADS
      if ((i % numThreads) != threadNum)
      {
         continue;
      }
#endif
      if (undulationBehavior)
      {
         undulationNetworkMorph = (UndulationNetworkHomomorph *)offspring[i];
         if (randomizer->RAND_CHANCE(mutationRate))
         {
            undulationNetworkMorph->mutate();
            undulationNetworkMorph->evaluate();
            fprintf(morphfp, "%d\t%d\t\t%f\n",
                    i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
         }
      }
      else if (neuronSimulation)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
         if (randomizer->RAND_CHANCE(mutationRate))
         {
            neuronSimNetworkMorph->mutate();
            neuronSimNetworkMorph->evaluate(neuronModelSim, neuronEvaluationSims[threadNum]);
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n",
                    i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error,
                    neuronSimNetworkMorph->meanError);
         }
      }
      else if (c302Simulation)
      {
         c302SimNetworkMorph = (c302SimNetworkHomomorph *)offspring[i];
         if (randomizer->RAND_CHANCE(mutationRate))
         {
            c302SimNetworkMorph->mutate();
            c302SimNetworkMorph->evaluate(c302ModelSim, c302EvaluationSims[threadNum]);
            fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n",
                    i, c302SimNetworkMorph->tag, c302SimNetworkMorph->error,
                    c302SimNetworkMorph->meanError);
         }
      }
      else
      {
         networkMorph = (NetworkHomomorph *)offspring[i];
         if (randomizer->RAND_CHANCE(mutationRate))
         {
            networkMorph->mutate();
            offspring[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
            fprintf(morphfp, "%d\t%d\t\t%f\n", i, offspring[i]->tag, offspring[i]->error);
         }
      }
   }

#ifdef THREADS
   // Re-group threads.
   if (numThreads > 1)
   {
      pthread_barrier_wait(&morphBarrier);
   }
#endif
}


// Optimize offspring.
void NetworkHomomorphoGenesis::optimize()
{
   fprintf(morphfp, "Optimize:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   optimize(0);
}


void NetworkHomomorphoGenesis::optimize(int threadNum)
{
   int                        i;
   NetworkHomomorph           *networkMorph;
   UndulationNetworkHomomorph *undulationNetworkMorph;
   NeuronSimNetworkHomomorph  *neuronSimNetworkMorph;
   c302SimNetworkHomomorph    *c302SimNetworkMorph;

#ifdef THREADS
   // Re-group threads.
   if (numThreads > 1)
   {
      pthread_barrier_wait(&morphBarrier);
   }
#endif

   for (i = 0; i < numOffspring; i++)
   {
#ifdef THREADS
      if ((i % numThreads) != threadNum)
      {
         continue;
      }
#endif
      if (undulationBehavior)
      {
         undulationNetworkMorph = (UndulationNetworkHomomorph *)offspring[i];
         undulationNetworkMorph->optimize(synapseOptimizedPathLength);
         fprintf(morphfp, "%d\t%d\t\t%f\n",
                 i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
      }
      else if (neuronSimulation)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
         neuronSimNetworkMorph->optimize(synapseOptimizedPathLength,
                                         neuronModelSim, neuronEvaluationSims[threadNum]);
         fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n",
                 i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error,
                 neuronSimNetworkMorph->meanError);
      }
      else if (c302Simulation)
      {
         c302SimNetworkMorph = (c302SimNetworkHomomorph *)offspring[i];
         c302SimNetworkMorph->optimize(synapseOptimizedPathLength,
                                       c302ModelSim, c302EvaluationSims[threadNum]);
         fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n",
                 i, c302SimNetworkMorph->tag, c302SimNetworkMorph->error,
                 c302SimNetworkMorph->meanError);
      }
      else
      {
         networkMorph = (NetworkHomomorph *)offspring[i];
         networkMorph->optimize(behaviors, fitnessMotorList,
                                synapseOptimizedPathLength, behaviorStep);
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, offspring[i]->tag, offspring[i]->error);
      }
   }

#ifdef THREADS
   // Re-group threads.
   if (numThreads > 1)
   {
      pthread_barrier_wait(&morphBarrier);
   }
#endif
}


// Prune members.
void NetworkHomomorphoGenesis::prune()
{
   int                        i, j, n;
   NetworkHomomorph           *networkMorph;
   UndulationNetworkHomomorph *undulationNetworkMorph;
   NeuronSimNetworkHomomorph  *neuronSimNetworkMorph;
   c302SimNetworkHomomorph    *c302SimNetworkMorph;

   fprintf(morphfp, "Prune:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      if (undulationBehavior)
      {
         undulationNetworkMorph = (UndulationNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
         delete undulationNetworkMorph;
      }
      else if (neuronSimulation)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n", i, neuronSimNetworkMorph->tag,
                 neuronSimNetworkMorph->error, neuronSimNetworkMorph->meanError);
         delete neuronSimNetworkMorph;
      }
      else if (c302Simulation)
      {
         c302SimNetworkMorph = (c302SimNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)\n", i, c302SimNetworkMorph->tag,
                 c302SimNetworkMorph->error, c302SimNetworkMorph->meanError);
         delete c302SimNetworkMorph;
      }
      else
      {
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
         delete (NetworkHomomorph *)population[i];
      }
      population[i] = offspring[j];
   }
   offspring.clear();
   if (parentLongevity != -1)
   {
      fprintf(morphfp, "Longevity replace:\n");
      fprintf(morphfp, "Member\tid\t\told fitness\tnew fitness\n");
      for (i = 0; i < n; i++)
      {
         if (population[i]->offspringCount > parentLongevity)
         {
            if (undulationBehavior)
            {
               undulationNetworkMorph = (UndulationNetworkHomomorph *)population[i];
               fprintf(morphfp, "%d\t%d\t\t%f", i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
               population[i] = (NetworkMorph *)new UndulationNetworkHomomorph(
                  undulationMovements, homomorph, networkMorph->synapseWeightsParm,
                  &motorConnections, randomizer, NetworkMorphoGenesis::tagGenerator++);
               assert(population[i] != NULL);
               delete undulationNetworkMorph;
               undulationNetworkMorph = (UndulationNetworkHomomorph *)population[i];
               undulationNetworkMorph->evaluate();
               fprintf(morphfp, "\t%f\n", undulationNetworkMorph->fitness);
            }
            else if (neuronSimulation)
            {
               neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
               fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)", i, neuronSimNetworkMorph->tag,
                       neuronSimNetworkMorph->error, neuronSimNetworkMorph->meanError);
               population[i] = (NetworkMorph *)new NeuronSimNetworkHomomorph(
                  homomorph, networkMorph->synapseWeightsParm, &motorConnections,
                  randomizer, NetworkMorphoGenesis::tagGenerator++);
               assert(population[i] != NULL);
               delete neuronSimNetworkMorph;
               neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
               neuronSimNetworkMorph->evaluate(neuronModelSim, neuronEvaluationSims[0]);
               fprintf(morphfp, "\t%.2f (%.2f)\n", neuronSimNetworkMorph->error,
                       neuronSimNetworkMorph->meanError);
            }
            else if (c302Simulation)
            {
               c302SimNetworkMorph = (c302SimNetworkHomomorph *)population[i];
               fprintf(morphfp, "%d\t%d\t\t%.2f (%.2f)", i, c302SimNetworkMorph->tag,
                       c302SimNetworkMorph->error, c302SimNetworkMorph->meanError);
               population[i] = (NetworkMorph *)new c302SimNetworkHomomorph(
                  homomorph, networkMorph->synapseWeightsParm, &motorConnections,
                  randomizer, NetworkMorphoGenesis::tagGenerator++);
               assert(population[i] != NULL);
               delete c302SimNetworkMorph;
               c302SimNetworkMorph = (c302SimNetworkHomomorph *)population[i];
               c302SimNetworkMorph->evaluate(c302ModelSim, c302EvaluationSims[0]);
               fprintf(morphfp, "\t%.2f (%.2f)\n", c302SimNetworkMorph->error,
                       c302SimNetworkMorph->meanError);
            }
            else
            {
               fprintf(morphfp, "%d\t%d\t\t%f", i, population[i]->tag, population[i]->error);
               networkMorph  = (NetworkHomomorph *)population[i];
               population[i] = (NetworkMorph *)new NetworkHomomorph(
                  homomorph, networkMorph->synapseWeightsParm, &motorConnections,
                  randomizer, NetworkMorphoGenesis::tagGenerator++);
               assert(population[i] != NULL);
               delete networkMorph;
               population[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
               fprintf(morphfp, "\t%f\n", population[i]->error);
            }
         }
      }
   }
   sort();
}


// Evaluate behavior.
void NetworkHomomorphoGenesis::evaluate()
{
   int i, n;

   if (undulationBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((UndulationNetworkHomomorph *)population[i])->evaluate();
      }
   }
   else if (neuronSimulation)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((NeuronSimNetworkHomomorph *)population[i])->evaluate(neuronModelSim, neuronEvaluationSims[0]);
      }
   }
   else if (c302Simulation)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((c302SimNetworkHomomorph *)population[i])->evaluate(c302ModelSim, c302EvaluationSims[0]);
      }
   }
   else
   {
      ((NetworkMorphoGenesis *)this)->evaluate();
   }
}


// Sort members by fitness.
void NetworkHomomorphoGenesis::sort()
{
   int i, j, n;
   UndulationNetworkHomomorph *undulationNetworkMorph1, *undulationNetworkMorph2;
   NetworkMorph               *networkMorph;

   if (undulationBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         undulationNetworkMorph1 = (UndulationNetworkHomomorph *)population[i];
         for (j = i + 1; j < n; j++)
         {
            undulationNetworkMorph2 = (UndulationNetworkHomomorph *)population[j];
            if (undulationNetworkMorph2->fitness > undulationNetworkMorph1->fitness)
            {
               undulationNetworkMorph1 = undulationNetworkMorph2;
               networkMorph            = population[i];
               population[i]           = population[j];
               population[j]           = networkMorph;
            }
         }
      }
   }
   else
   {
      ((NetworkMorphoGenesis *)this)->sort();
   }
}


// Load morph.
bool NetworkHomomorphoGenesis::load(char *filename, bool binary)
{
   int                        i, j, n, format;
   FilePointer                *fp;
   NetworkHomomorph           *networkMorph;
   UndulationNetworkHomomorph *undulationNetworkMorph;
   NeuronSimNetworkHomomorph  *neuronSimNetworkMorph;
   c302SimNetworkHomomorph    *c302SimNetworkMorph;

   if ((fp = FOPEN_READ(filename, binary)) == NULL)
   {
      fprintf(stderr, "Cannot load from file %s\n", filename);
      return(false);
   }

   // Check format compatibility.
   FREAD_INT(&format, fp);
   if (format != FORMAT)
   {
      fprintf(stderr, "File format %d is incompatible with expected format %d\n", format, FORMAT);
      return(false);
   }

   if (randomizer != NULL)
   {
      delete randomizer;
   }
   randomizer = new Random();
   assert(randomizer != NULL);
   randomizer->RAND_LOAD(fp);
   homomorph = new Network(fp);
   assert(homomorph != NULL);
   FREAD_FLOAT(&crossoverRate, fp);
   FREAD_FLOAT(&mutationRate, fp);
   FREAD_FLOAT(&synapseCrossoverBondStrength, fp);
   FREAD_INT(&synapseOptimizedPathLength, fp);
   FREAD_INT(&populationSize, fp);
   FREAD_INT(&numOffspring, fp);
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      delete population[i];
   }
   population.clear();
   offspring.clear();
   FREAD_INT(&n, fp);
   if (undulationBehavior)
   {
      for (i = 0; i < n; i++)
      {
         undulationNetworkMorph =
            new UndulationNetworkHomomorph(fp, undulationMovements, &motorConnections, randomizer);
         assert(undulationNetworkMorph != NULL);
         population.push_back((NetworkMorph *)undulationNetworkMorph);
         if (undulationNetworkMorph->tag >= NetworkMorphoGenesis::tagGenerator)
         {
            NetworkMorphoGenesis::tagGenerator = undulationNetworkMorph->tag + 1;
         }
      }
   }
   else if (neuronSimulation)
   {
      for (i = 0; i < n; i++)
      {
         neuronSimNetworkMorph = new NeuronSimNetworkHomomorph(fp, &motorConnections, randomizer);
         assert(neuronSimNetworkMorph != NULL);
         population.push_back((NetworkMorph *)neuronSimNetworkMorph);
         if (neuronSimNetworkMorph->tag >= NetworkMorphoGenesis::tagGenerator)
         {
            NetworkMorphoGenesis::tagGenerator = neuronSimNetworkMorph->tag + 1;
         }
      }
   }
   else if (c302Simulation)
   {
      for (i = 0; i < n; i++)
      {
         c302SimNetworkMorph = new c302SimNetworkHomomorph(fp, &motorConnections, randomizer);
         assert(c302SimNetworkMorph != NULL);
         population.push_back((NetworkMorph *)c302SimNetworkMorph);
         if (c302SimNetworkMorph->tag >= NetworkMorphoGenesis::tagGenerator)
         {
            NetworkMorphoGenesis::tagGenerator = c302SimNetworkMorph->tag + 1;
         }
      }
   }
   else
   {
      for (i = 0; i < n; i++)
      {
         networkMorph = new NetworkHomomorph(fp, &motorConnections, randomizer);
         assert(networkMorph != NULL);
         population.push_back((NetworkMorph *)networkMorph);
         if (networkMorph->tag >= NetworkMorphoGenesis::tagGenerator)
         {
            NetworkMorphoGenesis::tagGenerator = networkMorph->tag + 1;
         }
      }
   }
   FREAD_INT(&parentLongevity, fp);
   fitnessMotorList.clear();
   FREAD_INT(&n, fp);
   if (n > 0)
   {
      fitnessMotorList.resize((int)homomorph->numMotors, false);
      for (i = 0; i < n; i++)
      {
         FREAD_INT(&j, fp);
         fitnessMotorList[j] = true;
      }
   }
   FREAD_INT(&behaveQuorum, fp);
   FREAD_INT(&behaveQuorumMaxGenerations, fp);
   FREAD_INT(&behaviorStep, fp);
   FREAD_INT(&behaveQuorumGenerationCount, fp);
   FREAD_LONG(&randomSeed, fp);
   FREAD_INT(&generation, fp);
   FCLOSE(fp);
   return(true);
}


// Save morph.
bool NetworkHomomorphoGenesis::save(char *filename, bool binary)
{
   int         i, j, n, format;
   FilePointer *fp;

   if ((fp = FOPEN_WRITE(filename, binary)) == NULL)
   {
      fprintf(stderr, "Cannot save to file %s\n", filename);
      return(false);
   }
   format = FORMAT;
   FWRITE_INT(&format, fp);
   randomizer->RAND_SAVE(fp);
   homomorph->save(fp);
   FWRITE_FLOAT(&crossoverRate, fp);
   FWRITE_FLOAT(&mutationRate, fp);
   FWRITE_FLOAT(&synapseCrossoverBondStrength, fp);
   FWRITE_INT(&synapseOptimizedPathLength, fp);
   FWRITE_INT(&populationSize, fp);
   FWRITE_INT(&numOffspring, fp);
   n = (int)population.size();
   FWRITE_INT(&n, fp);
   if (undulationBehavior)
   {
      for (i = 0; i < n; i++)
      {
         ((UndulationNetworkHomomorph *)population[i])->save(fp);
      }
   }
   else if (neuronSimulation)
   {
      for (i = 0; i < n; i++)
      {
         ((NeuronSimNetworkHomomorph *)population[i])->save(fp);
      }
   }
   else if (c302Simulation)
   {
      for (i = 0; i < n; i++)
      {
         ((c302SimNetworkHomomorph *)population[i])->save(fp);
      }
   }
   else
   {
      for (i = 0; i < n; i++)
      {
         ((NetworkHomomorph *)population[i])->save(fp);
      }
   }
   FWRITE_INT(&parentLongevity, fp);
   n = (int)fitnessMotorList.size();
   for (i = j = 0; i < n; i++)
   {
      if (fitnessMotorList[i])
      {
         j++;
      }
   }
   FWRITE_INT(&j, fp);
   for (i = 0; i < n; i++)
   {
      if (fitnessMotorList[i])
      {
         FWRITE_INT(&i, fp);
      }
   }
   FWRITE_INT(&behaveQuorum, fp);
   FWRITE_INT(&behaveQuorumMaxGenerations, fp);
   FWRITE_INT(&behaviorStep, fp);
   FWRITE_INT(&behaveQuorumGenerationCount, fp);
   FWRITE_LONG(&randomSeed, fp);
   FWRITE_INT(&generation, fp);
   FCLOSE(fp);
   return(true);
}


// Print.
void NetworkHomomorphoGenesis::print(bool printNetwork)
{
   int i, n;

   printf("FORMAT=%d\n", FORMAT);
   printf("behaviors:\n");
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      behaviors[i]->print();
   }
   if (printNetwork)
   {
      printf("homomorph:\n");
      homomorph->print();
   }
   printf("populationSize=%d\n", populationSize);
   printf("numOffspring=%d\n", numOffspring);
   printf("parentLongevity=%d\n", parentLongevity);
   if (undulationBehavior)
   {
      printf("undulationMovements=%d\n", undulationMovements);
   }
   else if (neuronSimulation)
   {
      printf("neuronExec=%s\n", neuronExecPath.c_str());
      printf("simDir=%s\n", neuronSimDir.c_str());
   }
   else if (c302Simulation)
   {
      printf("jnmlCmd=%s\n", jnmlCmdPath.c_str());
      printf("simDir=%s\n", c302SimDir.c_str());
   }
   else
   {
      if (fitnessMotorList.size() > 0)
      {
         printf("Fitness motor list:\n");
         for (i = 0, n = (int)fitnessMotorList.size(); i < n; i++)
         {
            if (fitnessMotorList[i])
            {
               printf("%d ", i);
            }
         }
         printf("\n");
      }
      printf("behaveQuorum=%d\n", behaveQuorum);
      printf("behaveQuorumMaxGenerations=%d\n", behaveQuorumMaxGenerations);
   }
   printf("crossoverRate=%f\n", crossoverRate);
   printf("mutationRate=%f\n", mutationRate);
   printf("synapseCrossoverBondStrength=%f\n", synapseCrossoverBondStrength);
   printf("synapseOptimizedPathLength=%d\n", synapseOptimizedPathLength);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   if (undulationBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((UndulationNetworkHomomorph *)population[i])->print(printNetwork);
      }
   }
   else if (neuronSimulation)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((NeuronSimNetworkHomomorph *)population[i])->print(printNetwork);
      }
   }
   else if (c302Simulation)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((c302SimNetworkHomomorph *)population[i])->print(printNetwork);
      }
   }
   else
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((NetworkHomomorph *)population[i])->print(printNetwork);
      }
   }
}
