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
   homomorphic = false;
   error       = 0.0f;
   behaves     = false;
}


NetworkMorph::NetworkMorph(Network *homomorph, MutableParm& synapseWeightsParm,
                           Random *randomizer, int tag)
{
   int     i, j, n;
   Synapse *synapse;

   this->synapseWeightsParm = synapseWeightsParm;
   this->synapseWeightsParm.initValue(randomizer);
   this->randomizer = randomizer;
   this->tag        = tag;
   homomorphic      = true;
   network          = homomorph->clone();
   n = network->numNeurons;
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         synapse = network->synapses[i][j];
         if (synapse != NULL)
         {
            synapse->weight = (float)randomizer->RAND_INTERVAL(
               synapseWeightsParm.minimum, synapseWeightsParm.maximum);
         }
      }
   }
   error   = 0.0f;
   behaves = false;
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
void NetworkMorph::evaluate(vector<Behavior *>& behaviors, int maxStep)
{
   int      i, j, k, n, m, o, count, exceed;
   float    delta;
   Behavior *testBehavior;

   error   = 0.0f;
   behaves = true;
   count   = exceed = 0;
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      testBehavior = new Behavior(network, behaviors[i]->sensorSequence);
      assert(testBehavior != NULL);
      m = (int)testBehavior->motorSequence.size();
      if ((maxStep != -1) && ((maxStep + 1) < m))
      {
         m = maxStep + 1;
      }
      for (j = 0; j < m; j++)
      {
         for (k = 0, o = (int)testBehavior->motorSequence[j].size(); k < o; k++)
         {
            delta = fabs(behaviors[i]->motorSequence[j][k] - testBehavior->motorSequence[j][k]);
            if (delta > MAX_ERROR_TOLERANCE)
            {
               exceed++;
               behaves = false;
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
}


// Mutate.
void NetworkMorph::mutate()
{
   int     i, j, k, n;
   int     numExcitatory, newExcitatory, numInhibitory, newInhibitory, numSynapses;
   float   synapsePropensity, newSynapsePropensity, weight;
   Synapse *synapse;

   if (!homomorphic)
   {
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
   }

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
NetworkMorph *NetworkMorph::clone()
{
   NetworkMorph *networkMorph;

   if (homomorphic)
   {
      networkMorph = new NetworkMorph(
         network, synapseWeightsParm, randomizer, tag);
   }
   else
   {
      networkMorph = new NetworkMorph(
         excitatoryNeuronsParm, inhibitoryNeuronsParm,
         synapsePropensitiesParm, synapseWeightsParm,
         network->numSensors, network->numMotors, randomizer, tag);
   }
   assert(networkMorph != NULL);
   networkMorph->error   = error;
   networkMorph->behaves = behaves;
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
   FREAD_BOOL(&homomorphic, fp);
   FREAD_INT(&tag, fp);
   FREAD_FLOAT(&error, fp);
   FREAD_BOOL(&behaves, fp);
}


// Save.
void NetworkMorph::save(FILE *fp)
{
   excitatoryNeuronsParm.save(fp);
   inhibitoryNeuronsParm.save(fp);
   synapsePropensitiesParm.save(fp);
   synapseWeightsParm.save(fp);
   network->save(fp);
   FWRITE_BOOL(&homomorphic, fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
   FWRITE_BOOL(&behaves, fp);
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
   printf("homomorphic=");
   if (homomorphic)
   {
      printf("true\n");
   }
   else
   {
      printf("false\n");
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


// Isomorph constructor.
NetworkMorphoGenesis::NetworkMorphoGenesis(vector<Behavior *>& behaviors,
                                           int populationSize, int numMutants, int fitnessQuorum,
                                           MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                                           MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
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
   homomorph = NULL;
   assert(numMutants <= populationSize);
   this->populationSize = populationSize;
   this->numMutants     = numMutants;
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
      networkMorph = new NetworkMorph(excitatoryNeuronsParm, inhibitoryNeuronsParm,
                                      synapsePropensitiesParm, synapseWeightsParm,
                                      numSensors, numMotors, randomizer);
      assert(networkMorph != NULL);
      population.push_back(networkMorph);
   }
}


// Homomorph constructor.
NetworkMorphoGenesis::NetworkMorphoGenesis(vector<Behavior *>& behaviors, Network *homomorph,
                                           int populationSize, int numMutants, int numOffspring, int fitnessQuorum,
                                           MutableParm& synapseWeightsParm, RANDOM randomSeed)
{
   int          i, j;
   NetworkMorph *networkMorph;

   this->randomSeed = randomSeed;
   randomizer       = new Random(randomSeed);
   assert(randomizer != NULL);
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   this->homomorph = homomorph;
   assert((numMutants + numOffspring) <= populationSize);
   this->populationSize = populationSize;
   this->numMutants     = numMutants;
   this->numOffspring   = numOffspring;
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
   for (i = 0; i < populationSize; i++)
   {
      networkMorph = new NetworkMorph(homomorph, synapseWeightsParm, randomizer);
      assert(networkMorph != NULL);
      population.push_back(networkMorph);
   }
}


// Load constructor.
NetworkMorphoGenesis::NetworkMorphoGenesis(vector<Behavior *>& behaviors, char *filename)
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
void NetworkMorphoGenesis::morph(int numGenerations)
{
   int i, c, g, n;
   int maxBehaviorStep;

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
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      population[i]->evaluate(behaviors, behaviorStep);
   }
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
      mate();
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
            for (i = 0, n = (int)population.size(); i < n; i++)
            {
               population[i]->evaluate(behaviors, behaviorStep);
            }
            sort();
         }
      }
   }
}


// Mutate members.
void NetworkMorphoGenesis::mutate()
{
   int i, j;

   printf("Mutate:\n");
   printf("Member\tgeneration\tfitness\n");
   mutants.resize(numMutants);
   for (i = 0; i < numMutants; i++)
   {
      j          = randomizer->RAND_CHOICE(populationSize);
      mutants[i] = population[j]->clone();
      mutants[i]->tag++;
      mutants[i]->mutate();
      mutants[i]->evaluate(behaviors, behaviorStep);
      printf("%d\t%d\t\t%f\n", i, mutants[i]->tag, mutants[i]->error);
   }
}


// Mate homomorphic members.
void NetworkMorphoGenesis::mate()
{
   int     i, j, k, n, s;
   Network *parent, *child;

   if (homomorph != NULL)
   {
      printf("Mate:\n");
      assert(populationSize > 1);
      offspring.resize(numOffspring);
      for (i = 0; i < numOffspring; i++)
      {
         j = randomizer->RAND_CHOICE(populationSize);
         while ((k = randomizer->RAND_CHOICE(populationSize)) == j)
         {
         }
         offspring[i] = population[j]->clone();
         parent       = population[k]->network;
         child        = offspring[i]->network;
         for (n = 0; n < child->numNeurons; n++)
         {
            if (randomizer->RAND_BOOL())
            {
               child->neurons[n]->excitatory = parent->neurons[n]->excitatory;
               child->neurons[n]->function   = parent->neurons[n]->function;
               child->neurons[n]->bias       = parent->neurons[n]->bias;
               child->neurons[n]->activation = parent->neurons[n]->activation;
               for (s = 0; s < child->numNeurons; s++)
               {
                  if (child->synapses[n][s] != NULL)
                  {
                     child->synapses[n][s]->weight = parent->synapses[n][s]->weight;
                     child->synapses[n][s]->signal = parent->synapses[n][s]->signal;
                  }
               }
            }
         }
         offspring[i]->tag++;
         offspring[i]->evaluate(behaviors, behaviorStep);
         printf("%d\t%d\t\t%f\n", i, offspring[i]->tag, offspring[i]->error);
      }
   }
}


// Prune less fit members.
void NetworkMorphoGenesis::prune()
{
   int i, j, k, n;

   printf("Prune:\n");
   printf("Member\tgeneration\tfitness\n");
   for (n = (int)population.size(), i = n - (numMutants + numOffspring), j = k = 0; i < n; i++)
   {
      printf("%d\t%d\t\t%f\n", i, population[i]->tag, population[i]->error);
      delete population[i];
      if (j < numMutants)
      {
         population[i] = mutants[j];
         j++;
      }
      else
      {
         population[i] = offspring[k];
         k++;
      }
   }
   mutants.clear();
   offspring.clear();
   sort();
}


// Sort members by fitness.
void NetworkMorphoGenesis::sort()
{
   int          i, j, n;
   NetworkMorph *networkMorph;

   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      for (j = i + 1; j < n; j++)
      {
         if (population[i]->behaves)
         {
            if (population[j]->behaves && (population[i]->error > population[j]->error))
            {
               networkMorph  = population[i];
               population[i] = population[j];
               population[j] = networkMorph;
            }
         }
         else
         {
            if (population[j]->behaves || (population[i]->error > population[j]->error))
            {
               networkMorph  = population[i];
               population[i] = population[j];
               population[j] = networkMorph;
            }
         }
      }
   }
}


// Load morph.
bool NetworkMorphoGenesis::load(char *filename)
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
   FREAD_INT(&i, fp);
   if (i == 0)
   {
      homomorph = NULL;
   }
   else
   {
      homomorph = new Network(fp);
      assert(homomorph != NULL);
   }
   FREAD_INT(&populationSize, fp);
   FREAD_INT(&numMutants, fp);
   FREAD_INT(&numOffspring, fp);
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      delete population[i];
   }
   population.clear();
   mutants.clear();
   offspring.clear();
   FREAD_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      NetworkMorph *networkMorph = new NetworkMorph(fp, randomizer);
      assert(networkMorph != NULL);
      population.push_back(networkMorph);
   }
   FREAD_INT(&fitnessQuorum, fp);
   FREAD_INT(&behaviorStep, fp);
   FREAD_LONG(&randomSeed, fp);
   FREAD_INT(&generation, fp);
   return(true);
}


// Save morph.
bool NetworkMorphoGenesis::save(char *filename)
{
   int  i, n;
   FILE *fp;

   if ((fp = fopen(filename, "w")) == NULL)
   {
      fprintf(stderr, "Cannot save to file %s\n", filename);
      return(false);
   }
   randomizer->RAND_SAVE(fp);
   if (homomorph == NULL)
   {
      i = 0;
      FWRITE_INT(&i, fp);
   }
   else
   {
      i = 1;
      FWRITE_INT(&i, fp);
      homomorph->save(fp);
   }
   FWRITE_INT(&populationSize, fp);
   FWRITE_INT(&numMutants, fp);
   FWRITE_INT(&numOffspring, fp);
   n = (int)population.size();
   FWRITE_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      population[i]->save(fp);
   }
   FWRITE_INT(&fitnessQuorum, fp);
   FWRITE_INT(&behaviorStep, fp);
   FWRITE_LONG(&randomSeed, fp);
   FWRITE_INT(&generation, fp);
   return(true);
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
   printf("populationSize=%d\n", populationSize);
   printf("numMutants=%d\n", numMutants);
   printf("numOffspring=%d\n", numOffspring);
   printf("fitnessQuorum=%d\n", fitnessQuorum);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      population[i]->print();
   }
}
