// Network morphogenesis implementation.

#include "networkMorphoGenesis.hpp"

// Constructor.
NetworkMorphoGenesis::NetworkMorphoGenesis()
{
   populationSize              = 0;
   numOffspring                = 0;
   parentLongevity             = -1;
   behaveQuorum                = -1;
   behaveQuorumMaxGenerations  = -1;
   behaviorStep                = -1;
   behaveQuorumGenerationCount = -1;
   randomSeed = 4517;
   generation = 0;
   morphfp    = stdout;
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
