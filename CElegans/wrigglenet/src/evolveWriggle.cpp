// Evolve implementation.

#include "evolveWriggle.hpp"

// Constructors.
EvolveWriggle::EvolveWriggle(Network *homomorph,
                             int populationSize, int numOffspring,
                             float crossoverRate, float mutationRate,
                             int movements,
                             MutableParm& periodParm,
                             MutableParm& amplitudeParm,
                             MutableParm& phaseParm,
                             MutableParm& speedParm,
                             RANDOM randomSeed)
{
   Wriggle *wriggle;

   this->homomorph = homomorph;
   assert(numOffspring <= populationSize);
   this->populationSize = populationSize;
   this->numOffspring   = numOffspring;
   this->crossoverRate  = crossoverRate;
   this->mutationRate   = mutationRate;
   this->randomSeed     = randomSeed;
   randomizer           = new Random(randomSeed);
   assert(randomizer != NULL);
   generation = 0;
   logfp      = NULL;
   for (int i = 0; i < populationSize; i++)
   {
      wriggle = new Wriggle(homomorph,
                            movements,
                            periodParm,
                            amplitudeParm,
                            phaseParm,
                            speedParm,
                            randomizer);
      assert(wriggle != NULL);
      population.push_back(wriggle);
   }
}


EvolveWriggle::EvolveWriggle(char *filename)
{
   randomizer = NULL;
   logfp      = NULL;
   if (!load(filename))
   {
      fprintf(stderr, "Cannot load run from file %s\n", filename);
      assert(false);
   }
}


// Destructor.
EvolveWriggle::~EvolveWriggle()
{
   for (int i = 0, j = (int)population.size(); i < j; i++)
   {
      delete population[i];
   }
   population.clear();
   if (homomorph != NULL)
   {
      delete homomorph;
   }
   if (randomizer != NULL)
   {
      delete randomizer;
   }
}


// Evolve networks.
#ifdef THREADS
void EvolveWriggle::evolve(int numGenerations, int numEvalThreads, char *logFile)
#else
void EvolveWriggle::evolve(int numGenerations, char *logFile)
#endif
{
   int     i, j, g;
   Wriggle *wriggle;

   if (logFile != NULL)
   {
      startLog(logFile);
   }
   for (i = 0; i < populationSize; i++)
   {
      wriggle = population[i];
#ifdef THREADS
      wriggle->numEvalThreads = numEvalThreads;
#endif
      wriggle->evaluate();
   }
   sort();
   fprintf(logfp, "Generation=%d\n", generation);
   fprintf(logfp, "Population:\n");
   fprintf(logfp, "Member\tgeneration\terror\n");
   for (i = 0, j = (int)population.size(); i < j; i++)
   {
      wriggle = population[i];
      fprintf(logfp, "%d\t%d\t\t%f\n", i, wriggle->tag, wriggle->error);
   }
   fflush(logfp);
   for (g = 0; g < numGenerations; g++)
   {
      generation++;
      fprintf(logfp, "Generation=%d\n", generation);
      offspring.resize(numOffspring);
      mate();
      mutate();
      optimize();
      prune();
      fprintf(logfp, "Population:\n");
      fprintf(logfp, "Member\tgeneration\terror\n");
      for (i = 0, j = (int)population.size(); i < j; i++)
      {
         wriggle = population[i];
         fprintf(logfp, "%d\t%d\t\t%f\n", i, wriggle->tag, wriggle->error);
      }
      fflush(logfp);
   }

   if (logFile != NULL)
   {
      stopLog();
   }
}


// Mate.
void EvolveWriggle::mate()
{
   int     i, p1, p2;
   Wriggle *parent1, *parent2, *parent, *child;

   assert(populationSize > 1);
   fprintf(logfp, "Mate:\n");
   fprintf(logfp, "Member\tgeneration\terror\t\tparents\n");
   for (i = 0; i < numOffspring; i++)
   {
      p1           = randomizer->RAND_CHOICE(populationSize);
      parent1      = population[p1];
      offspring[i] = parent1->clone();

      // Mate parents?
      if (randomizer->RAND_CHANCE(crossoverRate))
      {
         while ((p2 = randomizer->RAND_CHOICE(populationSize)) == p1)
         {
         }
         parent2 = population[p2];
         if (population[p1]->tag > population[p2]->tag)
         {
            offspring[i]->tag = population[p1]->tag + 1;
         }
         else
         {
            offspring[i]->tag = population[p2]->tag + 1;
         }
         child = offspring[i];

         // Crossover parameters.
         if (randomizer->RAND_BOOL())
         {
            parent = parent1;
         }
         else
         {
            parent = parent2;
         }
         child->periodParm = parent->periodParm;
         if (randomizer->RAND_BOOL())
         {
            parent = parent1;
         }
         else
         {
            parent = parent2;
         }
         child->amplitudeParm = parent->amplitudeParm;
         if (randomizer->RAND_BOOL())
         {
            parent = parent1;
         }
         else
         {
            parent = parent2;
         }
         child->phaseParm = parent->phaseParm;
         if (randomizer->RAND_BOOL())
         {
            parent = parent1;
         }
         else
         {
            parent = parent2;
         }
         child->speedParm = parent->speedParm;
         child->evaluate();
         fprintf(logfp, "%d\t%d\t\t%f\t%d %d\n", i, child->tag, child->error, p1, p2);
      }
      else             // No crossover.
      {
         offspring[i]->tag++;
         fprintf(logfp, "%d\t%d\t\t%f\t%d\n", i, parent1->tag, parent1->error, p1);
      }
   }
}


// Mutate.
void EvolveWriggle::mutate()
{
   Wriggle *wriggle;

   fprintf(logfp, "Mutate:\n");
   fprintf(logfp, "Member\tgeneration\terror\n");
   for (int i = 0; i < numOffspring; i++)
   {
      wriggle = offspring[i];
      if (randomizer->RAND_CHANCE(mutationRate))
      {
         wriggle->mutate();
         fprintf(logfp, "%d\t%d\t\t%f\n", i, wriggle->tag, wriggle->error);
      }
   }
}


// Optimize.
void EvolveWriggle::optimize()
{
   Wriggle *wriggle;

   fprintf(logfp, "Optimize:\n");
   fprintf(logfp, "Member\tgeneration\terror\n");
   for (int i = 0; i < numOffspring; i++)
   {
      wriggle = offspring[i];
      wriggle->optimize();
      fprintf(logfp, "%d\t%d\t\t%f\n", i, wriggle->tag, wriggle->error);
   }
}


// Prune members.
void EvolveWriggle::prune()
{
   int     i, j, n;
   Wriggle *wriggle;

   fprintf(logfp, "Prune:\n");
   fprintf(logfp, "Member\tgeneration\terror\n");
   for (n = (int)population.size(), i = n - numOffspring, j = 0; i < n; i++, j++)
   {
      wriggle = population[i];
      fprintf(logfp, "%d\t%d\t\t%f\n", i, wriggle->tag, wriggle->error);
      delete wriggle;
      population[i] = offspring[j];
   }
   offspring.clear();
   sort();
}


// Sort members by error.
void EvolveWriggle::sort()
{
   int     i, j, n;
   Wriggle *wriggle1, *wriggle2;

   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      wriggle1 = population[i];
      for (j = i + 1; j < n; j++)
      {
         wriggle2 = population[j];
         if (wriggle2->error < wriggle1->error)
         {
            population[i] = wriggle2;
            population[j] = wriggle1;
            wriggle1      = wriggle2;
         }
      }
   }
}


// Load.
bool EvolveWriggle::load(char *filename)
{
   int     i, n;
   FILE    *fp;
   Wriggle *wriggle;

   if ((fp = fopen(filename, "r")) == NULL)
   {
      fprintf(stderr, "Cannot load from file %s\n", filename);
      return(false);
   }
   if (homomorph != NULL)
   {
      delete homomorph;
   }
   homomorph = new Network(fp);
   assert(homomorph != NULL);
   FREAD_INT(&populationSize, fp);
   FREAD_INT(&numOffspring, fp);
   FREAD_FLOAT(&crossoverRate, fp);
   FREAD_FLOAT(&mutationRate, fp);
   if (randomizer != NULL)
   {
      delete randomizer;
   }
   randomizer = new Random();
   assert(randomizer != NULL);
   randomizer->RAND_LOAD(fp);
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      delete population[i];
   }
   population.clear();
   offspring.clear();
   FREAD_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      wriggle = new Wriggle(fp, homomorph, randomizer);
      assert(wriggle != NULL);
      population.push_back(wriggle);
   }
   FREAD_LONG(&randomSeed, fp);
   FREAD_INT(&generation, fp);
   return(true);
}


// Save.
bool EvolveWriggle::save(char *filename)
{
   int  i, n;
   FILE *fp;

   if ((fp = fopen(filename, "w")) == NULL)
   {
      fprintf(stderr, "Cannot save to file %s\n", filename);
      return(false);
   }
   homomorph->save(fp);
   FWRITE_INT(&populationSize, fp);
   FWRITE_INT(&numOffspring, fp);
   FWRITE_FLOAT(&crossoverRate, fp);
   FWRITE_FLOAT(&mutationRate, fp);
   randomizer->RAND_SAVE(fp);
   n = (int)population.size();
   FWRITE_INT(&n, fp);
   for (i = 0; i < n; i++)
   {
      population[i]->save(fp);
   }
   FWRITE_LONG(&randomSeed, fp);
   FWRITE_INT(&generation, fp);
   return(true);
}


// Save networks.
bool EvolveWriggle::saveNetworks()
{
   char buf[BUFSIZ];

   bool result = true;

   for (int i = 0, n = (int)population.size(); i < n; i++)
   {
      sprintf(buf, "network_%d.txt", i);
      if (!population[i]->network->save(buf))
      {
         fprintf(stderr, "Cannot save network %d to file %s\n", i, buf);
         result = false;
      }
   }
   return(result);
}


// Print.
void EvolveWriggle::print(bool printNetwork)
{
   int i, n;

   printf("populationSize=%d\n", populationSize);
   printf("numOffspring=%d\n", numOffspring);
   printf("crossoverRate=%f\n", crossoverRate);
   printf("mutationRate=%f\n", mutationRate);
   printf("randomSeed=%lu\n", randomSeed);
   printf("Population:\n");
   for (i = 0, n = (int)population.size(); i < n; i++)
   {
      population[i]->print(printNetwork);
   }
}


// Start logging.
bool EvolveWriggle::startLog(char *logFile)
{
   if (logfp != stdout)
   {
      fclose(logfp);
   }
   if ((logfp = fopen(logFile, "w")) == NULL)
   {
      fprintf(stderr, "Cannot open log file %s\n", logFile);
      logfp = stdout;
      return(false);
   }
   else
   {
      return(true);
   }
}


// Stop logging.
void EvolveWriggle::stopLog()
{
   if (logfp != stdout)
   {
      fclose(logfp);
      logfp = stdout;
   }
}
