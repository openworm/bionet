// Evolve C. elegans networks that produce wriggling sinusoidal behavior.

#include "evolveWriggle.hpp"
#ifndef WIN32
#include <sys/sysinfo.h>
#endif

// Usage.
char *Usage[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"wrigglenet (new run)",
   (char *)"   -loadNetwork <C. elegans network file name>",
   (char *)"   -populationSize <number population members>",
   (char *)"   -numOffspring <number offspring per generation>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   -crossoverRate <probability>",
   (char *)"   -mutationRate <probability>",
   (char *)"   -movements <number of sinusoidal (wriggling) movements>",
   (char *)"   -periodParm <minimum> <maximum> <max delta>",
   (char *)"   -amplitudeParm <minimum> <maximum> <max delta>",
   (char *)"   -phaseParm <minimum> <maximum> <max delta>",
   (char *)"   -speedParm <minimum> <maximum> <max delta>",
   (char *)"   -saveRun <run file name> and/or -saveNetworks",
   (char *)"   [-randomSeed <random seed>]",
   (char *)"   [-log <log file name>]",
#ifdef THREADS
   (char *)"   [-numThreads <number of threads>(defaults to system capacity)]",
#endif
   (char *)"",
   (char *)"wrigglenet (resume run)",
   (char *)"   -loadRun <run file name>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   [-crossoverRate <probability> (defaults to loaded value)]",
   (char *)"   [-mutationRate <probability> (defaults to loaded value)]",
   (char *)"   -saveRun <run file name> and/or -saveNetworks",
   (char *)"   [-log <log file name>]",
#ifdef THREADS
   (char *)"   [-numThreads <number of threads>(defaults to system capacity)]",
#endif
   NULL
};

void printUsage()
{
   for (int i = 0; Usage[i] != NULL; i++)
   {
      fprintf(stderr, (char *)"%s\n", Usage[i]);
   }
}


void printUsageError(char *error)
{
   fprintf(stderr, (char *)"Error: %s\n", error);
   printUsage();
}


int main(int argc, char *argv[])
{
   char          *networkLoadFile = NULL;
   int           populationSize   = -1;
   int           numOffspring     = -1;
   int           numGenerations   = -1;
   float         crossoverRate    = -1.0f;
   float         mutationRate     = -1.0f;
   int           movements        = -1;
   MutableParm   periodParm;
   MutableParm   amplitudeParm;
   MutableParm   phaseParm;
   MutableParm   speedParm;
   char          *runSaveFile   = NULL;
   bool          saveNetworks   = false;
   char          *runLoadFile   = NULL;
   RANDOM        randomSeed     = Network::DEFAULT_RANDOM_SEED;
   bool          gotRandomSeed  = false;
   char          *logFile       = NULL;
   EvolveWriggle *evolveWriggle = NULL;

#ifdef THREADS
   int numThreads = -1;
#endif

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-loadNetwork") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         networkLoadFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-populationSize") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         populationSize = atoi(argv[i]);
         if (populationSize < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numOffspring") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numOffspring = atoi(argv[i]);
         if (numOffspring < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numGenerations") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numGenerations = atoi(argv[i]);
         if (numGenerations < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if ((strcmp(argv[i], "-crossoverRate") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         crossoverRate = (float)atof(argv[i]);
         if ((crossoverRate < 0.0f) || (crossoverRate > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if ((strcmp(argv[i], "-mutationRate") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         mutationRate = (float)atof(argv[i]);
         if ((mutationRate < 0.0f) || (mutationRate > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-movements") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         movements = atoi(argv[i]);
         if (movements < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if ((strcmp(argv[i], "-periodParm") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         float min = (float)atof(argv[i]);
         if (min < 0.0f)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         float max = (float)atof(argv[i]);
         if ((max < 0.0f) || (max < min))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         float maxDelta = (float)atof(argv[i]);
         if (maxDelta < 0.0f)
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         periodParm.init(min, max, maxDelta, 0.0f);
         continue;
      }
      if ((strcmp(argv[i], "-amplitudeParm") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         float min = (float)atof(argv[i]);
         if (min < 0.0f)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         float max = (float)atof(argv[i]);
         if ((max < 0.0f) || (max < min))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         float maxDelta = (float)atof(argv[i]);
         if (maxDelta < 0.0f)
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         amplitudeParm.init(min, max, maxDelta, 0.0f);
         continue;
      }
      if ((strcmp(argv[i], "-phaseParm") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         float min = (float)atof(argv[i]);
         if (min < 0.0f)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         float max = (float)atof(argv[i]);
         if ((max < 0.0f) || (max < min))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         float maxDelta = (float)atof(argv[i]);
         if (maxDelta < 0.0f)
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         phaseParm.init(min, max, maxDelta, 0.0f);
         continue;
      }
      if ((strcmp(argv[i], "-speedParm") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         float min = (float)atof(argv[i]);
         if (min < 0.0f)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         float max = (float)atof(argv[i]);
         if ((max < 0.0f) || (max < min))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         float maxDelta = (float)atof(argv[i]);
         if (maxDelta < 0.0f)
         {
            printUsageError(argv[i - 3]);
            return(1);
         }
         speedParm.init(min, max, maxDelta, 0.0f);
         continue;
      }
      if (strcmp(argv[i], "-saveRun") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         runSaveFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-saveNetworks") == 0)
      {
         saveNetworks = true;
         continue;
      }
      if (strcmp(argv[i], "-loadRun") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         runLoadFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-randomSeed") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         gotRandomSeed = true;
         randomSeed    = atoi(argv[i]);
         continue;
      }
      if (strcmp(argv[i], "-log") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         logFile = argv[i];
         continue;
      }
#ifdef THREADS
      if (strcmp(argv[i], "-numThreads") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numThreads = atoi(argv[i]);
         continue;
      }
#endif
      if ((strcmp(argv[i], "-h") == 0) ||
          (strcmp(argv[i], "-help") == 0) ||
          (strcmp(argv[i], "--h") == 0) ||
          (strcmp(argv[i], "--help") == 0) ||
          (strcmp(argv[i], "-?") == 0) ||
          (strcmp(argv[i], "--?") == 0))
      {
         printUsage();
         return(0);
      }
      printUsageError((char *)"invalid option");
      return(1);
   }

#ifdef THREADS
   if (numThreads == -1)
   {
#ifdef WIN32
      SYSTEM_INFO info;
      GetSystemInfo(&info);
      numThreads = info.dwNumberOfProcessors;
#else
      numThreads = get_nprocs();
#endif
      if (numThreads <= 0)
      {
         numThreads = 1;
      }
   }
#endif

   if (runLoadFile == NULL)
   {
      if ((networkLoadFile == NULL) ||
          ((runSaveFile == NULL) && !saveNetworks) ||
          (populationSize < 0) ||
          (numOffspring < 0) ||
          (numGenerations < 0) ||
          (crossoverRate < 0.0f) ||
          (mutationRate < 0.0f) ||
          (movements < 0) ||
          (periodParm.randomProbability < 0.0f) ||
          (amplitudeParm.randomProbability < 0.0f) ||
          (phaseParm.randomProbability < 0.0f) ||
          (speedParm.randomProbability < 0.0f))
      {
         printUsageError((char *)"missing required option");
         return(1);
      }
      if (numOffspring > populationSize)
      {
         printUsageError((char *)"numOffspring > populationSize");
         return(1);
      }
      if ((numOffspring > 0) && (populationSize < 2))
      {
         printUsageError((char *)"numOffspring > 0 && populationSize < 2");
         return(1);
      }
      Network *homomorph = new Network(networkLoadFile);
      assert(homomorph != NULL);
      evolveWriggle =
         new EvolveWriggle(homomorph,
                           populationSize, numOffspring,
                           crossoverRate, mutationRate, movements,
                           periodParm, amplitudeParm, phaseParm, speedParm,
                           randomSeed);
      assert(evolveWriggle != NULL);
   }
   else   // Resume run.
   {
      if (((runSaveFile == NULL) && !saveNetworks) ||
          (numGenerations < 0) ||
          (gotRandomSeed))
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
      evolveWriggle = new EvolveWriggle(runLoadFile);
      assert(evolveWriggle != NULL);
      if (crossoverRate != -1.0f)
      {
         evolveWriggle->crossoverRate = crossoverRate;
      }
      if (mutationRate != -1.0f)
      {
         evolveWriggle->mutationRate = mutationRate;
      }
   }
#ifdef THREADS
   evolveWriggle->evolve(numGenerations, numThreads, logFile);
#else
   evolveWriggle->evolve(numGenerations, logFile);
#endif
   if (runSaveFile != NULL)
   {
      if (!evolveWriggle->save(runSaveFile))
      {
         fprintf(stderr, "Cannot save run to file %s\n", runSaveFile);
         return(1);
      }
   }
   if (saveNetworks)
   {
      if (!evolveWriggle->saveNetworks())
      {
         fprintf(stderr, "Cannot save networks\n");
         return(1);
      }
   }
   delete evolveWriggle;
   return(0);
}
