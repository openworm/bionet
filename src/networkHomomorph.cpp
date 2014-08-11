// Network homomorph implementation.

#include "networkHomomorph.hpp"

// Constructors.
NetworkHomomorph::NetworkHomomorph(Network *homomorph,
                                   MutableParm& synapseWeightsParm,
                                   vector<vector<pair<int, int> > > *motorConnections,
                                   Random *randomizer, int tag)
{
   this->synapseWeightsParm = synapseWeightsParm;
   this->synapseWeightsParm.initValue(randomizer);
   this->motorConnections = motorConnections;
   this->randomizer       = randomizer;
   this->tag = tag;
   network   = homomorph->clone();
   motorErrors.resize(network->numMotors, false);
}


NetworkHomomorph::NetworkHomomorph()
{
   randomizer = NULL;
   tag        = -1;
   network    = NULL;
}


NetworkHomomorph::NetworkHomomorph(FilePointer *fp,
                                   vector<vector<pair<int, int> > > *motorConnections,
                                   Random *randomizer)
{
   network          = NULL;
   this->randomizer = randomizer;
   load(fp);
   this->motorConnections = motorConnections;
}


// Destructor.
NetworkHomomorph::~NetworkHomomorph()
{
   delete network;
}


// Mutate synapses.
void NetworkHomomorph::mutate()
{
   int     i, j, k, n;
   float   weight;
   Synapse *synapse;

   i = randomNeuron(true);
   n = network->numNeurons;
   for (j = 0; j < n; j++)
   {
      weight = (float)randomizer->RAND_INTERVAL(
         synapseWeightsParm.minimum, synapseWeightsParm.maximum);
      for (k = 0; k < (int)network->synapses[i][j].size(); k++)
      {
         synapse = network->synapses[i][j][k];
         synapse->setWeight(weight);
      }
   }
}


// Optimize synapses.
void NetworkHomomorph::optimize(vector<Behavior *>& behaviors,
                                vector<bool>& fitnessMotorList,
                                int synapseOptimizedPathLength, int maxStep)
{
   int   i, j, k, n, p, q;
   float e;

   // Initialize optimization.
   vector<vector<Synapse *> > synapses;
   vector<vector<float> >     permutations;
   initOptimize(synapses, permutations, synapseOptimizedPathLength);

   // Hill-climb synapse weight permutations.
   n = 0;
   e = error;
   for (i = 1, j = (int)permutations.size(); i < j; i++)
   {
      for (k = 0; k < (int)synapses.size(); k++)
      {
         for (p = 0, q = (int)synapses[k].size(); p < q; p++)
         {
            synapses[k][p]->setWeight(permutations[i][k]);
         }
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
      for (p = 0, q = (int)synapses[k].size(); p < q; p++)
      {
         synapses[k][p]->setWeight(permutations[n][k]);
      }
   }
   error = e;
}


// Initialize synapse optimization.
void NetworkHomomorph::initOptimize(vector<vector<Synapse *> >& synapses,
                                    vector<vector<float> >&     permutations,
                                    int                         synapseOptimizedPathLength)
{
   int   i, j, k, n, p, q, s;
   bool  forward;
   float weight;

   vector<pair<int, int> > visited;
   Synapse                 *synapse;
   vector<Synapse *>       chemSynapses, elecSynapses;
   vector<vector<float> >  weightRanges;
   vector<float>           weightRange;
   vector<float>           permutation;

   synapses.clear();
   permutations.clear();

   // Randomly select starting neuron with synapse.
   i = randomNeuron();
   n = network->numNeurons;
   for (s = 0; s < n; s++)
   {
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
      if (forward)
      {
         for (j = 0; j < n; j++)
         {
            if (network->synapses[i][j].size() > 0)
            {
               break;
            }
         }
         if (j < n)
         {
            break;
         }
      }
      else
      {
         for (j = 0; j < n; j++)
         {
            if (network->synapses[j][i].size() > 0)
            {
               break;
            }
         }
         if (j < n)
         {
            break;
         }
      }
      i++;
      i = (i % n);
   }
   if (s == n)
   {
      return;
   }

   // Select synapse path.
   for (j = 0; j < synapseOptimizedPathLength; j++)
   {
      k = randomizer->RAND_CHOICE(n);
      for (s = 0; s < n; s++)
      {
         if (forward)
         {
            if (network->synapses[i][k].size() > 0)
            {
               for (p = 0, q = (int)visited.size(); p < q; p++)
               {
                  if ((visited[p].first == i) && (visited[p].second == k))
                  {
                     break;
                  }
               }
               if (p == q)
               {
                  visited.push_back(pair<int, int>(i, k));
                  chemSynapses.clear();
                  elecSynapses.clear();
                  for (p = 0, q = (int)network->synapses[i][k].size(); p < q; p++)
                  {
                     synapse = network->synapses[i][k][p];
                     if (synapse->type == Synapse::CHEMICAL)
                     {
                        chemSynapses.push_back(synapse);
                     }
                     else
                     {
                        elecSynapses.push_back(synapse);
                     }
                  }
                  if (chemSynapses.size() > 0)
                  {
                     synapses.push_back(chemSynapses);
                  }
                  if (elecSynapses.size() > 0)
                  {
                     synapses.push_back(elecSynapses);
                  }
                  if ((chemSynapses.size() > 0) || (elecSynapses.size() > 0))
                  {
                     i = k;
                     break;
                  }
               }
            }
         }
         else
         {
            if (network->synapses[k][i].size() > 0)
            {
               for (p = 0, q = (int)visited.size(); p < q; p++)
               {
                  if ((visited[p].first == k) && (visited[p].second == i))
                  {
                     break;
                  }
               }
               if (p == q)
               {
                  visited.push_back(pair<int, int>(k, i));
                  chemSynapses.clear();
                  elecSynapses.clear();
                  for (p = 0, q = (int)network->synapses[k][i].size(); p < q; p++)
                  {
                     synapse = network->synapses[k][i][p];
                     if (synapse->type == Synapse::CHEMICAL)
                     {
                        chemSynapses.push_back(synapse);
                     }
                     else
                     {
                        elecSynapses.push_back(synapse);
                     }
                  }
                  if (chemSynapses.size() > 0)
                  {
                     synapses.push_back(chemSynapses);
                  }
                  if (elecSynapses.size() > 0)
                  {
                     synapses.push_back(elecSynapses);
                  }
                  if ((chemSynapses.size() > 0) || (elecSynapses.size() > 0))
                  {
                     i = k;
                     break;
                  }
               }
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

   if (synapses.size() == 0)
   {
      return;
   }

   // Compose synapse weight permutations.
   for (i = 0, j = (int)synapses.size(); i < j; i++)
   {
      weightRange.clear();
      weight = synapses[i][0]->weight;
      weightRange.push_back(weight);
      if (synapseWeightsParm.maxDelta > 0.0f)
      {
         weight = synapses[i][0]->weight -
                  (float)randomizer->RAND_INTERVAL(0.0f, synapseWeightsParm.maxDelta);
         if (weight < synapseWeightsParm.minimum)
         {
            weight = synapseWeightsParm.minimum;
         }
      }
      weightRange.push_back(weight);
      if (synapseWeightsParm.maxDelta > 0.0f)
      {
         weight = synapses[i][0]->weight +
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
}


// Select random neuron.
int NetworkHomomorph::randomNeuron(bool nonMotor)
{
   int  i, j, k, m, n, s, t;
   bool f;

   vector<int> indices;

   s = network->numSensors;
   t = s + network->numMotors;
   for (i = 0, n = (int)motorErrors.size(), f = false; i < n; i++)
   {
      if (motorErrors[i])
      {
         for (j = 0, k = (int)(*motorConnections)[i].size(); j < k; j++)
         {
            m = (*motorConnections)[i][j].first;
            indices.push_back(m);
            if ((m < s) || (m >= t))
            {
               f = true;
            }
         }
      }
   }
   if (nonMotor && !f)
   {
      nonMotor = false;
   }
   while (true)
   {
      n = (int)indices.size();
      if ((n > 0) && randomizer->RAND_BOOL())
      {
         n = indices[randomizer->RAND_CHOICE(n)];
      }
      else
      {
         n = randomizer->RAND_CHOICE(network->numNeurons);
      }
      if (nonMotor)
      {
         if ((n < s) || (n >= t))
         {
            break;
         }
      }
      else
      {
         break;
      }
   }
   return(n);
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
NetworkHomomorph *NetworkHomomorph::clone(int tag)
{
   int              i, n;
   NetworkHomomorph *networkMorph;

   networkMorph = new NetworkHomomorph(
      network, synapseWeightsParm,
      motorConnections, randomizer, tag);
   assert(networkMorph != NULL);
   networkMorph->error = error;
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      networkMorph->motorErrors[i] = motorErrors[i];
   }
   networkMorph->behaves = behaves;
   return(networkMorph);
}


// Load.
void NetworkHomomorph::load(FilePointer *fp)
{
   int  i, n;
   bool b;

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
   FREAD_INT(&offspringCount, fp);
}


// Save.
void NetworkHomomorph::save(FilePointer *fp)
{
   int  i, n;
   bool b;

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
   FWRITE_INT(&offspringCount, fp);
}


// Print.
void NetworkHomomorph::print(bool printNetwork)
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
   printf("offspringCount=%d\n", offspringCount);
   printf("synapseWeightsParm:\n");
   synapseWeightsParm.print();
}
