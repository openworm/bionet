// Wriggle implementation.

#include "wriggle.hpp"
#include "behavior.hpp"

// Network morphgenesis parameters.
const int Wriggle::  POPULATION_SIZE                 = 500;
const int Wriggle::  NUM_OFFSPRING                   = 250;
const int Wriggle::  NUM_GENERATIONS                 = 250;
const float Wriggle::CROSSOVER_RATE                  = 0.9f;
const float Wriggle::MUTATION_RATE                   = 0.05f;
MutableParm Wriggle::SYNAPSE_WEIGHTS_PARM            = MutableParm(0.0f, 0.25f, 0.1f, 0.0f);
const float Wriggle::SYNAPSE_CROSSOVER_BOND_STRENGTH = 0.5f;
const int Wriggle::  SYNAPSE_OPTIMIZED_PATH_LENGTH   = 4;
const char *Wriggle::LOG_FILE = (char *)"morph.log";

// Constructors.
Wriggle::Wriggle(Network *homomorph,
                 int movements,
                 MutableParm& periodParm,
                 MutableParm& amplitudeParm,
                 MutableParm& phaseParm,
                 MutableParm& speedParm,
                 Random *randomizer, int tag)
{
   this->homomorph  = homomorph;
   this->movements  = movements;
   this->periodParm = periodParm;
   this->periodParm.initValue(randomizer);
   this->amplitudeParm = amplitudeParm;
   this->amplitudeParm.initValue(randomizer);
   this->phaseParm = phaseParm;
   this->phaseParm.initValue(randomizer);
   this->speedParm = speedParm;
   this->speedParm.initValue(randomizer);
   this->randomizer = randomizer;
   this->tag        = tag;
   network          = NULL;
   error            = 0.0f;
#ifdef THREADS
   numEvalThreads = 1;
#endif
}


Wriggle::Wriggle()
{
   homomorph  = NULL;
   network    = NULL;
   tag        = -1;
   error      = 0.0f;
   randomizer = NULL;
#ifdef THREADS
   numEvalThreads = 1;
#endif
}


Wriggle::Wriggle(FilePointer *fp, Network *homomorph, Random *randomizer)
{
   this->homomorph  = homomorph;
   this->randomizer = randomizer;
   load(fp);
#ifdef THREADS
   numEvalThreads = 1;
#endif
}


// Destructor.
Wriggle::~Wriggle()
{
}


// Evaluate wriggling behavior.
void Wriggle::evaluate()
{
   // Generate wriggling movements.
   createMovements();

   // Create wriggling behaviors.
   vector<Behavior *> behaviors;
   createBehaviors(behaviors);

   // Create network trained to produce behaviors.
   createNetwork(behaviors);

   for (int i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      delete behaviors[i];
   }
   behaviors.clear();
}


// Pi.
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

// Create wriggling movements.
void Wriggle::createMovements()
{
   int   i, j;
   float angle;

   vector<float> dorsalMagnitudes, ventralMagnitudes;
   float         M_PI_X2 = M_PI * 2.0f;

   float period    = periodParm.value;
   float amplitude = amplitudeParm.value;
   float phase     = phaseParm.value;
   float speed     = speedParm.value;
   float xdelta    = M_PI_X2 / (float)(BODY_JOINTS - 1);
   for (i = 0; i < movements; i++, phase += speed)
   {
      dorsalMagnitudes.clear();
      ventralMagnitudes.clear();
      for (j = 0, angle = 0.0f; j < BODY_JOINTS; j++, angle += xdelta)
      {
         float x = sin((M_PI_X2 / period) * (angle - phase)) * amplitude;
         if (x > 0.0f)
         {
            dorsalMagnitudes.push_back(x);
            ventralMagnitudes.push_back(0.0f);
         }
         else if (x < 0.0f)
         {
            dorsalMagnitudes.push_back(0.0f);
            ventralMagnitudes.push_back(-x);
         }
         else
         {
            dorsalMagnitudes.push_back(0.0f);
            ventralMagnitudes.push_back(0.0f);
         }
      }
      dorsalMotorSequence.push_back(dorsalMagnitudes);
      ventralMotorSequence.push_back(ventralMagnitudes);
   }
}


// Sensor indices.
const struct Wriggle::CellIndex Wriggle::SensorIndices[] =
{
   { (char *)"ALML", 8 },
   { (char *)"ALMR", 9 }
};

// Muscle indices.
const struct Wriggle::CellIndex Wriggle::MuscleIndices[] =
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

// Create wriggling behaviors.
void Wriggle::createBehaviors(vector<Behavior *>& behaviors)
{
   int   i, j, k;
   float m;

   vector<float> dorsalMagnitudes, ventralMagnitudes;
   Behavior      *behavior;

   // Touch-stimulated behavior.
   behavior = new Behavior();
   assert(behavior != NULL);
   behavior->sensorSequence.resize(movements);
   behavior->motorSequence.resize(movements);
   for (i = 0; i < movements; i++)
   {
      behavior->sensorSequence[i].resize(NUM_SENSORS, 0.0f);
      behavior->sensorSequence[i][SensorIndices[0].index] = 1.0f;
      behavior->sensorSequence[i][SensorIndices[1].index] = 1.0f;
      behavior->motorSequence[i].resize(NUM_MOTORS, 0.0f);
      dorsalMagnitudes  = dorsalMotorSequence[i];
      ventralMagnitudes = ventralMotorSequence[i];
      for (j = 0; j < BODY_JOINTS; j++)
      {
         k = j * 2;
         m = dorsalMagnitudes[j];
         behavior->motorSequence[i][MuscleIndices[k].index]      = m;
         behavior->motorSequence[i][MuscleIndices[k + 1].index]  = m;
         behavior->motorSequence[i][MuscleIndices[k + 24].index] = m;
         behavior->motorSequence[i][MuscleIndices[k + 25].index] = m;
         m = ventralMagnitudes[j];
         behavior->motorSequence[i][MuscleIndices[k + 48].index] = m;
         if (j < BODY_JOINTS - 1)
         {
            behavior->motorSequence[i][MuscleIndices[k + 49].index] = m;
         }
         behavior->motorSequence[i][MuscleIndices[k + 72].index] = m;
         behavior->motorSequence[i][MuscleIndices[k + 73].index] = m;
      }
   }
   behaviors.push_back(behavior);

   // No touch behavior.
   behavior = new Behavior();
   assert(behavior != NULL);
   behavior->sensorSequence.resize(movements);
   behavior->motorSequence.resize(movements);
   for (i = 0; i < movements; i++)
   {
      behavior->sensorSequence[i].resize(NUM_SENSORS, 0.0f);
      behavior->motorSequence[i].resize(NUM_MOTORS, 0.0f);
   }
   behaviors.push_back(behavior);
}


// Create network trained to behaviors.
void Wriggle::createNetwork(vector<Behavior *>& behaviors)
{
   int parentLongevity            = -1;
   int numGenerations             = -1;
   int behaveCutoff               = -1;
   int behaveQuorum               = -1;
   int behaveQuorumMaxGenerations = -1;

   vector<int> fitnessMotorList;

   NetworkHomomorphoGenesis *morphoGenesis =
      new NetworkHomomorphoGenesis(
         behaviors, homomorph,
         POPULATION_SIZE, NUM_OFFSPRING, parentLongevity,
         fitnessMotorList, behaveQuorum, behaveQuorumMaxGenerations,
         CROSSOVER_RATE, MUTATION_RATE, SYNAPSE_WEIGHTS_PARM,
         SYNAPSE_CROSSOVER_BOND_STRENGTH, SYNAPSE_OPTIMIZED_PATH_LENGTH,
         randomizer->RAND());
   assert(morphoGenesis != NULL);

#ifdef THREADS
   morphoGenesis->morph(NUM_GENERATIONS, numEvalThreads, behaveCutoff, (char *)LOG_FILE);
#else
   morphoGenesis->morph(NUM_GENERATIONS, behaveCutoff, LOG_FILE);
#endif
   network = morphoGenesis->population[0]->network->clone();
   error   = ((NetworkHomomorph *)(morphoGenesis->population[0]))->error;
   delete morphoGenesis;
#ifdef WIN32
   _unlink(LOG_FILE);
#else
   unlink(LOG_FILE);
#endif
}


// Mutate.
void Wriggle::mutate()
{
   periodParm.value = (float)randomizer->RAND_INTERVAL(
      periodParm.minimum, periodParm.maximum);
   amplitudeParm.value = (float)randomizer->RAND_INTERVAL(
      amplitudeParm.minimum, amplitudeParm.maximum);
   phaseParm.value = (float)randomizer->RAND_INTERVAL(
      phaseParm.minimum, phaseParm.maximum);
   speedParm.value = (float)randomizer->RAND_INTERVAL(
      speedParm.minimum, speedParm.maximum);
   evaluate();
}


// Optimize.
void Wriggle::optimize()
{
   int   n;
   float value, e;

   vector<vector<float> > valueRanges;
   vector<float>          valueRange;
   vector<vector<float> > permutations;
   vector<float>          permutation;

   // Hill-climb parameter value permutations.
   valueRange.clear();
   value = periodParm.value;
   valueRange.push_back(value);
   if (periodParm.maxDelta > 0.0f)
   {
      value = periodParm.value -
              (float)randomizer->RAND_INTERVAL(0.0f, periodParm.maxDelta);
      if (value < periodParm.minimum)
      {
         value = periodParm.minimum;
      }
   }
   valueRange.push_back(value);
   if (periodParm.maxDelta > 0.0f)
   {
      value = periodParm.value +
              (float)randomizer->RAND_INTERVAL(0.0f, periodParm.maxDelta);
      if (value > periodParm.maximum)
      {
         value = periodParm.maximum;
      }
   }
   valueRange.push_back(value);
   valueRanges.push_back(valueRange);

   valueRange.clear();
   value = amplitudeParm.value;
   valueRange.push_back(value);
   if (amplitudeParm.maxDelta > 0.0f)
   {
      value = amplitudeParm.value -
              (float)randomizer->RAND_INTERVAL(0.0f, amplitudeParm.maxDelta);
      if (value < amplitudeParm.minimum)
      {
         value = amplitudeParm.minimum;
      }
   }
   valueRange.push_back(value);
   if (amplitudeParm.maxDelta > 0.0f)
   {
      value = amplitudeParm.value +
              (float)randomizer->RAND_INTERVAL(0.0f, amplitudeParm.maxDelta);
      if (value > amplitudeParm.maximum)
      {
         value = amplitudeParm.maximum;
      }
   }
   valueRange.push_back(value);
   valueRanges.push_back(valueRange);

   valueRange.clear();
   value = phaseParm.value;
   valueRange.push_back(value);
   if (phaseParm.maxDelta > 0.0f)
   {
      value = phaseParm.value -
              (float)randomizer->RAND_INTERVAL(0.0f, phaseParm.maxDelta);
      if (value < phaseParm.minimum)
      {
         value = phaseParm.minimum;
      }
   }
   valueRange.push_back(value);
   if (phaseParm.maxDelta > 0.0f)
   {
      value = phaseParm.value +
              (float)randomizer->RAND_INTERVAL(0.0f, phaseParm.maxDelta);
      if (value > phaseParm.maximum)
      {
         value = phaseParm.maximum;
      }
   }
   valueRange.push_back(value);
   valueRanges.push_back(valueRange);

   valueRange.clear();
   value = speedParm.value;
   valueRange.push_back(value);
   if (speedParm.maxDelta > 0.0f)
   {
      value = speedParm.value -
              (float)randomizer->RAND_INTERVAL(0.0f, speedParm.maxDelta);
      if (value < speedParm.minimum)
      {
         value = speedParm.minimum;
      }
   }
   valueRange.push_back(value);
   if (speedParm.maxDelta > 0.0f)
   {
      value = speedParm.value +
              (float)randomizer->RAND_INTERVAL(0.0f, speedParm.maxDelta);
      if (value > speedParm.maximum)
      {
         value = speedParm.maximum;
      }
   }
   valueRange.push_back(value);
   valueRanges.push_back(valueRange);

   permutation.resize(4);
   permuteValues(valueRanges, permutations, permutation, 0, 3);
   n = randomizer->RAND_CHOICE((int)permutations.size() - 1) + 1;
   e = error;
   periodParm.value    = permutations[n][0];
   amplitudeParm.value = permutations[n][1];
   phaseParm.value     = permutations[n][2];
   speedParm.value     = permutations[n][3];
   evaluate();
   if (error > e)
   {
      error               = e;
      periodParm.value    = permutations[0][0];
      amplitudeParm.value = permutations[0][1];
      phaseParm.value     = permutations[0][2];
      speedParm.value     = permutations[0][3];
   }
}


// Permute values.
void Wriggle::permuteValues(vector<vector<float> >& valueRanges,
                            vector<vector<float> >& permutations,
                            vector<float>& permutation,
                            int level, int depth)
{
   for (int i = 0; i < 3; i++)
   {
      permutation[level] = valueRanges[level][i];
      if (level < depth)
      {
         permuteValues(valueRanges, permutations, permutation, level + 1, depth);
      }
      else
      {
         permutations.push_back(permutation);
      }
   }
}


// Clone.
Wriggle *Wriggle::clone()
{
   Wriggle *wriggle;

   wriggle = new Wriggle();
   assert(wriggle != NULL);
   wriggle->homomorph     = homomorph;
   wriggle->movements     = movements;
   wriggle->periodParm    = periodParm;
   wriggle->amplitudeParm = amplitudeParm;
   wriggle->phaseParm     = phaseParm;
   wriggle->speedParm     = speedParm;
   if (network != NULL)
   {
      wriggle->network = network->clone();
   }
   wriggle->tag        = tag;
   wriggle->error      = error;
   wriggle->randomizer = randomizer;
   return(wriggle);
}


// Load.
void Wriggle::load(FilePointer *fp)
{
   bool b;

   FREAD_INT(&tag, fp);
   FREAD_FLOAT(&error, fp);
   FREAD_INT(&movements, fp);
   periodParm.load(fp);
   amplitudeParm.load(fp);
   phaseParm.load(fp);
   speedParm.load(fp);
   FREAD_BOOL(&b, fp);
   if (b)
   {
      network = new Network(fp);
      assert(network != NULL);
   }
   else
   {
      network = NULL;
   }
}


// Save.
void Wriggle::save(FilePointer *fp)
{
   bool b;

   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
   FWRITE_INT(&movements, fp);
   periodParm.save(fp);
   amplitudeParm.save(fp);
   phaseParm.save(fp);
   speedParm.save(fp);
   if (network != NULL)
   {
      b = true;
      FWRITE_BOOL(&b, fp);
      network->save(fp);
   }
   else
   {
      b = false;
      FWRITE_BOOL(&b, fp);
   }
}


// Print.
void Wriggle::print(bool printNetwork)
{
   printf("tag=%d\n", tag);
   printf("error=%f\n", error);
   printf("movements=%d\n", movements);
   printf("periodParm:\n");
   periodParm.print();
   printf("amplitudeParm:\n");
   amplitudeParm.print();
   printf("phaseParm:\n");
   phaseParm.print();
   printf("speedParm:\n");
   speedParm.print();
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
}
