// Network morph implementation.

#include "networkMorph.hpp"
#include <math.h>

MutableParm::MutableParm()
{
   value             = 0.0f;
   minimum           = 0.0f;
   maximum           = 0.0f;
   maxDelta          = 0.0f;
   randomProbability = -1.0f;
}


MutableParm::MutableParm(float minimum, float maximum, float maxDelta,
                         float randomProbability)
{
   this->minimum           = minimum;
   this->maximum           = maximum;
   this->maxDelta          = maxDelta;
   this->randomProbability = randomProbability;
}


void MutableParm::init(float minimum, float maximum, float maxDelta,
                       float randomProbability)
{
   this->minimum           = minimum;
   this->maximum           = maximum;
   this->maxDelta          = maxDelta;
   this->randomProbability = randomProbability;
}


void MutableParm::initValue(Random *randomizer)
{
   value = (float)randomizer->RAND_INTERVAL(minimum, maximum);
}


void MutableParm::setValue(float value)
{
   this->value = value;
   if (value > maximum)
   {
      value = maximum;
   }
   if (value < minimum)
   {
      value = minimum;
   }
}


void MutableParm::mutateValue(Random *randomizer)
{
   if (randomizer->RAND_CHANCE(randomProbability))
   {
      value = (float)randomizer->RAND_INTERVAL(minimum, maximum);
   }
   else
   {
      float delta = (float)randomizer->RAND_INTERVAL(0.0, maxDelta);
      if (randomizer->RAND_BOOL())
      {
         value += delta;
         if (value > maximum)
         {
            value = maximum;
         }
      }
      else
      {
         value -= delta;
         if (value < minimum)
         {
            value = minimum;
         }
      }
   }
}


void MutableParm::load(FILE *fp)
{
   FREAD_FLOAT(&value, fp);
   FREAD_FLOAT(&minimum, fp);
   FREAD_FLOAT(&maximum, fp);
   FREAD_FLOAT(&maxDelta, fp);
   FREAD_FLOAT(&randomProbability, fp);
}


void MutableParm::save(FILE *fp)
{
   FWRITE_FLOAT(&value, fp);
   FWRITE_FLOAT(&minimum, fp);
   FWRITE_FLOAT(&maximum, fp);
   FWRITE_FLOAT(&maxDelta, fp);
   FWRITE_FLOAT(&randomProbability, fp);
}


void MutableParm::print()
{
   printf("value=%f\n", value);
   printf("minimum=%f\n", minimum);
   printf("maximum=%f\n", maximum);
   printf("maxDelta=%f\n", maxDelta);
   printf("randomProbability=%f\n", randomProbability);
}


// Maximum tolerated motor output error.
const float NetworkMorph::MAX_ERROR_TOLERANCE = 0.05f;

// NetworkMorph constructors.
NetworkMorph::NetworkMorph(MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
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
                         inhibitorDensity, synapsePropensity, randomizer->RAND());
   assert(network != NULL);
   error = 0.0f;
}


NetworkMorph::NetworkMorph(MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                           MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                           Network *homomorph, Random *randomizer, int tag)
{
   int i, j, k, n;

   this->excitatoryNeuronsParm   = excitatoryNeuronsParm;
   this->inhibitoryNeuronsParm   = inhibitoryNeuronsParm;
   this->synapsePropensitiesParm = synapsePropensitiesParm;
   this->synapseWeightsParm      = synapseWeightsParm;
   this->randomizer = randomizer;
   this->tag        = tag;
   network          = homomorph->clone();
   int numExcitatory = 0;
   int numInhibitory = 0;
   int numSynapses   = 0;
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
            network->synapses[i][j]->weight = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
         }
      }
   }
   this->excitatoryNeuronsParm.setValue((float)numExcitatory);
   this->inhibitoryNeuronsParm.setValue((float)numInhibitory);
   n = network->numNeurons;
   float synapsePropensity = (float)numSynapses / (float)(n * n);
   this->synapsePropensitiesParm.setValue(synapsePropensity);
   error = 0.0f;
}


NetworkMorph::NetworkMorph(FILE *fp, Random *randomizer)
{
   network          = NULL;
   this->randomizer = randomizer;
   load(fp);
}


// Destructor.
NetworkMorph::~NetworkMorph()
{
   delete network;
}


// Evaluate behavior.
void NetworkMorph::evaluate(vector<Behavior *>& behaviors,
                            Network             *homomorph)
{
   int      i, j, k, n, m, o, count, exceed;
   float    delta;
   Behavior *testBehavior;

   error = 0.0f;
   count = exceed = 0;
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      testBehavior = new Behavior(network, behaviors[i]->sensorSequence);
      assert(testBehavior != NULL);
      for (j = 0, m = (int)testBehavior->motorSequence.size(); j < m; j++)
      {
         for (k = 0, o = (int)testBehavior->motorSequence[j].size(); k < o; k++)
         {
            delta = fabs(behaviors[i]->motorSequence[j][k] - testBehavior->motorSequence[j][k]);
            if (delta > MAX_ERROR_TOLERANCE)
            {
               exceed++;
            }
            error += delta;
            count++;
         }
      }
      delete testBehavior;
   }
   if (count > 0)
   {
      error /= (float)count;
   }
   error += (float)exceed;

   if (homomorph != NULL)
   {
      // TODO
   }
}


// Mutate.
void NetworkMorph::mutate()
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
   if (randomizer->RAND_CHANCE(excitatoryNeuronsParm.randomProbability))
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
   if (randomizer->RAND_CHANCE(inhibitoryNeuronsParm.randomProbability))
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
   if (randomizer->RAND_CHANCE(synapsePropensitiesParm.randomProbability))
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
//#ifdef NEVER
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
               weight = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
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
                        weight = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
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
            if (randomizer->RAND_CHANCE(synapseWeightsParm.randomProbability))
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
                     synapse->weight +=
                        (float)randomizer->RAND_INTERVAL(0.0, synapseWeightsParm.maxDelta);
                     if (synapse->weight > synapseWeightsParm.maximum)
                     {
                        synapse->weight = synapseWeightsParm.maximum;
                     }
                  }
                  else
                  {
                     synapse->weight -=
                        (float)randomizer->RAND_INTERVAL(0.0, synapseWeightsParm.maxDelta);
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
void NetworkMorph::deleteIndexedNeuron(int index)
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
void NetworkMorph::addIndexedNeuron(int index, bool excitatory)
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
               weight = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
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
               weight = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
               network->synapses[i][index] = new Synapse(weight);
               assert(network->synapses[i][index] != NULL);
            }
         }
      }
   }
}


// Clone.
NetworkMorph *NetworkMorph::clone()
{
   NetworkMorph *networkMorph = new NetworkMorph(
      excitatoryNeuronsParm, inhibitoryNeuronsParm,
      synapsePropensitiesParm, synapseWeightsParm,
      network->numSensors, network->numMotors, randomizer, tag);

   assert(networkMorph != NULL);
   delete networkMorph->network;
   networkMorph->network = network->clone();
   return(networkMorph);
}


// Load.
void NetworkMorph::load(FILE *fp)
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
}


// Save.
void NetworkMorph::save(FILE *fp)
{
   excitatoryNeuronsParm.save(fp);
   inhibitoryNeuronsParm.save(fp);
   synapsePropensitiesParm.save(fp);
   synapseWeightsParm.save(fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
}


// Print.
void NetworkMorph::print()
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
}


// Constructor.
NetworkMorphoGenesis::NetworkMorphoGenesis(vector<Behavior *>& behaviors,
                                           Network *homomorph, bool homomorphClones,
                                           MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                                           MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                                           int populationSize, int numOffspring, int numGenerations,
                                           RANDOM randomSeed)
{
   int          i, j, numSensors, numMotors;
   NetworkMorph *networkMorph;

   this->randomSeed = randomSeed;
   randomizer       = new Random(randomSeed);
   assert(randomizer != NULL);
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   this->homomorph       = homomorph;
   this->homomorphClones = homomorphClones;
   this->populationSize  = populationSize;
   this->numOffspring    = numOffspring;
   this->numGenerations  = numGenerations;
   if (homomorphClones)
   {
      assert(homomorph != NULL);
      for (i = 0; i < populationSize; i++)
      {
         networkMorph = new NetworkMorph(excitatoryNeuronsParm, inhibitoryNeuronsParm,
                                         synapsePropensitiesParm, synapseWeightsParm,
                                         homomorph, randomizer);
         assert(networkMorph != NULL);
         population.push_back(networkMorph);
      }
   }
   else
   {
      assert(behaviors.size() > 0);
      numSensors = (int)behaviors[0]->sensorSequence[0].size();
      numMotors  = (int)behaviors[0]->motorSequence[0].size();
      for (i = 0; i < populationSize; i++)
      {
         networkMorph = new NetworkMorph(excitatoryNeuronsParm, inhibitoryNeuronsParm,
                                         synapsePropensitiesParm, synapseWeightsParm,
                                         numSensors, numMotors, randomizer);
         assert(networkMorph != NULL);
         population.push_back(networkMorph);
      }
   }
}


// Destructor.
NetworkMorphoGenesis::~NetworkMorphoGenesis()
{
   for (int i = 0, j = (int)population.size(); i < j; i++)
   {
      delete population[i];
   }
   population.clear();
   delete randomizer;
}


// Morph networks.
void NetworkMorphoGenesis::morph()
{
   int i, n;

   printf("Generation=0\n");
   printf("Population:\n");
   printf("Member\tgeneration\tfitness\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      population[i]->evaluate(behaviors, homomorph);
      printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
   }
   for (int generation = 0; generation < numGenerations; generation++)
   {
      printf("Generation=%d\n", generation + 1);
      prune();
      mutate();
      printf("Population:\n");
      printf("Member\tgeneration\tfitness\n");
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
      }
   }
}


// Prune less fit members.
void NetworkMorphoGenesis::prune()
{
   int          i, j, n;
   NetworkMorph *networkMorph;

   printf("Prune:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      for (j = i + 1; j < n; j++)
      {
         if (population[i]->error > population[j]->error)
         {
            networkMorph  = population[i];
            population[i] = population[j];
            population[j] = networkMorph;
         }
      }
   }
   printf("Member\tgeneration\tfitness\n");
   for (i = n - numOffspring; i < n; i++)
   {
      printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
      delete population[i];
      population[i] = NULL;
   }
}


// Mutate members.
void NetworkMorphoGenesis::mutate()
{
   int i, j;

   printf("Mutate:\n");
   printf("Member\tgeneration\tfitness\n");
   for (i = 0, j = (int)population.size() - numOffspring; i < numOffspring; i++, j++)
   {
      population[j] = population[i]->clone();
      population[j]->tag++;
      population[j]->mutate();
      population[j]->evaluate(behaviors, homomorph);
      printf("%d\t%d\t\t%f\n", j, population[j]->tag, population[j]->error);
   }
}


// Save networks.
void NetworkMorphoGenesis::saveNetworks(char *filePrefix)
{
   char buf[BUFSIZ];

   for (int i = 0, n = (int)population.size(); i < n; i++)
   {
      sprintf(buf, "%s%d.txt", filePrefix, i);
      if (!population[i]->network->save(buf))
      {
         fprintf(stderr, "Cannot save network %d to file %s\n", i, buf);
      }
   }
}


// Print.
void NetworkMorphoGenesis::print()
{
   int i, n;

   printf("behaviors:\n");
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      behaviors[i]->print();
   }
   printf("homomorph:\n");
   if (homomorph != NULL)
   {
      homomorph->print();
   }
   else
   {
      printf("NULL\n");
   }
   if (homomorphClones)
   {
      printf("homomorphClones=true\n");
   }
   else
   {
      printf("homomorphClones=false\n");
   }
   printf("populationSize=%d\n", populationSize);
   printf("numOffspring=%d\n", numOffspring);
   printf("numGenerations=%d\n", numGenerations);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      population[i]->print();
   }
}
