// Evolve C. elegans networks that produce wriggling sinusoidal behavior.

#include "evolveWrigglenet.hpp"

// Usage.
char *Usage[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"wrigglenet (new run)",
   (char *)"   -populationSize <number population members>",
   (char *)"   -numOffspring <number offspring per generation>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   -crossoverRate <probability>",
   (char *)"   -mutationRate <probability>",
   (char *)"   -movements <number of sinusoidal (wriggling) movements>",
   (char *)"   -periodParm <minimum> <maximum> <max delta>",
   (char *)"   -amplitudeParm <minimum> <maximum> <max delta>",
   (char *)"   -phaseParm <minimum> <maximum> <max delta>",
   (char *)"   -delayParm <minimum> <maximum> <max delta>",
   (char *)"   -saveRun <run file name> and/or -saveNetworks",
   (char *)"   [-randomSeed <random seed>]",
   (char *)"",
   (char *)"wrigglenet (resume run)",
   (char *)"   -loadRun <run file name>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   [-crossoverRate <probability> (defaults to loaded value)]",
   (char *)"   [-mutationRate <probability> (defaults to loaded value)]",
   (char *)"   -saveRun <run file name> and/or -saveNetworks",
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
   for (int i = 1; i < argc; i++)
   {
   }
   return(0);
}
