// Network homomorph implementation.

#include "networkHomomorph.hpp"
#include <math.h>
#include <errno.h>

// Constructor.
NetworkHomomorph::NetworkHomomorph(Network *homomorph, MutableParm& synapseWeightsParm,
                                   Random *randomizer, int tag)
{
   this->synapseWeightsParm = synapseWeightsParm;
   this->synapseWeightsParm.initValue(randomizer);
   this->randomizer = randomizer;
   this->tag        = tag;
   network          = homomorph->clone();
}


NetworkHomomorph::NetworkHomomorph(FILE *fp, Random *randomizer)
{
   network          = NULL;
   this->randomizer = randomizer;
   load(fp);
}


// Destructor.
NetworkHomomorph::~NetworkHomomorph()
{
   delete network;
}


// Mutate synapses.
void NetworkHomomorph::mutate()
{
   int     i, j, n;
   Synapse *synapse;

   n = network->numNeurons;
   while (true)
   {
      i = randomizer->RAND_CHOICE(n);
      if ((i < network->numSensors) || (i >= (network->numSensors + network->numMotors)))
      {
         break;
      }
   }
   for (j = 0; j < n; j++)
   {
      if ((synapse = network->synapses[i][j]) != NULL)
      {
         synapse->weight = (float)randomizer->RAND_INTERVAL(
            synapseWeightsParm.minimum, synapseWeightsParm.maximum);
      }
   }
}


// Harmonize synapses.
void NetworkHomomorph::harmonize(vector<Behavior *>& behaviors,
                                 vector<bool>& fitnessMotorList,
                                 int synapseOptimizedPathLength, int maxStep)
{
   int   i, j, k, n, s;
   bool  forward;
   float weight, e;

   vector<Synapse *>      synapses;
   vector<vector<float> > weightRanges;
   vector<float>          weightRange;
   vector<vector<float> > permutations;
   vector<float>          permutation;

   n = network->numNeurons;
   i = randomizer->RAND_CHOICE(n);
   if (i < network->numSensors)
   {
      forward = true;
   }
   else if ((i >= network->numSensors) &&
            (i < (network->numSensors + network->numMotors)))
   {
      forward = false;
   }
   else
   {
      forward = randomizer->RAND_BOOL();
   }
   for (j = 0; j < synapseOptimizedPathLength; j++)
   {
      k = randomizer->RAND_CHOICE(n);
      for (s = 0; s < n; s++)
      {
         if (forward)
         {
            if (network->synapses[i][k] != NULL)
            {
               synapses.push_back(network->synapses[i][k]);
               i = k;
               break;
            }
         }
         else
         {
            if (network->synapses[k][i] != NULL)
            {
               synapses.push_back(network->synapses[k][i]);
               i = k;
               break;
            }
         }
         k++;
         k = (k % n);
      }
      if (s == n)
      {
         break;
      }
   }

   if ((int)synapses.size() == 0)
   {
      return;
   }

   // Hill-climb synapse weight permutations.
   for (i = 0, j = (int)synapses.size(); i < j; i++)
   {
      weightRange.clear();
      weight = synapses[i]->weight;
      weightRange.push_back(weight);
      if (synapseWeightsParm.maxDelta > 0.0f)
      {
         weight = synapses[i]->weight -
                  (float)randomizer->RAND_INTERVAL(0.0f, synapseWeightsParm.maxDelta);
         if (weight < synapseWeightsParm.minimum)
         {
            weight = synapseWeightsParm.minimum;
         }
      }
      weightRange.push_back(weight);
      if (synapseWeightsParm.maxDelta > 0.0f)
      {
         weight = synapses[i]->weight +
                  (float)randomizer->RAND_INTERVAL(0.0f, synapseWeightsParm.maxDelta);
         if (weight > synapseWeightsParm.maximum)
         {
            weight = synapseWeightsParm.maximum;
         }
      }
      weightRange.push_back(weight);
      weightRanges.push_back(weightRange);
   }
   permutation.resize((int)synapses.size());
   permuteWeights(weightRanges, permutations, permutation, 0, (int)synapses.size() - 1);
   n = 0;
   e = error;
   for (i = 1, j = (int)permutations.size(); i < j; i++)
   {
      for (k = 0; k < (int)synapses.size(); k++)
      {
         synapses[k]->weight = permutations[i][k];
      }
      evaluate(behaviors, fitnessMotorList, maxStep);
      if (error < e)
      {
         n = i;
         e = error;
      }
   }
   for (k = 0; k < (int)synapses.size(); k++)
   {
      synapses[k]->weight = permutations[n][k];
   }
   error = e;
}


// Permute weights.
void NetworkHomomorph::permuteWeights(vector<vector<float> >& weightRanges,
                                      vector<vector<float> >& permutations,
                                      vector<float>& permutation,
                                      int level, int depth)
{
   for (int i = 0; i < 3; i++)
   {
      permutation[level] = weightRanges[level][i];
      if (level < depth)
      {
         permuteWeights(weightRanges, permutations, permutation, level + 1, depth);
      }
      else
      {
         permutations.push_back(permutation);
      }
   }
}


// Clone.
NetworkHomomorph *NetworkHomomorph::clone()
{
   NetworkHomomorph *networkMorph;

   networkMorph = new NetworkHomomorph(
      network, synapseWeightsParm, randomizer, tag);
   assert(networkMorph != NULL);
   networkMorph->error   = error;
   networkMorph->behaves = behaves;
   return(networkMorph);
}


// Load.
void NetworkHomomorph::load(FILE *fp)
{
   synapseWeightsParm.load(fp);
   if (network != NULL)
   {
      delete network;
   }
   network = new Network(fp);
   assert(network != NULL);
   FREAD_INT(&tag, fp);
   FREAD_FLOAT(&error, fp);
   FREAD_BOOL(&behaves, fp);
   FREAD_INT(&offspringCount, fp);
}


// Save.
void NetworkHomomorph::save(FILE *fp)
{
   synapseWeightsParm.save(fp);
   network->save(fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
   FWRITE_BOOL(&behaves, fp);
   FWRITE_INT(&offspringCount, fp);
}


// Print.
void NetworkHomomorph::print(bool printNetwork)
{
   if (printNetwork)
   {
      printf("Network:\n");
      if (network != NULL)
      {
         network->print();
      }
      else
      {
         printf("NULL\n");
      }
   }
   printf("tag=%d\n", tag);
   printf("error=%f\n", error);
   printf("behaves=");
   if (behaves)
   {
      printf("true\n");
   }
   else
   {
      printf("false\n");
   }
   printf("offspringCount=%d\n", offspringCount);
   printf("synapseWeightsParm:\n");
   synapseWeightsParm.print();
}


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
   int              i, j, k, n;
   NetworkHomomorph *networkMorph;
   Network          *network;
   Synapse          *synapse;

   this->randomSeed = randomSeed;
   randomizer       = new Random(randomSeed);
   assert(randomizer != NULL);
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   this->homomorph = homomorph;
   assert(numOffspring <= populationSize);
   this->populationSize  = populationSize;
   this->numOffspring    = numOffspring;
   this->parentLongevity = parentLongevity;
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
   this->crossoverRate = crossoverRate;
   this->mutationRate  = mutationRate;
   this->synapseCrossoverBondStrength = synapseCrossoverBondStrength;
   this->synapseOptimizedPathLength   = synapseOptimizedPathLength;
   generation = 0;
   for (i = 0; i < populationSize; i++)
   {
      networkMorph = new NetworkHomomorph(
         homomorph, synapseWeightsParm, randomizer);
      assert(networkMorph != NULL);
      network = ((NetworkMorph *)networkMorph)->network;
      n       = network->numNeurons;
      for (j = 0; j < n; j++)
      {
         for (k = 0; k < n; k++)
         {
            synapse = network->synapses[j][k];
            if (synapse != NULL)
            {
               synapse->weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
            }
         }
      }
      population.push_back((NetworkMorph *)networkMorph);
   }
}


// Load constructor.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(vector<Behavior *>& behaviors, char *filename)
{
   int i, j;

   randomizer = NULL;
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   if (!load(filename))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      assert(false);
   }
}


// Destructor.
NetworkHomomorphoGenesis::~NetworkHomomorphoGenesis()
{
   for (int i = 0, j = (int)population.size(); i < j; i++)
   {
      delete (NetworkHomomorph *)population[i];
   }
   population.clear();
   delete randomizer;
}


// Morph networks.
#ifdef THREADS
void NetworkHomomorphoGenesis::morph(int numGenerations, int numThreads,
                                     int behaveCutoff, char *logFile)
#else
void NetworkHomomorphoGenesis::morph(int numGenerations, int behaveCutoff,
                                     char *logFile)
#endif
{
   int i, g, n;
   int behaveCount;
   int maxBehaviorStep;

   if (logFile != NULL)
   {
      startMorphLog(logFile);
   }
#ifdef THREADS
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
   fprintf(morphfp, "Member\tgeneration\tfitness\n");
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
   fflush(morphfp);
   for (g = 0; g < numGenerations; g++)
   {
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
      optimize();
      prune();
      fprintf(morphfp, "Population:\n");
      fprintf(morphfp, "Member\tgeneration\tfitness\n");
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
      fflush(morphfp);
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
      morphoGenesis->optimize(threadNum);
   }
   return(NULL);
}


#endif

// Mate members.
void NetworkHomomorphoGenesis::mate()
{
   fprintf(morphfp, "Mate:\n");
   fprintf(morphfp, "Member\tgeneration\tfitness\t\tparents\n");
   mate(0);
}


void NetworkHomomorphoGenesis::mate(int threadNum)
{
   int     i, j, k, n, p1, p2;
   Network *parent1, *parent2, *parent, *child;

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
      offspring[i] = (NetworkMorph *)((NetworkHomomorph *)population[p1])->clone();
      population[p1]->offspringCount++;
#ifdef THREADS
      if (numThreads > 1)
      {
         pthread_mutex_unlock(&morphMutex);
      }
#endif

      // Crossover parents?
      if (randomizer->RAND_CHANCE(crossoverRate))
      {
         while ((p2 = randomizer->RAND_CHOICE(populationSize)) == p1)
         {
         }
         population[p2]->offspringCount++;
         parent2 = population[p2]->network;
         if (population[p1]->tag > population[p2]->tag)
         {
            offspring[i]->tag = population[p1]->tag + 1;
         }
         else
         {
            offspring[i]->tag = population[p2]->tag + 1;
         }
         child = offspring[i]->network;
         n     = child->numNeurons;
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
         offspring[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
         fprintf(morphfp, "%d\t%d\t\t%f\t%d %d\n", i, offspring[i]->tag, offspring[i]->error, p1, p2);
      }
      else   // No crossover.
      {
         offspring[i]->tag++;
         fprintf(morphfp, "%d\t%d\t\t%f\t%d\n", i, offspring[i]->tag, offspring[i]->error, p1);
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
   int   i, j, n;
   float b;

   child->neurons[index]->index      = index;
   child->neurons[index]->excitatory = parent->neurons[index]->excitatory;
   child->neurons[index]->function   = parent->neurons[index]->function;
   child->neurons[index]->bias       = parent->neurons[index]->bias;
   child->neurons[index]->activation = parent->neurons[index]->activation;
   for (i = 0, n = child->numNeurons; i < n; i++)
   {
      if (child->synapses[index][i] != NULL)
      {
         child->synapses[index][i]->weight = parent->synapses[index][i]->weight;
         child->synapses[index][i]->signal = parent->synapses[index][i]->signal;
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
         if (child->synapses[index][j] != NULL)
         {
            if (randomizer->RAND_CHANCE(b))
            {
               crossover(child, parent, j, distance + 1);
            }
         }
         if (child->synapses[j][index] != NULL)
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


// Optimize offspring.
void NetworkHomomorphoGenesis::optimize()
{
   fprintf(morphfp, "Optimize:\n");
   fprintf(morphfp, "Member\tgeneration\tfitness\n");
   optimize(0);
}


void NetworkHomomorphoGenesis::optimize(int threadNum)
{
   int              i;
   NetworkHomomorph *networkMorph;

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

      networkMorph = (NetworkHomomorph *)offspring[i];
      if (randomizer->RAND_CHANCE(mutationRate))
      {
         networkMorph->mutate();
         offspring[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
      }
      networkMorph->harmonize(behaviors, fitnessMotorList,
                              synapseOptimizedPathLength, behaviorStep);
      fprintf(morphfp, "%d\t%d\t\t%f\n", i, offspring[i]->tag, offspring[i]->error);
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
   int              i, j, n;
   NetworkHomomorph *networkMorph;

   fprintf(morphfp, "Prune:\n");
   fprintf(morphfp, "Member\tgeneration\tfitness\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      fprintf(morphfp, "%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
      delete (NetworkHomomorph *)population[i];
      population[i] = offspring[j];
   }
   offspring.clear();
   if (parentLongevity != -1)
   {
      fprintf(morphfp, "Longevity replace:\n");
      fprintf(morphfp, "Member\tgeneration\told fitness\tnew fitness\n");
      for (i = 0; i < n; i++)
      {
         if (population[i]->offspringCount > parentLongevity)
         {
            fprintf(morphfp, "%d\t%d\t\t%f", i, population[i]->tag, population[i]->error);
            networkMorph  = (NetworkHomomorph *)population[i];
            population[i] = (NetworkMorph *)new NetworkHomomorph(
               homomorph, networkMorph->synapseWeightsParm, randomizer);
            assert(population[i] != NULL);
            delete networkMorph;
            population[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
            fprintf(morphfp, "\t%f\n", population[i]->error);
         }
      }
   }
   sort();
}


// Load morph.
bool NetworkHomomorphoGenesis::load(char *filename)
{
   int  i, j, n;
   FILE *fp;

   if ((fp = fopen(filename, "r")) == NULL)
   {
      fprintf(stderr, "Cannot load from file %s\n", filename);
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
   for (i = 0; i < n; i++)
   {
      NetworkHomomorph *networkMorph = new NetworkHomomorph(fp, randomizer);
      assert(networkMorph != NULL);
      population.push_back((NetworkMorph *)networkMorph);
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
   return(true);
}


// Save morph.
bool NetworkHomomorphoGenesis::save(char *filename)
{
   int  i, j, n;
   FILE *fp;

   if ((fp = fopen(filename, "w")) == NULL)
   {
      fprintf(stderr, "Cannot save to file %s\n", filename);
      return(false);
   }
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
   for (i = 0; i < n; i++)
   {
      ((NetworkHomomorph *)population[i])->save(fp);
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
   return(true);
}


// Print.
void NetworkHomomorphoGenesis::print(bool printNetwork)
{
   int i, n;

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
   printf("crossoverRate=%f\n", crossoverRate);
   printf("mutationRate=%f\n", mutationRate);
   printf("synapseCrossoverBondStrength=%f\n", synapseCrossoverBondStrength);
   printf("synapseOptimizedPathLength=%d\n", synapseOptimizedPathLength);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      ((NetworkHomomorph *)population[i])->print(printNetwork);
   }
}
