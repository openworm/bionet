// Undulation network homomorph implementation.

#include "undulationNetworkHomomorph.hpp"
#ifdef FFT_UNDULATION_EVAL
#include <numeric>
#endif

// Constructors.
UndulationNetworkHomomorph::UndulationNetworkHomomorph(int undulationMovements, Network *homomorph,
                                                       MutableParm& synapseWeightsParm,
                                                       vector<vector<pair<int, int> > > *motorConnections,
                                                       Random *randomizer, int tag)
{
   this->undulationMovements = undulationMovements;
   this->synapseWeightsParm  = synapseWeightsParm;
   this->synapseWeightsParm.initValue(randomizer);
   this->motorConnections = motorConnections;
   this->randomizer       = randomizer;
   this->tag = tag;
   network   = homomorph->clone();
   motorErrors.resize(network->numMotors, false);
   fitness = 0.0f;
#ifdef FFT_UNDULATION_EVAL
   activations      = (double *)fftw_malloc(sizeof(double) * (undulationMovements * NUM_BODY_JOINTS));
   bodyActivations  = (double *)fftw_malloc(sizeof(double) * NUM_BODY_JOINTS);
   bodyDFT          = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (NUM_BODY_JOINTS / 2 + 1));
   bodyPlan         = fftw_plan_dft_r2c_1d(NUM_BODY_JOINTS, bodyActivations, bodyDFT, FFTW_MEASURE);
   jointActivations = (double *)fftw_malloc(sizeof(double) * undulationMovements);
   jointDFT         = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (undulationMovements / 2 + 1));
   jointPlan        = fftw_plan_dft_r2c_1d(undulationMovements, jointActivations, jointDFT, FFTW_MEASURE);
#endif
}


UndulationNetworkHomomorph::UndulationNetworkHomomorph(FilePointer *fp, int undulationMovements,
                                                       vector<vector<pair<int, int> > > *motorConnections,
                                                       Random *randomizer)
{
   this->undulationMovements = undulationMovements;
   this->randomizer          = randomizer;
   network = NULL;
   load(fp);
   this->motorConnections = motorConnections;
#ifdef FFT_UNDULATION_EVAL
   activations      = (double *)fftw_malloc(sizeof(double) * (undulationMovements * NUM_BODY_JOINTS));
   bodyActivations  = (double *)fftw_malloc(sizeof(double) * NUM_BODY_JOINTS);
   bodyDFT          = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (NUM_BODY_JOINTS / 2 + 1));
   bodyPlan         = fftw_plan_dft_r2c_1d(NUM_BODY_JOINTS, bodyActivations, bodyDFT, FFTW_MEASURE);
   jointActivations = (double *)fftw_malloc(sizeof(double) * undulationMovements);
   jointDFT         = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (undulationMovements / 2 + 1));
   jointPlan        = fftw_plan_dft_r2c_1d(undulationMovements, jointActivations, jointDFT, FFTW_MEASURE);
#endif
}


UndulationNetworkHomomorph::~UndulationNetworkHomomorph()
{
#ifdef FFT_UNDULATION_EVAL
   fftw_destroy_plan(bodyPlan);
   fftw_free(bodyActivations);
   fftw_free(bodyDFT);
   fftw_destroy_plan(jointPlan);
   fftw_free(jointActivations);
   fftw_free(jointDFT);
   fftw_free(activations);
#endif
}


// Optimize synapses.
void UndulationNetworkHomomorph::optimize(int synapseOptimizedPathLength)
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
      evaluate();
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


// Sensor connectome indices.
const struct UndulationNetworkHomomorph::CellIndex UndulationNetworkHomomorph::sensorIndices[] =
{
   { (char *)"ALML", 8 },
   { (char *)"ALMR", 9 }
};

// Muscle connectome indices.
const struct UndulationNetworkHomomorph::CellIndex UndulationNetworkHomomorph::muscleIndices[] =
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
const struct UndulationNetworkHomomorph::BodyJoint UndulationNetworkHomomorph::bodyJoints[] =
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

#ifdef FFT_UNDULATION_EVAL

/*
 * Evaluate undulation behavior fitness.
 *
 * The body is modeled as a linear span of equally spaced articulated joints, flexed
 * on a common plane by opposing muscle groups that are activated by dorsal and ventral
 * motor neurons.
 *
 * Undulation is a sinusoidal muscle activation pattern of the joints that changes
 * over time. This is represented by a two dimensional array: space X time.
 *
 * The Fourier Transform of the muscle activation array produces frequency spectrums
 * of sinusoidal patterns in space and time.
 *
 * Fitness is a function of the dispersion of these frequency spectrums when the light
 * touch sensor neurons are stimulated. A less dispersed spectrum represents a more distinct
 * undulation which is a fitter undulation behavior.
 */
void UndulationNetworkHomomorph::evaluate()
{
   int i, j, k, m, n;

   vector<vector<float> > sensorSequence;
   Behavior               *behavior;
   vector<double>         values;
   double                 sum, mean, max, bodyMax, jointMax, bodySum, jointSum;
   double                 r, im, mag;

   // Stimulate the touch sensors.
   sensorSequence.resize(undulationMovements);
   n = network->numSensors;
   for (i = 0; i < undulationMovements; i++)
   {
      sensorSequence[i].resize(n, 0.0f);
      sensorSequence[i][sensorIndices[0].index] = 1.0f;
      sensorSequence[i][sensorIndices[1].index] = 1.0f;
   }

   // Get muscle activations.
   behavior = new Behavior(network, sensorSequence);
   assert(behavior != NULL);
   for (i = 0; i < undulationMovements; i++)
   {
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         activations[j + (NUM_BODY_JOINTS * i)] = 0.0;
         for (k = 0; k < 4; k++)
         {
            m = muscleIndices[bodyJoints[j].dorsalMuscles[k]].index;
            if (m != -1)
            {
               activations[j + (NUM_BODY_JOINTS * i)] += behavior->motorSequence[i][m];
            }
            m = muscleIndices[bodyJoints[j].ventralMuscles[k]].index;
            if (m != -1)
            {
               activations[j + (NUM_BODY_JOINTS * i)] -= behavior->motorSequence[i][m];
            }
         }
      }
   }

   // Get spectrums of body shapes for each movement.
   fitness = 0.0f;
   bodyMax = 0.0;
   for (i = 0, k = NUM_BODY_JOINTS / 2 + 1; i < undulationMovements; i++)
   {
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         bodyActivations[j] = activations[j + (NUM_BODY_JOINTS * i)];
      }
      fftw_execute(bodyPlan);
      values.clear();
      max = 0.0;
      for (j = 0; j < k; j++)
      {
         r   = bodyDFT[j][0];
         im  = bodyDFT[j][1];
         mag = sqrt((r * r) + (im * im));
         if (j > 0)
         {
            values.push_back(mag);
            if ((j == 1) || (mag > max))
            {
               max = mag;
            }
         }
      }
      if (values.size() > 0)
      {
         sum      = std::accumulate(values.begin(), values.end(), 0.0);
         mean     = sum / values.size();
         bodyMax += (max - mean);
      }
   }
   if (undulationMovements > 0)
   {
      bodyMax /= (double)undulationMovements;
   }

   // Get spectrums of joints moving in time.
   jointMax = 0.0;
   for (i = 0, k = undulationMovements / 2 + 1; i < NUM_BODY_JOINTS; i++)
   {
      for (j = 0; j < undulationMovements; j++)
      {
         jointActivations[j] = activations[i + (NUM_BODY_JOINTS * j)];
      }
      fftw_execute(jointPlan);
      values.clear();
      max = 0.0;
      for (j = 0; j < k; j++)
      {
         r   = jointDFT[j][0];
         im  = jointDFT[j][1];
         mag = sqrt((r * r) + (im * im));
         if (j > 0)
         {
            values.push_back(mag);
            if ((j == 1) || (mag > max))
            {
               max = mag;
            }
         }
      }
      if (values.size() > 0)
      {
         sum       = std::accumulate(values.begin(), values.end(), 0.0);
         mean      = sum / values.size();
         jointMax += (max - mean);
      }
   }
   delete behavior;
   if (NUM_BODY_JOINTS > 0)
   {
      jointMax /= (double)NUM_BODY_JOINTS;
   }

   // Fitness is product of body and joint spectrum dispersions.
   fitness = (float)(bodyMax * jointMax);

   // Remove touch stimulation.
   for (i = 0; i < undulationMovements; i++)
   {
      sensorSequence[i][sensorIndices[0].index] = 0.0f;
      sensorSequence[i][sensorIndices[1].index] = 0.0f;
   }

   // Get muscle activations.
   behavior = new Behavior(network, sensorSequence);
   assert(behavior != NULL);
   for (i = 0; i < undulationMovements; i++)
   {
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         activations[j + (NUM_BODY_JOINTS * i)] = 0.0;
         for (k = 0; k < 4; k++)
         {
            m = muscleIndices[bodyJoints[j].dorsalMuscles[k]].index;
            if (m != -1)
            {
               activations[j + (NUM_BODY_JOINTS * i)] += behavior->motorSequence[i][m];
            }
            m = muscleIndices[bodyJoints[j].ventralMuscles[k]].index;
            if (m != -1)
            {
               activations[j + (NUM_BODY_JOINTS * i)] -= behavior->motorSequence[i][m];
            }
         }
      }
   }

   // Less energy is fitter.
   bodySum = 0.0;
   for (i = 0, k = NUM_BODY_JOINTS / 2 + 1; i < undulationMovements; i++)
   {
      for (j = 0; j < NUM_BODY_JOINTS; j++)
      {
         bodyActivations[j] = activations[j + (NUM_BODY_JOINTS * i)];
      }
      fftw_execute(bodyPlan);
      values.clear();
      for (j = 0; j < k; j++)
      {
         r   = bodyDFT[j][0];
         im  = bodyDFT[j][1];
         mag = sqrt((r * r) + (im * im));
         values.push_back(mag);
      }
      if (values.size() > 0)
      {
         bodySum += std::accumulate(values.begin(), values.end(), 0.0);
      }
   }
   if (undulationMovements > 0)
   {
      bodySum /= (double)undulationMovements;
   }

   jointSum = 0.0;
   for (i = 0, k = undulationMovements / 2 + 1; i < NUM_BODY_JOINTS; i++)
   {
      for (j = 0; j < undulationMovements; j++)
      {
         jointActivations[j] = activations[i + (NUM_BODY_JOINTS * j)];
      }
      fftw_execute(jointPlan);
      values.clear();
      for (j = 0; j < k; j++)
      {
         r   = jointDFT[j][0];
         im  = jointDFT[j][1];
         mag = sqrt((r * r) + (im * im));
         values.push_back(mag);
      }
      if (values.size() > 0)
      {
         jointSum += std::accumulate(values.begin(), values.end(), 0.0);
      }
   }
   delete behavior;
   if (NUM_BODY_JOINTS > 0)
   {
      jointSum /= (double)NUM_BODY_JOINTS;
   }
   fitness -= (float)((bodySum + jointSum) / 2.0);
}


#else

// Evaluate undulation behavior fitness.
// Fitness is a function of the number and magnitude of opposing muscle forces
// when the light touch sensors are active.
void UndulationNetworkHomomorph::evaluate()
{
   int i, j, k, m, n;

   vector<vector<float> > sensorSequence;
   Behavior               *behavior;
   float highForces[NUM_BODY_JOINTS];
   float forces[NUM_BODY_JOINTS];

   // Stimulate the touch sensors.
   sensorSequence.resize(undulationMovements);
   n = network->numSensors;
   for (i = 0; i < undulationMovements; i++)
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
   for (i = 0; i < undulationMovements; i++)
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
   for (i = 0; i < undulationMovements; i++)
   {
      sensorSequence[i][sensorIndices[0].index] = 0.0f;
      sensorSequence[i][sensorIndices[1].index] = 0.0f;
   }

   // Get muscle outputs.
   behavior = new Behavior(network, sensorSequence);
   assert(behavior != NULL);

   // Decrement fitness for movement.
   for (i = 0; i < undulationMovements; i++)
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


#endif


// Clone.
UndulationNetworkHomomorph *UndulationNetworkHomomorph::clone(int tag)
{
   int i, n;
   UndulationNetworkHomomorph *undulationNetworkMorph;

   undulationNetworkMorph = new UndulationNetworkHomomorph(
      undulationMovements, network, synapseWeightsParm,
      motorConnections, randomizer, tag);
   assert(undulationNetworkMorph != NULL);
   undulationNetworkMorph->fitness = fitness;
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      undulationNetworkMorph->motorErrors[i] = motorErrors[i];
   }
   return(undulationNetworkMorph);
}


// Load.
void UndulationNetworkHomomorph::load(FilePointer *fp)
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
void UndulationNetworkHomomorph::save(FilePointer *fp)
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
void UndulationNetworkHomomorph::print(bool printNetwork)
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
