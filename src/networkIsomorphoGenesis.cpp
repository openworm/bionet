// Network isomorphogenesis implementation.

#include "networkIsomorphoGenesis.hpp"

// Constructor.
NetworkIsomorphoGenesis::NetworkIsomorphoGenesis(vector<Behavior *>& behaviors,
                                                 int populationSize, int numOffspring,
                                                 int behaveQuorum, int behaveQuorumMaxGenerations,
                                                 MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                                                 MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                                                 RANDOM randomSeed)
{
   int             i, j, numSensors, numMotors;
   NetworkIsomorph *networkMorph;

   this->randomSeed = randomSeed;
   randomizer       = new Random(randomSeed);
   assert(randomizer != NULL);
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   assert(numOffspring <= populationSize);
   this->populationSize = populationSize;
   this->numOffspring   = numOffspring;
   numOffspring         = -1;
   this->behaveQuorum   = behaveQuorum;
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
   generation = 0;
   assert(behaviors.size() > 0);
   numSensors = (int)behaviors[0]->sensorSequence[0].size();
   numMotors  = (int)behaviors[0]->motorSequence[0].size();
   for (i = 0; i < populationSize; i++)
   {
      networkMorph = new NetworkIsomorph(excitatoryNeuronsParm, inhibitoryNeuronsParm,
                                         synapsePropensitiesParm, synapseWeightsParm,
                                         numSensors, numMotors, randomizer);
      assert(networkMorph != NULL);
      population.push_back((NetworkMorph *)networkMorph);
   }
}


// Load constructor.
NetworkIsomorphoGenesis::NetworkIsomorphoGenesis(vector<Behavior *>& behaviors,
                                                 char *filename, bool binary)
{
   int i, j;

   randomizer = NULL;
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   if (!load(filename, binary))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      exit(1);
   }
}


// Destructor.
NetworkIsomorphoGenesis::~NetworkIsomorphoGenesis()
{
   for (int i = 0, j = (int)population.size(); i < j; i++)
   {
      delete (NetworkIsomorph *)population[i];
   }
   population.clear();
   delete randomizer;
}


// Morph networks.
#ifdef THREADS
void NetworkIsomorphoGenesis::morph(int numGenerations,
                                    int numThreads, char *logFile)
#else
void NetworkIsomorphoGenesis::morph(int numGenerations, char *logFile)
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
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      fprintf(morphfp, "%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
   }
   if (behaviorStep != -1)
   {
      fprintf(morphfp, "Behavior testing step=%d\n", behaviorStep);
   }
   fflush(morphfp);
   for (g = 0; g < numGenerations; g++)
   {
      generation++;
      fprintf(morphfp, "Generation=%d\n", generation);
      mutate();
      prune();
      fprintf(morphfp, "Population:\n");
      fprintf(morphfp, "Member\tid\t\tfitness\n");
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
      mutate(0);
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
void *NetworkIsomorphoGenesis::morphThread(void *arg)
{
   struct ThreadInfo       *info          = (struct ThreadInfo *)arg;
   NetworkIsomorphoGenesis *morphoGenesis = info->morphoGenesis;
   int threadNum = info->threadNum;

   delete info;
   while (true)
   {
      morphoGenesis->mutate(threadNum);
   }
   return(NULL);
}


#endif


// Mutate members.
void NetworkIsomorphoGenesis::mutate()
{
   fprintf(morphfp, "Mutate:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   mutate(0);
}


void NetworkIsomorphoGenesis::mutate(int threadNum)
{
   int i, j;

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

   offspring.resize(numOffspring);
   for (i = 0; i < numOffspring; i++)
   {
#ifdef THREADS
      // Divide work among threads.
      if ((i % numThreads) != threadNum)
      {
         continue;
      }
#endif

      j = randomizer->RAND_CHOICE(populationSize);
#ifdef THREADS
      if (numThreads > 1)
      {
         pthread_mutex_lock(&morphMutex);
      }
#endif
      offspring[i] = (NetworkMorph *)((NetworkIsomorph *)population[j])->clone(
         NetworkMorphoGenesis::tagGenerator++);
#ifdef THREADS
      if (numThreads > 1)
      {
         pthread_mutex_unlock(&morphMutex);
      }
#endif
      ((NetworkIsomorph *)offspring[i])->mutate();
      offspring[i]->evaluate(behaviors, behaviorStep);
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


// Prune less fit members.
void NetworkIsomorphoGenesis::prune()
{
   int i, j, n;

   fprintf(morphfp, "Prune:\n");
   fprintf(morphfp, "Member\tid\t\tfitness\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      fprintf(morphfp, "%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
      delete (NetworkIsomorph *)population[i];
      population[i] = offspring[j];
   }
   offspring.clear();
   sort();
}


// Load morph.
bool NetworkIsomorphoGenesis::load(char *filename, bool binary)
{
   int         i, n, format;
   FilePointer *fp;

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
   FREAD_INT(&populationSize, fp);
   FREAD_INT(&numOffspring, fp);
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      delete (NetworkIsomorph *)population[i];
   }
   population.clear();
   offspring.clear();
   FREAD_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      NetworkIsomorph *networkMorph = new NetworkIsomorph(fp, randomizer);
      assert(networkMorph != NULL);
      population.push_back((NetworkMorph *)networkMorph);
      if (networkMorph->tag >= NetworkMorphoGenesis::tagGenerator)
      {
         NetworkMorphoGenesis::tagGenerator = networkMorph->tag + 1;
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
bool NetworkIsomorphoGenesis::save(char *filename, bool binary)
{
   int         i, n, format;
   FilePointer *fp;

   if ((fp = FOPEN_WRITE(filename, binary)) == NULL)
   {
      fprintf(stderr, "Cannot save to file %s\n", filename);
      return(false);
   }
   format = FORMAT;
   FWRITE_INT(&format, fp);
   randomizer->RAND_SAVE(fp);
   FWRITE_INT(&populationSize, fp);
   FWRITE_INT(&numOffspring, fp);
   n = (int)population.size();
   FWRITE_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      ((NetworkIsomorph *)population[i])->save(fp);
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
void NetworkIsomorphoGenesis::print(bool printNetwork)
{
   int i, n;

   printf("FORMAT=%d\n", FORMAT);
   printf("behaviors:\n");
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      behaviors[i]->print();
   }
   printf("populationSize=%d\n", populationSize);
   printf("numOffspring=%d\n", numOffspring);
   printf("behaveQuorum=%d\n", behaveQuorum);
   printf("behaveQuorumMaxGenerations=%d\n", behaveQuorumMaxGenerations);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      ((NetworkIsomorph *)population[i])->print(printNetwork);
   }
}
