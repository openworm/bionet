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


NetworkHomomorph::NetworkHomomorph(FILE *fp,
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
   int     i, j, n;
   Synapse *synapse;

   i = randomNeuron(true);
   n = network->numNeurons;
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

   i = randomNeuron();
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
   n = network->numNeurons;
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
NetworkHomomorph *NetworkHomomorph::clone()
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
void NetworkHomomorph::load(FILE *fp)
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
void NetworkHomomorph::save(FILE *fp)
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


// Constructors.
LocomotionNetworkHomomorph::LocomotionNetworkHomomorph(int locomotionMovements, Network *homomorph,
                                                       MutableParm& synapseWeightsParm,
                                                       vector<vector<pair<int, int> > > *motorConnections,
                                                       Random *randomizer, int tag)
{
   this->locomotionMovements = locomotionMovements;
   this->synapseWeightsParm  = synapseWeightsParm;
   this->synapseWeightsParm.initValue(randomizer);
   this->motorConnections = motorConnections;
   this->randomizer       = randomizer;
   this->tag = tag;
   network   = homomorph->clone();
   motorErrors.resize(network->numMotors, false);
   fitness = 0.0f;
}


LocomotionNetworkHomomorph::LocomotionNetworkHomomorph(FILE *fp, int locomotionMovements,
                                                       vector<vector<pair<int, int> > > *motorConnections,
                                                       Random *randomizer)
{
   this->locomotionMovements = locomotionMovements;
   this->randomizer          = randomizer;
   network = NULL;
   load(fp);
   this->motorConnections = motorConnections;
}


// Harmonize synapses.
void LocomotionNetworkHomomorph::harmonize(int synapseOptimizedPathLength)
{
   int   i, j, k, n, s;
   bool  forward;
   float weight, e;

   vector<Synapse *>      synapses;
   vector<vector<float> > weightRanges;
   vector<float>          weightRange;
   vector<vector<float> > permutations;
   vector<float>          permutation;

   i = randomNeuron();
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
   n = network->numNeurons;
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
      evaluate();
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


// Sensor connectome indices.
const struct LocomotionNetworkHomomorph::CellIndex LocomotionNetworkHomomorph::sensorIndices[] =
{
   { (char *)"ALML", 8 },
   { (char *)"ALMR", 9 }
};

// Muscle connectome indices.
const struct LocomotionNetworkHomomorph::CellIndex LocomotionNetworkHomomorph::muscleIndices[] =
{
   { (char *)"MDL01", 48 },
   { (char *)"MDL02", 49 },
   { (char *)"MDL03", 53 },
   { (char *)"MDL04", 50 },
   { (char *)"MDL05",  0 },
   { (char *)"MDL06", 74 },
   { (char *)"MDL07", 16 },
   { (char *)"MDL08",  2 },
   { (char *)"MDL09", 20 },
   { (char *)"MDL10", 18 },
   { (char *)"MDL11", 24 },
   { (char *)"MDL12", 22 },
   { (char *)"MDL13", 28 },
   { (char *)"MDL14", 26 },
   { (char *)"MDL15", 32 },
   { (char *)"MDL16", 30 },
   { (char *)"MDL17", 36 },
   { (char *)"MDL18", 34 },
   { (char *)"MDL19",  6 },
   { (char *)"MDL20",  4 },
   { (char *)"MDL21",  8 },
   { (char *)"MDL22",  9 },
   { (char *)"MDL23", 10 },
   { (char *)"MDL24", 11 },
   { (char *)"MDR01", 51 },
   { (char *)"MDR02", 52 },
   { (char *)"MDR03", 55 },
   { (char *)"MDR04", 71 },
   { (char *)"MDR05",  1 },
   { (char *)"MDR06", 75 },
   { (char *)"MDR07", 17 },
   { (char *)"MDR08",  3 },
   { (char *)"MDR09", 21 },
   { (char *)"MDR10", 19 },
   { (char *)"MDR11", 25 },
   { (char *)"MDR12", 23 },
   { (char *)"MDR13", 29 },
   { (char *)"MDR14", 27 },
   { (char *)"MDR15", 33 },
   { (char *)"MDR16", 31 },
   { (char *)"MDR17", 37 },
   { (char *)"MDR18", 35 },
   { (char *)"MDR19",  7 },
   { (char *)"MDR20",  5 },
   { (char *)"MDR21", 12 },
   { (char *)"MDR22", 13 },
   { (char *)"MDR23", 14 },
   { (char *)"MDR24", 15 },
   { (char *)"MVL01", 54 },
   { (char *)"MVL02", 58 },
   { (char *)"MVL03", 44 },
   { (char *)"MVL04", 73 },
   { (char *)"MVL05", 65 },
   { (char *)"MVL06", 69 },
   { (char *)"MVL07", 66 },
   { (char *)"MVL08", 70 },
   { (char *)"MVL09", 62 },
   { (char *)"MVL10", 42 },
   { (char *)"MVL11", 39 },
   { (char *)"MVL12", 38 },
   { (char *)"MVL13", 63 },
   { (char *)"MVL14", 40 },
   { (char *)"MVL15", 80 },
   { (char *)"MVL16", 81 },
   { (char *)"MVL17", 86 },
   { (char *)"MVL18", 84 },
   { (char *)"MVL19", 88 },
   { (char *)"MVL20", 89 },
   { (char *)"MVL21", 92 },
   { (char *)"MVL22", 60 },
   { (char *)"MVL23", 93 },
   { (char *)"MVL24", -1 }, // missing
   { (char *)"MVR01", 56 },
   { (char *)"MVR02", 59 },
   { (char *)"MVR03", 57 },
   { (char *)"MVR04", 45 },
   { (char *)"MVR05", 64 },
   { (char *)"MVR06", 67 },
   { (char *)"MVR07", 72 },
   { (char *)"MVR08", 68 },
   { (char *)"MVR09", 76 },
   { (char *)"MVR10", 43 },
   { (char *)"MVR11", 78 },
   { (char *)"MVR12", 77 },
   { (char *)"MVR13", 79 },
   { (char *)"MVR14", 41 },
   { (char *)"MVR15", 82 },
   { (char *)"MVR16", 83 },
   { (char *)"MVR17", 87 },
   { (char *)"MVR18", 85 },
   { (char *)"MVR19", 90 },
   { (char *)"MVR20", 91 },
   { (char *)"MVR21", 61 },
   { (char *)"MVR22", 94 },
   { (char *)"MVR23", 95 },
   { (char *)"MVR24", 96 }
};

// Body joints.
const struct LocomotionNetworkHomomorph::BodyJoint LocomotionNetworkHomomorph::bodyJoints[] =
{
   {
      {  0,  1, 24, 25 },
      { 48, 49, 72, 73 }
   },
   {
      {  2,  3, 26, 27 },
      { 50, 51, 74, 75 }
   },
   {
      {  4,  5, 28, 29 },
      { 52, 53, 76, 77 }
   },
   {
      {  6,  7, 30, 31 },
      { 54, 55, 78, 79 }
   },
   {
      {  8,  9, 32, 33 },
      { 56, 57, 80, 81 }
   },
   {
      { 10, 11, 34, 35 },
      { 58, 59, 82, 83 }
   },
   {
      { 12, 13, 36, 37 },
      { 60, 61, 84, 85 }
   },
   {
      { 14, 15, 38, 39 },
      { 62, 63, 86, 87 }
   },
   {
      { 16, 17, 40, 41 },
      { 64, 65, 88, 89 }
   },
   {
      { 18, 19, 42, 43 },
      { 66, 67, 90, 91 }
   },
   {
      { 20, 21, 44, 45 },
      { 68, 69, 92, 93 }
   },
   {
      { 22, 23, 46, 47 },
      { 70, 71, 94, 95 }
   }
};

// Evaluate locomotion behavior fitness.
// Fitness is a function of the number and magnitude of opposing muscle forces
// when the light touch sensors are active.
void LocomotionNetworkHomomorph::evaluate()
{
   int i, j, k, m, n;

   vector<vector<float> > sensorSequence;
   Behavior               *behavior;
   float highForces[NUM_BODY_JOINTS];
   float forces[NUM_BODY_JOINTS];

   // Stimulate the touch sensors.
   sensorSequence.resize(locomotionMovements);
   n = network->numSensors;
   for (i = 0; i < locomotionMovements; i++)
   {
      sensorSequence[i].resize(n, 0.0f);
      sensorSequence[i][sensorIndices[0].index] = 1.0f;
      sensorSequence[i][sensorIndices[1].index] = 1.0f;
   }

   // Get muscle outputs.
   behavior = new Behavior(network, sensorSequence);
   assert(behavior != NULL);

   // Evaluate fitness.
   fitness = 0.0f;
   for (i = 0; i < NUM_BODY_JOINTS; i++)
   {
      highForces[i] = 0.0f;
   }
   for (i = 0; i < locomotionMovements; i++)
   {
      // Accumulate joint forces.
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         forces[j] = 0.0f;
         for (k = 0; k < 4; k++)
         {
            m = muscleIndices[bodyJoints[j].dorsalMuscles[k]].index;
            if (m != -1)
            {
               forces[j] += behavior->motorSequence[i][m];
            }
            m = muscleIndices[bodyJoints[j].ventralMuscles[k]].index;
            if (m != -1)
            {
               forces[j] -= behavior->motorSequence[i][m];
            }
         }
      }

      // Differentiate forces.
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         if (forces[j] > 0.0f)
         {
            if ((i == 0) || (highForces[j] < forces[j]))
            {
               if ((j == 0) || (forces[j - 1] < forces[j]))
               {
                  if ((j == NUM_BODY_JOINTS - 1) || (forces[j + 1] < forces[j]))
                  {
                     if (highForces[j] >= 0.0f)
                     {
                        fitness += forces[j] - highForces[j];
                     }
                     else
                     {
                        fitness += forces[j];
                     }
                  }
               }
            }
         }
         else if (forces[j] < 0.0f)
         {
            if ((i == 0) || (highForces[j] > forces[j]))
            {
               if ((j == 0) || (forces[j - 1] > forces[j]))
               {
                  if ((j == NUM_BODY_JOINTS - 1) || (forces[j + 1] > forces[j]))
                  {
                     if (highForces[j] <= 0.0f)
                     {
                        fitness += highForces[j] - forces[j];
                     }
                     else
                     {
                        fitness += -forces[j];
                     }
                  }
               }
            }
         }
      }
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         if (forces[j] > 0.0f)
         {
            if (forces[j] > highForces[j])
            {
               highForces[j] = forces[j];
            }
         }
         else if (forces[j] < 0.0f)
         {
            if (forces[j] < highForces[j])
            {
               highForces[j] = forces[j];
            }
         }
      }
   }
   delete behavior;

   // Remove touch stimulation.
   for (i = 0; i < locomotionMovements; i++)
   {
      sensorSequence[i][sensorIndices[0].index] = 0.0f;
      sensorSequence[i][sensorIndices[1].index] = 0.0f;
   }

   // Get muscle outputs.
   behavior = new Behavior(network, sensorSequence);
   assert(behavior != NULL);

   // Decrement fitness for movement.
   for (i = 0; i < locomotionMovements; i++)
   {
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         forces[j] = 0.0f;
         for (k = 0; k < 4; k++)
         {
            m = muscleIndices[bodyJoints[j].dorsalMuscles[k]].index;
            if (m != -1)
            {
               forces[j] += behavior->motorSequence[i][m];
            }
            m = muscleIndices[bodyJoints[j].ventralMuscles[k]].index;
            if (m != -1)
            {
               forces[j] -= behavior->motorSequence[i][m];
            }
         }
         fitness -= fabs(forces[j]);
      }
   }
   delete behavior;
}


// Clone.
LocomotionNetworkHomomorph *LocomotionNetworkHomomorph::clone()
{
   int i, n;
   LocomotionNetworkHomomorph *locomotionNetworkMorph;

   locomotionNetworkMorph = new LocomotionNetworkHomomorph(
      locomotionMovements, network, synapseWeightsParm,
      motorConnections, randomizer, tag);
   assert(locomotionNetworkMorph != NULL);
   locomotionNetworkMorph->fitness = fitness;
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      locomotionNetworkMorph->motorErrors[i] = motorErrors[i];
   }
   return(locomotionNetworkMorph);
}


// Load.
void LocomotionNetworkHomomorph::load(FILE *fp)
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
   FREAD_FLOAT(&fitness, fp);
   n = (int)network->numMotors;
   motorErrors.resize(n, false);
   for (i = 0; i < n; i++)
   {
      FREAD_BOOL(&b, fp);
      motorErrors[i] = b;
   }
   FREAD_INT(&offspringCount, fp);
}


// Save.
void LocomotionNetworkHomomorph::save(FILE *fp)
{
   int  i, n;
   bool b;

   synapseWeightsParm.save(fp);
   network->save(fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&fitness, fp);
   n = (int)motorErrors.size();
   for (i = 0; i < n; i++)
   {
      b = motorErrors[i];
      FWRITE_BOOL(&b, fp);
   }
   FWRITE_INT(&offspringCount, fp);
}


// Print.
void LocomotionNetworkHomomorph::print(bool printNetwork)
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
   printf("fitness=%f\n", fitness);
   printf("motorErrors: ");
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      if (motorErrors[i])
      {
         printf("%d ", i);
      }
   }
   printf("\n");
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
   int i, j, k, n;

   locomotionBehavior  = false;
   locomotionMovements = -1;
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


// Constructor for locomotion homomorphogenesis.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(int locomotionMovements, Network *homomorph,
                                                   int populationSize, int numOffspring, int parentLongevity,
                                                   float crossoverRate, float mutationRate,
                                                   MutableParm& synapseWeightsParm,
                                                   float synapseCrossoverBondStrength,
                                                   int synapseOptimizedPathLength,
                                                   RANDOM randomSeed)
{
   locomotionBehavior        = true;
   this->locomotionMovements = locomotionMovements;
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


void NetworkHomomorphoGenesis::init(Network *homomorph,
                                    int populationSize, int numOffspring, int parentLongevity,
                                    float crossoverRate, float mutationRate,
                                    MutableParm& synapseWeightsParm,
                                    float synapseCrossoverBondStrength,
                                    int synapseOptimizedPathLength,
                                    RANDOM randomSeed)
{
   int          i, j, k, n;
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
      if (locomotionBehavior)
      {
         networkMorph = (NetworkMorph *)new LocomotionNetworkHomomorph(
            locomotionMovements, homomorph, synapseWeightsParm,
            &motorConnections, randomizer);
      }
      else
      {
         networkMorph = (NetworkMorph *)new NetworkHomomorph(
            homomorph, synapseWeightsParm, &motorConnections, randomizer);
      }
      assert(networkMorph != NULL);
      network = networkMorph->network;
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
      population.push_back(networkMorph);
   }
}


// Load constructor.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(
   vector<Behavior *>& behaviors, char *filename)
{
   int i, j;

   locomotionBehavior  = false;
   locomotionMovements = -1;
   randomizer          = NULL;
   for (i = 0, j = (int)behaviors.size(); i < j; i++)
   {
      this->behaviors.push_back(behaviors[i]);
   }
   if (!load(filename))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      assert(false);
   }
   getMotorConnections();
}


// Load constructor for locomotion homomorphogenesis.
NetworkHomomorphoGenesis::NetworkHomomorphoGenesis(int locomotionMovements, char *filename)
{
   locomotionBehavior        = true;
   this->locomotionMovements = locomotionMovements;
   randomizer = NULL;
   if (!load(filename))
   {
      fprintf(stderr, "Cannot load morph from file %s\n", filename);
      assert(false);
   }
   getMotorConnections();
}


// Destructor.
NetworkHomomorphoGenesis::~NetworkHomomorphoGenesis()
{
   if (locomotionBehavior)
   {
      for (int i = 0, j = (int)population.size(); i < j; i++)
      {
         delete (LocomotionNetworkHomomorph *)population[i];
      }
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
         if (homomorph->synapses[i][index] != NULL)
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
                                     int behaveCutoff, char *logFile)
#else
void NetworkHomomorphoGenesis::morph(int numGenerations, int behaveCutoff,
                                     char *logFile)
#endif
{
   int i, g, n;
   int behaveCount;
   int maxBehaviorStep;
   LocomotionNetworkHomomorph *locomotionNetworkMorph;

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
   if (locomotionBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         locomotionNetworkMorph = (LocomotionNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness);
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
      harmonize();
      prune();
      fprintf(morphfp, "Population:\n");
      fprintf(morphfp, "Member\tgeneration\tfitness\n");
      if (locomotionBehavior)
      {
         for (i = 0, n = (int)population.size(); i < n; i++)
         {
            locomotionNetworkMorph = (LocomotionNetworkHomomorph *)population[i];
            fprintf(morphfp, "%d\t%d\t\t%f\n", i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness);
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
      morphoGenesis->harmonize(threadNum);
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
   LocomotionNetworkHomomorph *locomotionNetworkMorph;

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
      if (locomotionBehavior)
      {
         offspring[i] = (NetworkMorph *)((LocomotionNetworkHomomorph *)population[p1])->clone();
      }
      else
      {
         offspring[i] = (NetworkMorph *)((NetworkHomomorph *)population[p1])->clone();
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
         if (locomotionBehavior)
         {
            locomotionNetworkMorph = (LocomotionNetworkHomomorph *)offspring[i];
            locomotionNetworkMorph->evaluate();
            fprintf(morphfp, "%d\t%d\t\t%f\t%d %d\n",
                    i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness, p1, p2);
         }
         else
         {
            offspring[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
            fprintf(morphfp, "%d\t%d\t\t%f\t%d %d\n", i, offspring[i]->tag, offspring[i]->error, p1, p2);
         }
      }
      else   // No crossover.
      {
         offspring[i]->tag++;
         if (locomotionBehavior)
         {
            locomotionNetworkMorph = (LocomotionNetworkHomomorph *)offspring[i];
            fprintf(morphfp, "%d\t%d\t\t%f\t%d\n",
                    i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness, p1);
         }
         else
         {
            fprintf(morphfp, "%d\t%d\t\t%f\t%d\n", i, offspring[i]->tag, offspring[i]->error, p1);
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


// Mutate offspring.
void NetworkHomomorphoGenesis::mutate()
{
   fprintf(morphfp, "Mutate:\n");
   fprintf(morphfp, "Member\tgeneration\tfitness\n");
   mutate(0);
}


void NetworkHomomorphoGenesis::mutate(int threadNum)
{
   int                        i;
   NetworkHomomorph           *networkMorph;
   LocomotionNetworkHomomorph *locomotionNetworkMorph;

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
      if (locomotionBehavior)
      {
         locomotionNetworkMorph = (LocomotionNetworkHomomorph *)offspring[i];
         if (randomizer->RAND_CHANCE(mutationRate))
         {
            locomotionNetworkMorph->mutate();
            locomotionNetworkMorph->evaluate();
            fprintf(morphfp, "%d\t%d\t\t%f\n",
                    i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness);
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


// Harmonize offspring.
void NetworkHomomorphoGenesis::harmonize()
{
   fprintf(morphfp, "Harmonize:\n");
   fprintf(morphfp, "Member\tgeneration\tfitness\n");
   harmonize(0);
}


void NetworkHomomorphoGenesis::harmonize(int threadNum)
{
   int                        i;
   NetworkHomomorph           *networkMorph;
   LocomotionNetworkHomomorph *locomotionNetworkMorph;

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
      if (locomotionBehavior)
      {
         locomotionNetworkMorph = (LocomotionNetworkHomomorph *)offspring[i];
         locomotionNetworkMorph->harmonize(synapseOptimizedPathLength);
         fprintf(morphfp, "%d\t%d\t\t%f\n",
                 i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness);
      }
      else
      {
         networkMorph = (NetworkHomomorph *)offspring[i];
         networkMorph->harmonize(behaviors, fitnessMotorList,
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
   LocomotionNetworkHomomorph *locomotionNetworkMorph;

   fprintf(morphfp, "Prune:\n");
   fprintf(morphfp, "Member\tgeneration\tfitness\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      if (locomotionBehavior)
      {
         locomotionNetworkMorph = (LocomotionNetworkHomomorph *)population[i];
         fprintf(morphfp, "%d\t%d\t\t%f\n", i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness);
         delete (LocomotionNetworkHomomorph *)locomotionNetworkMorph;
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
      fprintf(morphfp, "Member\tgeneration\told fitness\tnew fitness\n");
      for (i = 0; i < n; i++)
      {
         if (population[i]->offspringCount > parentLongevity)
         {
            if (locomotionBehavior)
            {
               locomotionNetworkMorph = (LocomotionNetworkHomomorph *)population[i];
               fprintf(morphfp, "%d\t%d\t\t%f", i, locomotionNetworkMorph->tag, locomotionNetworkMorph->fitness);
               population[i] = (NetworkMorph *)new LocomotionNetworkHomomorph(
                  locomotionMovements, homomorph, networkMorph->synapseWeightsParm,
                  &motorConnections, randomizer);
               assert(population[i] != NULL);
               delete locomotionNetworkMorph;
               locomotionNetworkMorph = (LocomotionNetworkHomomorph *)population[i];
               locomotionNetworkMorph->evaluate();
               fprintf(morphfp, "\t%f\n", locomotionNetworkMorph->fitness);
            }
            else
            {
               fprintf(morphfp, "%d\t%d\t\t%f", i, population[i]->tag, population[i]->error);
               networkMorph  = (NetworkHomomorph *)population[i];
               population[i] = (NetworkMorph *)new NetworkHomomorph(
                  homomorph, networkMorph->synapseWeightsParm, &motorConnections, randomizer);
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

   if (locomotionBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((LocomotionNetworkHomomorph *)population[i])->evaluate();
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
   LocomotionNetworkHomomorph *locomotionNetworkMorph1, *locomotionNetworkMorph2;
   NetworkMorph               *networkMorph;

   if (locomotionBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         locomotionNetworkMorph1 = (LocomotionNetworkHomomorph *)population[i];
         for (j = i + 1; j < n; j++)
         {
            locomotionNetworkMorph2 = (LocomotionNetworkHomomorph *)population[j];
            if (locomotionNetworkMorph2->fitness > locomotionNetworkMorph1->fitness)
            {
               locomotionNetworkMorph1 = locomotionNetworkMorph2;
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
   FREAD_BOOL(&locomotionBehavior, fp);
   FREAD_INT(&locomotionMovements, fp);
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
   if (locomotionBehavior)
   {
      for (i = 0; i < n; i++)
      {
         LocomotionNetworkHomomorph *locomotionNetworkMorph =
            new LocomotionNetworkHomomorph(fp,
                                           locomotionMovements, &motorConnections, randomizer);
         assert(locomotionNetworkMorph != NULL);
         population.push_back((NetworkMorph *)locomotionNetworkMorph);
      }
   }
   else
   {
      for (i = 0; i < n; i++)
      {
         NetworkHomomorph *networkMorph =
            new NetworkHomomorph(fp, &motorConnections, randomizer);
         assert(networkMorph != NULL);
         population.push_back((NetworkMorph *)networkMorph);
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
   FWRITE_BOOL(&locomotionBehavior, fp);
   FWRITE_INT(&locomotionMovements, fp);
   FWRITE_FLOAT(&crossoverRate, fp);
   FWRITE_FLOAT(&mutationRate, fp);
   FWRITE_FLOAT(&synapseCrossoverBondStrength, fp);
   FWRITE_INT(&synapseOptimizedPathLength, fp);
   FWRITE_INT(&populationSize, fp);
   FWRITE_INT(&numOffspring, fp);
   n = (int)population.size();
   FWRITE_INT(&n, fp);
   if (locomotionBehavior)
   {
      for (i = 0; i < n; i++)
      {
         ((LocomotionNetworkHomomorph *)population[i])->save(fp);
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
   if (locomotionBehavior)
   {
      printf("locomotionMovements=%d\n", locomotionMovements);
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
   if (locomotionBehavior)
   {
      for (i = 0, n = (int)population.size(); i < n; i++)
      {
         ((LocomotionNetworkHomomorph *)population[i])->print(printNetwork);
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
