// Network isomorph implementation.

#include "networkIsomorph.hpp"
#include <math.h>

// Constructor.
NetworkIsomorph::NetworkIsomorph(MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                                 MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                                 int numSensors, int numMotors, Random *randomizer, int tag)
{
   this->excitatoryNeuronsParm   = excitatoryNeuronsParm;
   this->inhibitoryNeuronsParm   = inhibitoryNeuronsParm;
   this->synapsePropensitiesParm = synapsePropensitiesParm;
   this->synapseWeightsParm      = synapseWeightsParm;
   this->randomizer = randomizer;
   this->tag        = tag;
   this->excitatoryNeuronsParm.initValue(randomizer);
   this->inhibitoryNeuronsParm.initValue(randomizer);
   this->synapsePropensitiesParm.initValue(randomizer);
   this->synapseWeightsParm.initValue(randomizer);
   int numInterneurons = (int)this->excitatoryNeuronsParm.value +
                         (int)this->inhibitoryNeuronsParm.value;
   int   numNeurons       = numSensors + numMotors + numInterneurons;
   float inhibitorDensity = 0.0f;
   if (numInterneurons > 0)
   {
      inhibitorDensity = this->inhibitoryNeuronsParm.value /
                         (this->excitatoryNeuronsParm.value + this->inhibitoryNeuronsParm.value);
   }
   float synapsePropensity = this->synapsePropensitiesParm.value;
   network = new Network(numNeurons, numSensors, numMotors,
                         inhibitorDensity, synapsePropensity,
                         synapseWeightsParm.minimum, synapseWeightsParm.maximum,
                         randomizer->RAND());
   assert(network != NULL);
}


NetworkIsomorph::NetworkIsomorph(FILE *fp, Random *randomizer)
{
   network          = NULL;
   this->randomizer = randomizer;
   load(fp);
}


// Destructor.
NetworkIsomorph::~NetworkIsomorph()
{
   delete network;
}


// Mutate.
void NetworkIsomorph::mutate()
{
   int     i, j, k, n;
   int     numExcitatory, newExcitatory, numInhibitory, newInhibitory, numSynapses;
   float   synapsePropensity, newSynapsePropensity, weight;
   Synapse *synapse;

   // Mutate network parameters.
   numExcitatory = numInhibitory = numSynapses = 0;
   for (i = 0, k = network->numSensors + network->numMotors,
        n = network->numNeurons; i < n; i++)
   {
      if (i >= k)
      {
         if (network->neurons[i]->excitatory)
         {
            numExcitatory++;
         }
         else
         {
            numInhibitory++;
         }
      }
      for (j = 0; j < n; j++)
      {
         if (network->synapses[i][j] != NULL)
         {
            numSynapses++;
         }
      }
   }
   synapsePropensity = (float)numSynapses / (float)(network->numNeurons * network->numNeurons);
   synapsePropensitiesParm.setValue(synapsePropensity);
   newExcitatory        = numExcitatory;
   newInhibitory        = numInhibitory;
   newSynapsePropensity = synapsePropensity;
   if (!behaves && randomizer->RAND_CHANCE(excitatoryNeuronsParm.randomProbability))
   {
      newExcitatory = randomizer->RAND_CHOICE(
         (int)(excitatoryNeuronsParm.maximum - excitatoryNeuronsParm.minimum) + 1) +
                      (int)excitatoryNeuronsParm.minimum;
   }
   else
   {
      if (excitatoryNeuronsParm.maxDelta > 0.0f)
      {
         if (randomizer->RAND_BOOL())
         {
            newExcitatory += randomizer->RAND_CHOICE((int)excitatoryNeuronsParm.maxDelta + 1);
            if (newExcitatory > (int)excitatoryNeuronsParm.maximum)
            {
               newExcitatory = (int)excitatoryNeuronsParm.maximum;
            }
         }
         else
         {
            newExcitatory -= randomizer->RAND_CHOICE((int)excitatoryNeuronsParm.maxDelta + 1);
            if (newExcitatory < (int)excitatoryNeuronsParm.minimum)
            {
               newExcitatory = (int)excitatoryNeuronsParm.minimum;
            }
         }
      }
   }
   if (!behaves && randomizer->RAND_CHANCE(inhibitoryNeuronsParm.randomProbability))
   {
      newInhibitory = randomizer->RAND_CHOICE(
         (int)(inhibitoryNeuronsParm.maximum - inhibitoryNeuronsParm.minimum) + 1) +
                      (int)inhibitoryNeuronsParm.minimum;
   }
   else
   {
      if (inhibitoryNeuronsParm.maxDelta > 0.0f)
      {
         if (randomizer->RAND_BOOL())
         {
            newInhibitory += randomizer->RAND_CHOICE((int)inhibitoryNeuronsParm.maxDelta + 1);
            if (newInhibitory > (int)inhibitoryNeuronsParm.maximum)
            {
               newInhibitory = (int)inhibitoryNeuronsParm.maximum;
            }
         }
         else
         {
            newInhibitory -= randomizer->RAND_CHOICE((int)inhibitoryNeuronsParm.maxDelta + 1);
            if (newInhibitory < (int)inhibitoryNeuronsParm.minimum)
            {
               newInhibitory = (int)inhibitoryNeuronsParm.minimum;
            }
         }
      }
   }
   if (!behaves && randomizer->RAND_CHANCE(synapsePropensitiesParm.randomProbability))
   {
      newSynapsePropensity = (float)randomizer->RAND_INTERVAL(
         synapsePropensitiesParm.minimum, synapsePropensitiesParm.maximum);
   }
   else
   {
      if (synapsePropensitiesParm.maxDelta > 0.0f)
      {
         if (randomizer->RAND_BOOL())
         {
            newSynapsePropensity +=
               (float)randomizer->RAND_INTERVAL(0.0, synapsePropensitiesParm.maxDelta);
            if (newSynapsePropensity > synapsePropensitiesParm.maximum)
            {
               newSynapsePropensity = synapsePropensitiesParm.maximum;
            }
         }
         else
         {
            newSynapsePropensity -=
               (float)randomizer->RAND_INTERVAL(0.0, synapsePropensitiesParm.maxDelta);
            if (newSynapsePropensity < synapsePropensitiesParm.minimum)
            {
               newSynapsePropensity = synapsePropensitiesParm.minimum;
            }
         }
      }
   }

   // Mutate network?
   if ((newExcitatory != numExcitatory) || (newInhibitory != numInhibitory))
   {
      if (newExcitatory < numExcitatory)
      {
         for (i = 0, j = numExcitatory - newExcitatory,
              k = network->numSensors + network->numMotors; i < j; i++)
         {
            while (true)
            {
               n = randomizer->RAND_CHOICE(network->numNeurons);
               if ((n >= k) && network->neurons[n]->excitatory)
               {
                  deleteIndexedNeuron(n);
                  break;
               }
            }
         }
      }
      if (newInhibitory < numInhibitory)
      {
         for (i = 0, j = numInhibitory - newInhibitory,
              k = network->numSensors + network->numMotors; i < j; i++)
         {
            while (true)
            {
               n = randomizer->RAND_CHOICE(network->numNeurons);
               if ((n >= k) && !network->neurons[n]->excitatory)
               {
                  deleteIndexedNeuron(n);
                  break;
               }
            }
         }
      }
      if (newExcitatory > numExcitatory)
      {
         for (i = 0, j = newExcitatory - numExcitatory; i < j; i++)
         {
            addIndexedNeuron(network->numNeurons, true);
         }
      }
      if (newInhibitory > numInhibitory)
      {
         for (i = 0, j = newInhibitory - numInhibitory; i < j; i++)
         {
            addIndexedNeuron(network->numNeurons, false);
         }
      }

      // Ensure network connectivity.
      n = network->numSensors + network->numMotors;
      while (!network->isConnected())
      {
         i = randomizer->RAND_CHOICE(network->numNeurons);
         j = randomizer->RAND_CHOICE(network->numNeurons);
         if (i == j)
         {
            continue;
         }
         if ((network->numNeurons > n) && (i < network->numSensors) &&
             (j >= network->numSensors) && (j < n))
         {
            continue;
         }
         if (network->synapses[i][j] == NULL)
         {
            if (((i < network->numSensors) || (i >= n)) && (j >= network->numSensors))
            {
               weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
               network->synapses[i][j] = new Synapse(weight);
               assert(network->synapses[i][j] != NULL);
            }
         }
      }
   }

   // Mutate synapse propensity?
   if (newSynapsePropensity != synapsePropensity)
   {
      numSynapses = 0;
      for (i = 0, n = network->numNeurons; i < n; i++)
      {
         for (j = 0; j < n; j++)
         {
            if (network->synapses[i][j] != NULL)
            {
               numSynapses++;
            }
         }
      }
      n = network->numNeurons * network->numNeurons;
      int newNumSynapses = (int)(newSynapsePropensity * (float)n);
      if (newNumSynapses > numSynapses)
      {
         int   deltaSynapses        = newNumSynapses - numSynapses;
         int   numAvailableSynapses = n - numSynapses;
         float deltaPropensity      = (float)deltaSynapses / (float)numAvailableSynapses;
         n = network->numSensors + network->numMotors;
         for (i = 0; i < network->numNeurons; i++)
         {
            for (j = 0; j < network->numNeurons; j++)
            {
               if (i == j)
               {
                  continue;
               }
               if ((network->numNeurons > n) && (i < network->numSensors) &&
                   (j >= network->numSensors) && (j < n))
               {
                  continue;
               }
               if (network->synapses[i][j] == NULL)
               {
                  if (((i < network->numSensors) || (i >= n)) && (j >= network->numSensors))
                  {
                     if (randomizer->RAND_CHANCE(deltaPropensity))
                     {
                        weight = (float)randomizer->RAND_INTERVAL(
                           synapseWeightsParm.minimum, synapseWeightsParm.maximum);
                        network->synapses[i][j] = new Synapse(weight);
                        assert(network->synapses[i][j] != NULL);
                        numSynapses++;
                     }
                  }
               }
            }
         }
      }
      else if (newNumSynapses < numSynapses)
      {
         int   deltaSynapses   = numSynapses - newNumSynapses;
         float deltaPropensity = (float)deltaSynapses / (float)numSynapses;
         for (i = 0, n = network->numNeurons; i < n; i++)
         {
            for (j = 0; j < n; j++)
            {
               if ((synapse = network->synapses[i][j]) != NULL)
               {
                  if (randomizer->RAND_CHANCE(deltaPropensity))
                  {
                     network->synapses[i][j] = NULL;
                     if (network->isConnected())
                     {
                        delete synapse;
                        numSynapses--;
                     }
                     else
                     {
                        network->synapses[i][j] = synapse;
                     }
                  }
               }
            }
         }
      }
   }
   synapsePropensity = (float)numSynapses / (float)(network->numNeurons * network->numNeurons);
   synapsePropensitiesParm.setValue(synapsePropensity);

   // Mutate synapse weights.
   for (i = 0, n = network->numNeurons; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         if ((synapse = network->synapses[i][j]) != NULL)
         {
            if (!behaves && randomizer->RAND_CHANCE(synapseWeightsParm.randomProbability))
            {
               synapse->weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
            }
            else
            {
               if (synapseWeightsParm.maxDelta > 0.0f)
               {
                  if (randomizer->RAND_BOOL())
                  {
                     synapse->weight += (float)randomizer->RAND_INTERVAL(
                        0.0, synapseWeightsParm.maxDelta);
                     if (synapse->weight > synapseWeightsParm.maximum)
                     {
                        synapse->weight = synapseWeightsParm.maximum;
                     }
                  }
                  else
                  {
                     synapse->weight -= (float)randomizer->RAND_INTERVAL(
                        0.0, synapseWeightsParm.maxDelta);
                     if (synapse->weight < synapseWeightsParm.minimum)
                     {
                        synapse->weight = synapseWeightsParm.minimum;
                     }
                  }
               }
            }
         }
      }
   }
}


// Delete neuron at index.
void NetworkIsomorph::deleteIndexedNeuron(int index)
{
   int i, j, k, n;

   delete network->neurons[index];
   network->neurons[index] = NULL;
   n = network->numNeurons;
   network->numNeurons--;
   for (i = 0; i < n; i++)
   {
      if (network->synapses[index][i] != NULL)
      {
         delete network->synapses[index][i];
         network->synapses[index][i] = NULL;
      }
   }
   for (i = index, j = n - 1; i < j; i++)
   {
      network->neurons[i] = network->neurons[i + 1];
      network->neurons[i]->index--;
      network->neurons[i + 1] = NULL;
      for (k = 0; k < n; k++)
      {
         network->synapses[i][k]     = network->synapses[i + 1][k];
         network->synapses[i + 1][k] = NULL;
      }
   }
   for (i = 0; i < n; i++)
   {
      if (network->synapses[i][index] != NULL)
      {
         delete network->synapses[i][index];
         network->synapses[i][index] = NULL;
      }
      for (k = index, j = n - 1; k < j; k++)
      {
         network->synapses[i][k]     = network->synapses[i][k + 1];
         network->synapses[i][k + 1] = NULL;
      }
      network->synapses[i].resize(network->numNeurons);
   }
   network->neurons.resize(network->numNeurons);
   network->synapses.resize(network->numNeurons);
}


// Add neuron at index.
void NetworkIsomorph::addIndexedNeuron(int index, bool excitatory)
{
   int    i, j, n;
   float  synapsePropensity, weight;
   Neuron *neuron;

   network->numNeurons++;
   n = network->numNeurons;
   j = n - 1;
   network->neurons.resize(n);
   network->neurons[j] = NULL;
   network->synapses.resize(n);
   for (i = 0; i < n; i++)
   {
      network->synapses[i].resize(n);
      network->synapses[i][j] = NULL;
   }
   for (i = 0; i < j; i++)
   {
      network->synapses[j][i] = NULL;
   }

   neuron = new Neuron(network, index, excitatory);
   assert(neuron != NULL);
   network->neurons[index] = neuron;

   synapsePropensity = synapsePropensitiesParm.value;
   n = network->numSensors + network->numMotors;
   for (i = 0, j = network->numNeurons; i < j; i++)
   {
      if (i == index)
      {
         continue;
      }
      if ((network->numNeurons > n) && (index < network->numSensors) &&
          (i >= network->numSensors) && (i < n))
      {
         continue;
      }
      if (network->synapses[index][i] == NULL)
      {
         if (((index < network->numSensors) || (index >= n)) && (i >= network->numSensors))
         {
            if (randomizer->RAND_CHANCE(synapsePropensity))
            {
               weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
               network->synapses[index][i] = new Synapse(weight);
               assert(network->synapses[index][i] != NULL);
            }
         }
      }
   }
   for (i = 0, j = network->numNeurons - 1; i < j; i++)
   {
      if (i == index)
      {
         continue;
      }
      if ((network->numNeurons > n) && (i < network->numSensors) &&
          (index >= network->numSensors) && (index < n))
      {
         continue;
      }
      if (network->synapses[i][index] == NULL)
      {
         if (((i < network->numSensors) || (i >= n)) && (index >= network->numSensors))
         {
            if (randomizer->RAND_CHANCE(synapsePropensity))
            {
               weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
               network->synapses[i][index] = new Synapse(weight);
               assert(network->synapses[i][index] != NULL);
            }
         }
      }
   }
}


// Clone.
NetworkIsomorph *NetworkIsomorph::clone()
{
   NetworkIsomorph *networkMorph;

   networkMorph = new NetworkIsomorph(
      excitatoryNeuronsParm, inhibitoryNeuronsParm,
      synapsePropensitiesParm, synapseWeightsParm,
      network->numSensors, network->numMotors, randomizer, tag);
   assert(networkMorph != NULL);
   networkMorph->error   = error;
   networkMorph->behaves = behaves;
   delete networkMorph->network;
   networkMorph->network = network->clone();
   return(networkMorph);
}


// Load.
void NetworkIsomorph::load(FILE *fp)
{
   excitatoryNeuronsParm.load(fp);
   inhibitoryNeuronsParm.load(fp);
   synapsePropensitiesParm.load(fp);
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
}


// Save.
void NetworkIsomorph::save(FILE *fp)
{
   excitatoryNeuronsParm.save(fp);
   inhibitoryNeuronsParm.save(fp);
   synapsePropensitiesParm.save(fp);
   synapseWeightsParm.save(fp);
   network->save(fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
   FWRITE_BOOL(&behaves, fp);
}


// Print.
void NetworkIsomorph::print()
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
}


// Constructor.
NetworkIsomorphoGenesis::NetworkIsomorphoGenesis(vector<Behavior *>& behaviors,
                                                 int populationSize, int numOffspring, int fitnessQuorum,
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
   this->fitnessQuorum  = fitnessQuorum;
   if (fitnessQuorum == -1)
   {
      behaviorStep = -1;
   }
   else
   {
      behaviorStep = 0;
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
NetworkIsomorphoGenesis::NetworkIsomorphoGenesis(vector<Behavior *>& behaviors, char *filename)
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
void NetworkIsomorphoGenesis::morph(int numGenerations, int numThreads)
#else
void NetworkIsomorphoGenesis::morph(int numGenerations)
#endif
{
   int i, c, g, n;
   int maxBehaviorStep;

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
   printf("Threads=%d\n", numThreads);
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
   printf("Generation=%d\n", generation);
   printf("Population:\n");
   printf("Member\tgeneration\tfitness\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
   }
   if (behaviorStep != -1)
   {
      printf("Behavior testing step=%d\n", behaviorStep);
   }
   for (g = 0; g < numGenerations; g++)
   {
      generation++;
      printf("Generation=%d\n", generation);
      mutate();
      prune();
      printf("Population:\n");
      printf("Member\tgeneration\tfitness\n");
      for (i = c = 0, n = (int)population.size(); i < n; i++)
      {
         printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
         if (population[i]->behaves)
         {
            c++;
         }
      }
      if (behaviorStep != -1)
      {
         printf("Behavior testing step=%d\n", behaviorStep);
         if ((c >= fitnessQuorum) && (behaviorStep < maxBehaviorStep))
         {
            behaviorStep++;
            evaluate();
            sort();
         }
      }
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
      pthread_mutex_destroy(&morphMutex);
      pthread_barrier_destroy(&morphBarrier);
      delete threads;
   }
#endif
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
   printf("Mutate:\n");
   printf("Member\tgeneration\tfitness\n");
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
      pthread_mutex_lock(&morphMutex);
#endif
      offspring[i] = (NetworkMorph *)((NetworkIsomorph *)population[j])->clone();
#ifdef THREADS
      pthread_mutex_unlock(&morphMutex);
#endif
      offspring[i]->tag++;
      ((NetworkIsomorph *)offspring[i])->mutate();
      offspring[i]->evaluate(behaviors, behaviorStep);
      printf("%d\t%d\t\t%f\n", i, offspring[i]->tag, offspring[i]->error);
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

   printf("Prune:\n");
   printf("Member\tgeneration\tfitness\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
      delete (NetworkIsomorph *)population[i];
      population[i] = offspring[j];
   }
   offspring.clear();
   sort();
}


// Load morph.
bool NetworkIsomorphoGenesis::load(char *filename)
{
   int  i, n;
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
   }
   FREAD_INT(&fitnessQuorum, fp);
   FREAD_INT(&behaviorStep, fp);
   FREAD_LONG(&randomSeed, fp);
   FREAD_INT(&generation, fp);
   return(true);
}


// Save morph.
bool NetworkIsomorphoGenesis::save(char *filename)
{
   int  i, n;
   FILE *fp;

   if ((fp = fopen(filename, "w")) == NULL)
   {
      fprintf(stderr, "Cannot save to file %s\n", filename);
      return(false);
   }
   randomizer->RAND_SAVE(fp);
   FWRITE_INT(&populationSize, fp);
   FWRITE_INT(&numOffspring, fp);
   n = (int)population.size();
   FWRITE_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      ((NetworkIsomorph *)population[i])->save(fp);
   }
   FWRITE_INT(&fitnessQuorum, fp);
   FWRITE_INT(&behaviorStep, fp);
   FWRITE_LONG(&randomSeed, fp);
   FWRITE_INT(&generation, fp);
   return(true);
}


// Print.
void NetworkIsomorphoGenesis::print()
{
   int i, n;

   printf("behaviors:\n");
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      behaviors[i]->print();
   }
   printf("populationSize=%d\n", populationSize);
   printf("numOffspring=%d\n", numOffspring);
   printf("fitnessQuorum=%d\n", fitnessQuorum);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      ((NetworkIsomorph *)population[i])->print();
   }
}
