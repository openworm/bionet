// Network isomorph implementation.

#include "networkIsomorph.hpp"

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
   motorErrors.resize(network->numMotors, false);
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
         numSynapses += (int)network->synapses[i][j].size();
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
         if (network->synapses[i][j].size() == 0)
         {
            if (((i < network->numSensors) || (i >= n)) && (j >= network->numSensors))
            {
               weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
               network->synapses[i][j].push_back(new Synapse(weight));
               assert(network->synapses[i][j][0] != NULL);
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
            numSynapses += (int)network->synapses[i][j].size();
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
               if (network->synapses[i][j].size() == 0)
               {
                  if (((i < network->numSensors) || (i >= n)) && (j >= network->numSensors))
                  {
                     if (randomizer->RAND_CHANCE(deltaPropensity))
                     {
                        weight = (float)randomizer->RAND_INTERVAL(
                           synapseWeightsParm.minimum, synapseWeightsParm.maximum);
                        network->synapses[i][j].push_back(new Synapse(weight));
                        assert(network->synapses[i][j][0] != NULL);
                        numSynapses++;
                     }
                  }
               }
            }
         }
      }
      else if (newNumSynapses < numSynapses)
      {
         int               deltaSynapses   = numSynapses - newNumSynapses;
         float             deltaPropensity = (float)deltaSynapses / (float)numSynapses;
         vector<Synapse *> synapses;
         for (i = 0, n = network->numNeurons; i < n; i++)
         {
            for (j = 0; j < n; j++)
            {
               for (k = 0; k < (int)network->synapses[i][j].size(); k++)
               {
                  synapse = network->synapses[i][j][k];
                  if (randomizer->RAND_CHANCE(deltaPropensity))
                  {
                     synapses.clear();
                     for (k = 0; k < (int)network->synapses[i][j].size(); k++)
                     {
                        if (network->synapses[i][j][k] != synapse)
                        {
                           synapses.push_back(network->synapses[i][j][k]);
                        }
                     }
                     network->synapses[i][j].clear();
                     for (k = 0; k < (int)synapses.size(); k++)
                     {
                        network->synapses[i][j].push_back(synapses[k]);
                     }
                     if ((network->synapses[i][j].size() > 0) || network->isConnected())
                     {
                        delete synapse;
                        numSynapses--;
                     }
                     else
                     {
                        network->synapses[i][j].push_back(synapse);
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
         for (k = 0; k < (int)network->synapses[i][j].size(); k++)
         {
            synapse = network->synapses[i][j][k];
            if (!behaves && randomizer->RAND_CHANCE(synapseWeightsParm.randomProbability))
            {
               synapse->setWeight((float)randomizer->RAND_INTERVAL(
                                     synapseWeightsParm.minimum, synapseWeightsParm.maximum));
            }
            else
            {
               if (synapseWeightsParm.maxDelta > 0.0f)
               {
                  if (randomizer->RAND_BOOL())
                  {
                     synapse->setWeight(synapse->weight + (float)randomizer->RAND_INTERVAL(
                                           0.0, synapseWeightsParm.maxDelta));
                     if (synapse->weight > synapseWeightsParm.maximum)
                     {
                        synapse->setWeight(synapseWeightsParm.maximum);
                     }
                  }
                  else
                  {
                     synapse->setWeight(synapse->weight - (float)randomizer->RAND_INTERVAL(
                                           0.0, synapseWeightsParm.maxDelta));
                     if (synapse->weight < synapseWeightsParm.minimum)
                     {
                        synapse->setWeight(synapseWeightsParm.minimum);
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
      for (j = 0; j < (int)network->synapses[index][i].size(); j++)
      {
         delete network->synapses[index][i][j];
      }
      network->synapses[index][i].clear();
   }
   for (i = index, j = n - 1; i < j; i++)
   {
      network->neurons[i] = network->neurons[i + 1];
      network->neurons[i]->index--;
      network->neurons[i + 1] = NULL;
      for (k = 0; k < n; k++)
      {
         network->synapses[i][k] = network->synapses[i + 1][k];
         network->synapses[i + 1][k].clear();
      }
   }
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < (int)network->synapses[i][index].size(); j++)
      {
         delete network->synapses[i][index][j];
      }
      network->synapses[i][index].clear();
      for (k = index, j = n - 1; k < j; k++)
      {
         network->synapses[i][k] = network->synapses[i][k + 1];
         network->synapses[i][k + 1].clear();
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
      network->synapses[i][j].clear();
   }
   for (i = 0; i < j; i++)
   {
      network->synapses[j][i].clear();
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
      if (((index < network->numSensors) || (index >= n)) && (i >= network->numSensors))
      {
         if (randomizer->RAND_CHANCE(synapsePropensity))
         {
            weight = (float)randomizer->RAND_INTERVAL(
               synapseWeightsParm.minimum, synapseWeightsParm.maximum);
            network->synapses[index][i].push_back(new Synapse(weight));
            assert(network->synapses[index][i][0] != NULL);
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
      if (network->synapses[i][index].size() == 0)
      {
         if (((i < network->numSensors) || (i >= n)) && (index >= network->numSensors))
         {
            if (randomizer->RAND_CHANCE(synapsePropensity))
            {
               weight = (float)randomizer->RAND_INTERVAL(
                  synapseWeightsParm.minimum, synapseWeightsParm.maximum);
               network->synapses[i][index].push_back(new Synapse(weight));
               assert(network->synapses[i][index][0] != NULL);
            }
         }
      }
   }
}


// Clone.
NetworkIsomorph *NetworkIsomorph::clone(int tag)
{
   int             i, n;
   NetworkIsomorph *networkMorph;

   networkMorph = new NetworkIsomorph(
      excitatoryNeuronsParm, inhibitoryNeuronsParm,
      synapsePropensitiesParm, synapseWeightsParm,
      network->numSensors, network->numMotors, randomizer, tag);
   assert(networkMorph != NULL);
   networkMorph->error = error;
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      networkMorph->motorErrors[i] = motorErrors[i];
   }
   networkMorph->behaves = behaves;
   delete networkMorph->network;
   networkMorph->network = network->clone();
   return(networkMorph);
}


// Load.
void NetworkIsomorph::load(FILE *fp)
{
   int  i, n;
   bool b;

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
   n = (int)network->numMotors;
   motorErrors.resize(n, false);
   for (i = 0; i < n; i++)
   {
      FREAD_BOOL(&b, fp);
      motorErrors[i] = b;
   }
   FREAD_BOOL(&behaves, fp);
}


// Save.
void NetworkIsomorph::save(FILE *fp)
{
   int  i, n;
   bool b;

   excitatoryNeuronsParm.save(fp);
   inhibitoryNeuronsParm.save(fp);
   synapsePropensitiesParm.save(fp);
   synapseWeightsParm.save(fp);
   network->save(fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
   n = (int)motorErrors.size();
   for (i = 0; i < n; i++)
   {
      b = motorErrors[i];
      FWRITE_BOOL(&b, fp);
   }
   FWRITE_BOOL(&behaves, fp);
}


// Print.
void NetworkIsomorph::print(bool printNetwork)
{
   int i, n;

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
   printf("motorErrors: ");
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      if (motorErrors[i])
      {
         printf("%d ", i);
      }
   }
   printf("\n");
   printf("behaves=");
   if (behaves)
   {
      printf("true\n");
   }
   else
   {
      printf("false\n");
   }
   printf("excitatoryNeuronsParm:\n");
   excitatoryNeuronsParm.print();
   printf("inhibitoryNeuronsParm:\n");
   inhibitoryNeuronsParm.print();
   printf("synapsePropensitiesParm:\n");
   synapsePropensitiesParm.print();
   printf("synapseWeightsParm:\n");
   synapseWeightsParm.print();
}
