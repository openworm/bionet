// Network morph implementation.

#include "networkMorph.hpp"
#include <math.h>

// Maximum tolerated motor output error.
const float NetworkMorph::MAX_ERROR_TOLERANCE = 0.05f;

// Constructor.
NetworkMorph::NetworkMorph()
{
   network        = NULL;
   tag            = 0;
   error          = 0.0f;
   behaves        = false;
   offspringCount = 0;
}


// Evaluate behavior.
void NetworkMorph::evaluate(vector<Behavior *>& behaviors, int maxStep)
{
   vector<bool> fitnessMotorList;
   evaluate(behaviors, fitnessMotorList, maxStep);
}


void NetworkMorph::evaluate(vector<Behavior *>& behaviors,
                            vector<bool>& fitnessMotorList, int maxStep)
{
   int      i, j, k, n, m, o, count, exceed;
   bool     motorFitness;
   float    delta;
   Behavior *testBehavior;

   error   = 0.0f;
   behaves = true;
   count   = exceed = 0;
   if (fitnessMotorList.size() > 0)
   {
      motorFitness = true;
   }
   else
   {
      motorFitness = false;
   }
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
            if (motorFitness && !fitnessMotorList[k])
            {
               continue;
            }
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


// Constructor.
NetworkMorphoGenesis::NetworkMorphoGenesis()
{
   populationSize  = 0;
   numOffspring    = 0;
   parentLongevity = -1;
   fitnessQuorum   = -1;
   behaviorStep    = -1;
   randomSeed      = 4517;
   generation      = 0;
   morphfp         = stdout;
}


// Evaluate behavior.
void NetworkMorphoGenesis::evaluate()
{
   int i, n;

   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      population[i]->evaluate(behaviors, fitnessMotorList, behaviorStep);
   }
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


// Start morph logging to file.
bool NetworkMorphoGenesis::startMorphLog(char *logFile)
{
   if (morphfp != stdout)
   {
      fclose(morphfp);
   }
   if ((morphfp = fopen(logFile, "w")) == NULL)
   {
      fprintf(stderr, "Cannot open morph log file %s\n", logFile);
      morphfp = stdout;
      return(false);
   }
   else
   {
      return(true);
   }
}


// Stop morph logging.
void NetworkMorphoGenesis::stopMorphLog()
{
   if (morphfp != stdout)
   {
      fclose(morphfp);
      morphfp = stdout;
   }
}
