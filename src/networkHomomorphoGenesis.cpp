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
   neuronSim           = false;
   modelSim            = NULL;
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
   neuronSim    = false;
   modelSim     = NULL;
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


// Constructor with NEURON network simulator fitness evaluation.
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
   neuronSim = true;
   char buf[BUFSIZ];
#ifdef WIN32
   this->neuronExecPath = _fullpath(buf, neuronExecPath, BUFSIZ);
#else
   this->neuronExecPath = realpath(neuronExecPath, buf);
#endif
   this->simDir                = simDir;
   this->simHocFile            = simHocFile;
   undulationBehavior          = false;
   undulationMovements         = -1;
   behaveQuorum                = -1;
   behaviorStep                = -1;
   behaveQuorumMaxGenerations  = -1;
   behaveQuorumGenerationCount = -1;

   // Initialize model simulation.
   string workDir = this->simDir;
   workDir.append("/model");
   modelSim = new NeuronSim(this->neuronExecPath, workDir, this->simHocFile);
   assert(modelSim != NULL);
   modelSim->exportSynapses(homomorph);
   modelSim->importSynapseWeights(homomorph);

   // Common initialization.
   init(homomorph,
        populationSize, numOffspring, parentLongevity,
        crossoverRate, mutationRate,
        synapseWeightsParm,
        synapseCrossoverBondStrength,
        synapseOptimizedPathLength,
        randomSeed);

   // Run model simulation to produce activations.
   modelSim->run();
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
      else if (neuronSim)
      {
         networkMorph = (NetworkMorph *)new NeuronSimNetworkHomomorph(
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
   neuronSim           = false;
   modelSim            = NULL;
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
   neuronSim  = false;
   modelSim   = NULL;
   randomizer = NULL;
   sigterm    = false;
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
   getMotorConnections();
}


// Load constructor for homomorphogenesis with NEURON network simulator evaluation.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(char *neuronExecPath, char *simDir,
                                                   char *simHocFile, char *filename, bool binary)
{
   neuronSim = true;
   char buf[BUFSIZ];
#ifdef WIN32
   this->neuronExecPath = _fullpath(buf, neuronExecPath, BUFSIZ);
#else
   this->neuronExecPath = realpath(neuronExecPath, buf);
#endif
   this->simDir        = simDir;
   this->simHocFile    = simHocFile;
   undulationBehavior  = false;
   undulationMovements = -1;
   randomizer          = NULL;
   sigterm             = false;
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
   getMotorConnections();
   string workDir = this->simDir;
   workDir.append("/model");
   modelSim = new NeuronSim(this->neuronExecPath, workDir, this->simHocFile);
   assert(modelSim != NULL);
   modelSim->importSynapseWeights(homomorph);
   modelSim->run();
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
   else if (neuronSim)
   {
      for (int i = 0, j = (int)population.size(); i < j; i++)
      {
         delete (NeuronSimNetworkHomomorph *)population[i];
      }
      delete modelSim;
      for (int i = 0, j = (int)evaluationSims.size(); i < j; i++)
      {
         delete evaluationSims[i];
      }
      evaluationSims.clear();
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

   if (logFile != NULL)
   {
      startMorphLog(logFile);
   }
#ifdef THREADS
   if (neuronSim)
   {
      // Initialize NEURON evaluation simulations.
      initEvaluationSims(numThreads);
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
   else if (neuronSim)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error);
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
      else if (neuronSim)
      {
         for (i = 0, n = (int)population.size(); i < n; i++)
         {
            neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
            fprintf(morphfp, "%d\t%d\t\t%f\n", i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error);
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
void NetworkHomomorphoGenesis::initEvaluationSims(int numSims)
{
   int       n;
   NeuronSim *evalSim;
   char      evalDir[BUFSIZ], modelDir[BUFSIZ];
   char      evalFile[BUFSIZ], modelFile[BUFSIZ];

   // Remove extra simulations.
   for (n = (int)evaluationSims.size(); n > numSims; n--)
   {
      delete evaluationSims[n - 1];
      evaluationSims.pop_back();
   }

#ifdef WIN32
   WIN32_FIND_DATA fileinfo;
   HANDLE          hp;
   sprintf(modelDir, "%s/model", simDir.c_str());
   for (int i = n; i < numSims; i++)
   {
      sprintf(evalDir, "%s/eval%d", simDir.c_str(), i);

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
   sprintf(modelDir, "%s/model", simDir.c_str());
   for (int i = n; i < numSims; i++)
   {
      sprintf(evalDir, "%s/eval%d", simDir.c_str(), i);

      // Remove possible stale simulation.
      sprintf(buf, "/bin/rm -fr %s", evalDir);
      int result = system(buf);

      // Copy fresh model files.
      mkdir(evalDir, 0755);
      sprintf(buf, "/bin/cp -r %s/* %s", modelDir, evalDir);
      result = system(buf);
#endif
      // Add simulation.
      evalSim = new NeuronSim(neuronExecPath, evalDir, simHocFile);
      assert(evalSim != NULL);
      evaluationSims.push_back(evalSim);
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
      else if (neuronSim)
      {
         offspring[i] = (NetworkMorph *)((NeuronSimNetworkHomomorph *)population[p1])->clone(
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
         else if (neuronSim)
         {
            neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
            neuronSimNetworkMorph->evaluate(modelSim, evaluationSims[threadNum]);
            fprintf(morphfp, "%d\t%d\t\t%f\t%d %d\n",
                    i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error,
                    population[p1]->tag, population[p2]->tag);
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
         else if (neuronSim)
         {
            neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
            fprintf(morphfp, "%d\t%d\t\t%f\t%d\n",
                    i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error, population[p1]->tag);
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
      else if (neuronSim)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
         if (randomizer->RAND_CHANCE(mutationRate))
         {
            neuronSimNetworkMorph->mutate();
            neuronSimNetworkMorph->evaluate(modelSim, evaluationSims[threadNum]);
            fprintf(morphfp, "%d\t%d\t\t%f\n",
                    i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error);
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
      else if (neuronSim)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)offspring[i];
         neuronSimNetworkMorph->optimize(synapseOptimizedPathLength,
                                         modelSim, evaluationSims[threadNum]);
         fprintf(morphfp, "%d\t%d\t\t%f\n",
                 i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error);
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

   fprintf(morphfp, "Prune:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      if (undulationBehavior)
      {
         undulationNetworkMorph = (UndulationNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, undulationNetworkMorph->tag, undulationNetworkMorph->fitness);
         delete (UndulationNetworkHomomorph *)undulationNetworkMorph;
      }
      else if (neuronSim)
      {
         neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error);
         delete (NeuronSimNetworkHomomorph *)neuronSimNetworkMorph;
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
            else if (neuronSim)
            {
               neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
               fprintf(morphfp, "%d\t%d\t\t%f", i, neuronSimNetworkMorph->tag, neuronSimNetworkMorph->error);
               population[i] = (NetworkMorph *)new NeuronSimNetworkHomomorph(
                  homomorph, networkMorph->synapseWeightsParm, &motorConnections,
                  randomizer, NetworkMorphoGenesis::tagGenerator++);
               assert(population[i] != NULL);
               delete neuronSimNetworkMorph;
               neuronSimNetworkMorph = (NeuronSimNetworkHomomorph *)population[i];
               neuronSimNetworkMorph->evaluate(modelSim, evaluationSims[0]);
               fprintf(morphfp, "\t%f\n", neuronSimNetworkMorph->error);
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
   else if (neuronSim)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((NeuronSimNetworkHomomorph *)population[i])->evaluate(modelSim, evaluationSims[0]);
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
   else if (neuronSim)
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
   else if (neuronSim)
   {
      for (i = 0; i < n; i++)
      {
         ((NeuronSimNetworkHomomorph *)population[i])->save(fp);
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
   else if (neuronSim)
   {
      printf("neuronExec=%s\n", neuronExecPath.c_str());
      printf("simDir=%s\n", simDir.c_str());
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
   else if (neuronSim)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((NeuronSimNetworkHomomorph *)population[i])->print(printNetwork);
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
