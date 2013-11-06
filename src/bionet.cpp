// bionet: a "biological" (irregular) neural network.

#include "networkHomomorph.hpp"
#include "networkIsomorph.hpp"
#ifndef WIN32
#include <sys/sysinfo.h>
#endif

// Usage.
char *Usage[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"Create network:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -createNetwork",
   (char *)"   -numNeurons <number of neurons>",
   (char *)"   -numSensors <number of sensors>",
   (char *)"   -numMotors <number of motors>",
   (char *)"   [-inhibitorDensity <inhibitory neuron density [0.0,1.0]>]",
   (char *)"   [-synapsePropensity <synapse generation propensity (0.0,1.0]>]",
   (char *)"   [-minSynapseWeight <minimum synapse weight (defaults to 0.0)>]",
   (char *)"   [-maxSynapseWeight <maximum synapse weight (defaults to 1.0)>]",
   (char *)"   [-randomSeed <random seed>]",
   (char *)"   [-saveNetwork <network file name>]",
   (char *)"   [-graphNetwork <graph file name ('dot' format)>]",
   (char *)"",
   (char *)"Print network:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -printNetwork",
   (char *)"   [-connectivity (also print connectivity)]",
   (char *)"   -loadNetwork <network file name>",
   (char *)"",
   (char *)"Graph network:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -graphNetwork <graph file name ('dot' format)>",
   (char *)"   [-title <graph title>]",
   (char *)"   -loadNetwork <network file name>",
   (char *)"",
   (char *)"Create network behaviors:",
   (char *)"",
   (char *)"bionet",
   (char *)"  -createNetworkBehaviors",
   (char *)"  -loadNetwork <network file name>",
   (char *)"  -behaviorLengths <sensory-motor sequence length list (blank separator)>",
   (char *)"  [-randomSeed <random seed>]",
   (char *)"  [-saveBehaviors <behaviors file name>]",
   (char *)"",
   (char *)"Test network behaviors:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -testNetworkBehaviors",
   (char *)"   -loadNetwork <network file name>",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"   [-motorDeltaTolerance <minimum motor delta to print>]",
   (char *)"",
   (char *)"Print network behaviors:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -printNetworkBehaviors",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"",
   (char *)"Create homomorphic networks:",
   (char *)"",
   (char *)"bionet (new morph)",
   (char *)"   -createhomomorphicNetworks",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"   -loadNetwork <homomorph network file name>",
   (char *)"   -populationSize <number population members>",
   (char *)"   -numOffspring <number offspring per generation>",
   (char *)"   [-parentLongevity <parent dies after this many offspring>]",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   [-behaveCutoff <stop evolution when this many members behave>]",
   (char *)"   [-behaveQuorum <behaving member quorum required to advance behavior testing to next sensory-motor step>",
   (char *)"      (defaults to immediate testing of entire behavior sequences)",
   (char *)"      [<maximum generations before advancing without a quorum>]]",
   (char *)"   [-fitnessMotorList <list of motor outputs evaluated for fitness (0-n, comma-separated)>",
   (char *)"      (defaults to fitness evaluation of all motor outputs)]",
   (char *)"   -crossoverRate <probability>",
   (char *)"   -mutationRate <probability>",
   (char *)"   -synapseWeights <minimum> <maximum> <max delta>",
   (char *)"   -synapseCrossoverBondStrength <probability of connected neurons crossing over together>",
   (char *)"   -synapseOptimizedPathLength <synapse path length optimized as a group>",
   (char *)"   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default=\"network_\")>]",
   (char *)"   [-randomSeed <random seed>]",
   (char *)"   [-logMorph <log file name>]",
#ifdef THREADS
   (char *)"   [-numThreads <number of threads> (defaults to system capacity)]",
#endif
   (char *)"",
   (char *)"bionet (resume morph)",
   (char *)"   -createHomomorphicNetworks",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"   -loadMorph <morph file name>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   [-behaveCutoff <stop evolution when this many members behave>]",
   (char *)"   [-fitnessMotorList <list of motor outputs evaluated for fitness (0-n, comma-separated)>",
   (char *)"      (defaults to loaded list)]",
   (char *)"   [-crossoverRate <probability> (defaults to loaded value)]",
   (char *)"   [-mutationRate <probability> (defaults to loaded value)]",
   (char *)"   [-synapseBondStrength <probability of connected neurons crossing over together> (defaults to loaded value)]",
   (char *)"   [-synapseChainSize <number of chained synapses optimized as a group> (defaults to loaded value)]",
   (char *)"   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default=\"network_\")>]",
   (char *)"   [-logMorph <morph log file name> (instead of standard output)]",
#ifdef THREADS
   (char *)"   [-numThreads <number of threads> (defaults to system capacity)]",
#endif
   (char *)"",
   (char *)"Merge homomorphic network populations:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -mergeHomomorphicNetworks",
   (char *)"   -loadMorph <morph file name> <morph file name>",
   (char *)"   -saveMorph <morph log file name> (instead of standard output)",
   (char *)"   [-randomSeed <random seed>]",
   (char *)"",
   (char *)"Create isomorphic networks:",
   (char *)"",
   (char *)"bionet (new morph)",
   (char *)"   -createIsomorphicNetworks",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"   -populationSize <number population members>",
   (char *)"   -numMutants <number mutants per generation>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   [-behaveQuorum <behaving member quorum required to advance behavior testing to next sensory-motor step>",
   (char *)"      (defaults to immediate testing of entire behavior sequences)",
   (char *)"      [<maximum generations before advancing without a quorum>]]",
   (char *)"   -excitatoryNeurons <minimum number> <maximum> <max delta> <probability of random change>",
   (char *)"   -inhibitoryNeurons <minimum number> <maximum> <max delta> <probability of random change>",
   (char *)"   -synapsePropensities <minimum> <maximum> <max delta> <probability of random change>",
   (char *)"   -synapseWeights <minimum> <maximum> <max delta> <probability of random change>",
   (char *)"   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default=\"network_\")>]",
   (char *)"   [-randomSeed <random seed>]",
   (char *)"   [-logMorph <morph log file name> (instead of standard output)]",
#ifdef THREADS
   (char *)"   [-numThreads <number of threads> (defaults to system capacity)]",
#endif
   (char *)"",
   (char *)"bionet (resume morph)",
   (char *)"   -createIsomorphicNetworks",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"   -loadMorph <morph file name>",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   -saveMorph <morph file name> and/or -saveNetworks [<files prefix (default=\"network_\")>]",
   (char *)"   [-logMorph <morph log file name> (instead of standard output)]",
#ifdef THREADS
   (char *)"   [-numThreads <number of threads> (defaults to system capacity)]",
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


// Create network.
int createNetwork(int argc, char *argv[])
{
   int    numNeurons        = -1;
   int    numSensors        = -1;
   int    numMotors         = -1;
   float  inhibitorDensity  = Network::DEFAULT_INHIBITOR_DENSITY;
   float  synapsePropensity = Network::DEFAULT_SYNAPSE_PROPENSITY;
   float  minSynapseWeight  = 0.0f;
   float  maxSynapseWeight  = 1.0f;
   RANDOM randomSeed        = Network::DEFAULT_RANDOM_SEED;
   char   *networkSaveFile  = NULL;
   char   *networkGraphFile = NULL;

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-createNetwork") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-numNeurons") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numNeurons = atoi(argv[i]);
         if (numNeurons < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numSensors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numSensors = atoi(argv[i]);
         if (numSensors < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numMotors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numMotors = atoi(argv[i]);
         if (numMotors < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-inhibitorDensity") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         inhibitorDensity = (float)atof(argv[i]);
         if ((inhibitorDensity < 0.0f) || (inhibitorDensity > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-synapsePropensity") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         synapsePropensity = (float)atof(argv[i]);
         if ((synapsePropensity <= 0.0f) || (synapsePropensity > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-minSynapseWeight") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         minSynapseWeight = (float)atof(argv[i]);
         if ((minSynapseWeight <= 0.0f) || (minSynapseWeight > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-maxSynapseWeight") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         maxSynapseWeight = (float)atof(argv[i]);
         if ((maxSynapseWeight <= 0.0f) || (maxSynapseWeight > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
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
         randomSeed = atoi(argv[i]);
         continue;
      }
      if (strcmp(argv[i], "-saveNetwork") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         networkSaveFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-graphNetwork") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         networkGraphFile = argv[i];
         continue;
      }
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((numNeurons < 0) || (numSensors < 0) || (numMotors < 0))
   {
      printUsageError((char *)"missing required option");
      return(1);
   }
   if (numNeurons <= (numSensors + numMotors))
   {
      printUsageError((char *)"numNeurons <= (numSensors + numMotors)");
      return(1);
   }
   if (minSynapseWeight > maxSynapseWeight)
   {
      printUsageError((char *)"minSynapseWeight > maxSynapseWeight");
      return(1);
   }

   // Create network.
   Network *network = new Network(numNeurons, numSensors, numMotors,
                                  inhibitorDensity, synapsePropensity,
                                  minSynapseWeight, maxSynapseWeight,
                                  randomSeed);
   assert(network != NULL);
   network->print();
   if (networkSaveFile != NULL)
   {
      if (!network->save(networkSaveFile))
      {
         fprintf(stderr, "Cannot save network to file %s\n", networkSaveFile);
         delete network;
         return(1);
      }
   }
   if (networkGraphFile != NULL)
   {
      if (!network->dumpGraph((char *)"Network", networkGraphFile))
      {
         fprintf(stderr, "Cannot write network graph to file %s\n", networkGraphFile);
         delete network;
         return(1);
      }
   }
   delete network;
   return(0);
}


// Print network.
int printNetwork(int argc, char *argv[])
{
   bool connectivity     = false;
   char *networkLoadFile = NULL;

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-printNetwork") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-connectivity") == 0)
      {
         connectivity = true;
         continue;
      }
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
      printUsageError((char *)"invalid option");
      return(1);
   }

   if (networkLoadFile == NULL)
   {
      printUsageError((char *)"missing required option");
      return(1);
   }

   // Load and print network.
   Network *network = new Network(networkLoadFile);
   assert(network != NULL);
   network->print(true, connectivity);
   delete network;
   return(0);
}


// Graph network.
int graphNetwork(int argc, char *argv[])
{
   char *networkGraphFile = NULL;
   char *networkLoadFile  = NULL;
   char *title            = (char *)"Network";

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-graphNetwork") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         networkGraphFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-title") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         title = argv[i];
         continue;
      }
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
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((networkLoadFile == NULL) || (networkGraphFile == NULL))
   {
      printUsageError((char *)"missing required option");
      return(1);
   }

   // Load and graph network.
   Network *network = new Network(networkLoadFile);
   assert(network != NULL);
   network->dumpGraph(title, networkGraphFile);
   delete network;
   return(0);
}


// Create network behaviors.
int createNetworkBehaviors(int argc, char *argv[])
{
   int    i, j, result;
   RANDOM randomSeed       = Network::DEFAULT_RANDOM_SEED;
   char   *networkLoadFile = NULL;

   vector<int> behaviorSequenceLengths;
   char        *behaviorsSaveFile = NULL;

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-createNetworkBehaviors") == 0)
      {
         continue;
      }
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
      if (strcmp(argv[i], "-behaviorLengths") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaviorSequenceLengths.push_back(atoi(argv[i]));
         for (j = i + 1; j < argc && argv[j][0] != '-'; i = j, j++)
         {
            behaviorSequenceLengths.push_back(atoi(argv[j]));
         }
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
         randomSeed = atoi(argv[i]);
         continue;
      }
      if (strcmp(argv[i], "-saveBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaviorsSaveFile = argv[i];
         continue;
      }
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((networkLoadFile == NULL) || (behaviorSequenceLengths.size() == 0))
   {
      printUsageError((char *)"missing required option");
      return(1);
   }

   // Create behaviors.
   Network *network = new Network(networkLoadFile);
   assert(network != NULL);
   Random             *randomizer = new Random(randomSeed);
   vector<Behavior *> behaviors;
   for (i = 0; i < (int)behaviorSequenceLengths.size(); i++)
   {
      network->clear();
      Behavior *behavior = new Behavior(network, behaviorSequenceLengths[i], randomizer);
      assert(behavior != NULL);
      behaviors.push_back(behavior);
      printf("Behavior %d:\n", i);
      behavior->print();
   }
   result = 0;
   if (behaviorsSaveFile != NULL)
   {
      if (!Behavior::saveBehaviors(behaviorsSaveFile, behaviors))
      {
         fprintf(stderr, "Cannot save behaviors to file %s\n", behaviorsSaveFile);
         result = 1;
      }
   }
   for (i = 0; i < (int)behaviors.size(); i++)
   {
      delete behaviors[i];
   }
   behaviors.clear();
   delete network;
   return(result);
}


// Test network behaviors.
int testNetworkBehaviors(int argc, char *argv[])
{
   int   i, n, result;
   char  *networkLoadFile    = NULL;
   char  *behaviorsLoadFile  = NULL;
   float motorDeltaTolerance = 0.0f;

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-testNetworkBehaviors") == 0)
      {
         continue;
      }
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
      if (strcmp(argv[i], "-loadBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaviorsLoadFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-motorDeltaTolerance") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         motorDeltaTolerance = (float)atof(argv[i]);
         continue;
      }
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((networkLoadFile == NULL) || (behaviorsLoadFile == NULL))
   {
      printUsageError((char *)"missing required option");
      return(1);
   }

   // Test behaviors.
   Network *network = new Network(networkLoadFile);
   assert(network != NULL);
   network->print();
   vector<Behavior *> behaviors;
   if (!Behavior::loadBehaviors(behaviorsLoadFile, behaviors))
   {
      fprintf(stderr, "Cannot load behaviors from file %s\n", behaviorsLoadFile);
      delete network;
      return(1);
   }
   result = 0;
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      Behavior *behavior = behaviors[i];
      if ((behavior->sensorSequence.size() > 0) &&
          ((int)behavior->sensorSequence[0].size() != network->numSensors))
      {
         fprintf(stderr, "Number of sensors not equal\n");
         result = 1;
         break;
      }
      printf("Behavior %d:\n", i);
      behavior->print();
      Behavior *testBehavior = new Behavior(network, behavior->sensorSequence);
      assert(testBehavior != NULL);
      printf("Test:\n");
      testBehavior->print();
      testBehavior->printMotorDeltas(behavior, motorDeltaTolerance);
      delete testBehavior;
   }
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      delete behaviors[i];
   }
   behaviors.clear();
   delete network;
   return(result);
}


// Print network behaviors.
int printNetworkBehaviors(int argc, char *argv[])
{
   int  i, n;
   char *behaviorsLoadFile = NULL;

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-printNetworkBehaviors") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-loadBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaviorsLoadFile = argv[i];
         continue;
      }
      printUsageError((char *)"invalid option");
      return(1);
   }

   if (behaviorsLoadFile == NULL)
   {
      printUsageError((char *)"missing required option");
      return(1);
   }

   // Print behaviors.
   vector<Behavior *> behaviors;
   if (!Behavior::loadBehaviors(behaviorsLoadFile, behaviors))
   {
      fprintf(stderr, "Cannot load behaviors from file %s\n", behaviorsLoadFile);
      return(1);
   }
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      Behavior *behavior = behaviors[i];
      printf("Behavior %d:\n", i);
      behavior->print();
      delete behavior;
   }
   behaviors.clear();
   return(0);
}


// Create homomorphic networks.
int createHomomorphicNetworks(int argc, char *argv[])
{
   int  i, j, k, n, result;
   char *behaviorsLoadFile         = NULL;
   char *networkLoadFile           = NULL;
   int  populationSize             = -1;
   int  numOffspring               = -1;
   int  parentLongevity            = -1;
   int  numGenerations             = -1;
   int  behaveCutoff               = -1;
   int  behaveQuorum               = -1;
   int  behaveQuorumMaxGenerations = -1;

   vector<int> fitnessMotorList;
   bool        gotFitnessMotorList = false;
   float       crossoverRate       = -1.0f;
   float       mutationRate        = -1.0f;
   MutableParm synapseWeightsParm;
   float       synapseCrossoverBondStrength = -1.0f;
   int         synapseOptimizedPathLength   = -1;
   bool        saveNetworks   = false;
   char        *filesPrefix   = (char *)"network_";
   RANDOM      randomSeed     = Network::DEFAULT_RANDOM_SEED;
   bool        gotRandomSeed  = false;
   char        *morphSaveFile = NULL;
   char        *morphLoadFile = NULL;
   char        *logFile       = NULL;

#ifdef THREADS
   int numThreads = -1;
#endif

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-createHomomorphicNetworks") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-loadBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaviorsLoadFile = argv[i];
         continue;
      }
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
      if (strcmp(argv[i], "-parentLongevity") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         parentLongevity = atoi(argv[i]);
         if (parentLongevity <= 0)
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
      if (strcmp(argv[i], "-behaveCutoff") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaveCutoff = atoi(argv[i]);
         if (behaveCutoff < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-behaveQuorum") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaveQuorum = atoi(argv[i]);
         if (behaveQuorum < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         if (((i + 1) < argc) && (argv[i + 1][0] != '-'))
         {
            i++;
            behaveQuorumMaxGenerations = atoi(argv[i]);
            if (behaveQuorumMaxGenerations < 0)
            {
               printUsageError(argv[i - 2]);
               return(1);
            }
         }
         continue;
      }
      if (strcmp(argv[i], "-fitnessMotorList") == 0)
      {
         gotFitnessMotorList = true;
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            i--;
            continue;
         }
         char comma[] = ",";
         char *token  = strtok(argv[i], comma);
         while (token != NULL)
         {
            n = atoi(token);
            if (n < 0)
            {
               printUsageError(argv[i - 1]);
               return(1);
            }
            for (int j = 0; j < (int)fitnessMotorList.size(); j++)
            {
               if (fitnessMotorList[j] == n)
               {
                  printUsageError(argv[i - 1]);
                  return(1);
               }
            }
            fitnessMotorList.push_back(n);
            token = strtok(NULL, comma);
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
      if ((strcmp(argv[i], "-synapseWeights") == 0))
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
         synapseWeightsParm.init(min, max, maxDelta, 0.0f);
         continue;
      }
      if ((strcmp(argv[i], "-synapseCrossoverBondStrength") == 0))
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         synapseCrossoverBondStrength = (float)atof(argv[i]);
         if ((synapseCrossoverBondStrength < 0.0f) || (synapseCrossoverBondStrength > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-synapseOptimizedPathLength") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         synapseOptimizedPathLength = atoi(argv[i]);
         if (synapseOptimizedPathLength < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-saveNetworks") == 0)
      {
         saveNetworks = true;
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            i--;
         }
         else
         {
            filesPrefix = argv[i];
         }
         continue;
      }
      if (strcmp(argv[i], "-saveMorph") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         morphSaveFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-loadMorph") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         morphLoadFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-randomSeed") == 0)
      {
         gotRandomSeed = true;
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         randomSeed = atoi(argv[i]);
         continue;
      }
      if (strcmp(argv[i], "-logMorph") == 0)
      {
         i++;
         if (i >= argc)
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
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((behaviorsLoadFile == NULL))
   {
      printUsageError((char *)"missing loadBehaviors option");
      return(1);
   }
   if (numGenerations < 0)
   {
      printUsageError((char *)"missing numGenerations option");
      return(1);
   }
   if ((morphSaveFile == NULL) && !saveNetworks)
   {
      printUsageError((char *)"missing save option");
      return(1);
   }

   vector<Behavior *> behaviors;
   if (!Behavior::loadBehaviors(behaviorsLoadFile, behaviors))
   {
      fprintf(stderr, "Cannot load behaviors from file %s\n", behaviorsLoadFile);
      return(1);
   }
   Network *homomorph = NULL;
   NetworkHomomorphoGenesis *morphoGenesis = NULL;
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
   result = 0;
   if (morphLoadFile == NULL)
   {
      if ((networkLoadFile == NULL) ||
          (populationSize < 0) ||
          (numOffspring < 0) ||
          (crossoverRate < 0.0f) ||
          (mutationRate < 0.0f) ||
          (synapseWeightsParm.randomProbability < 0.0f) ||
          (synapseCrossoverBondStrength < 0.0f) ||
          (synapseOptimizedPathLength < 0))
      {
         printUsageError((char *)"missing required option");
         return(1);
      }
      if (behaveQuorum > populationSize)
      {
         printUsageError((char *)"behaveQuorum > populationSize");
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
      homomorph = new Network(networkLoadFile);
      assert(homomorph != NULL);
      morphoGenesis =
         new NetworkHomomorphoGenesis(
            behaviors, homomorph,
            populationSize, numOffspring, parentLongevity,
            fitnessMotorList, behaveQuorum, behaveQuorumMaxGenerations,
            crossoverRate, mutationRate, synapseWeightsParm,
            synapseCrossoverBondStrength, synapseOptimizedPathLength,
            randomSeed);
      assert(morphoGenesis != NULL);
   }
   else   // Resume morph.
   {
      if ((networkLoadFile != NULL) ||
          (populationSize >= 0) ||
          (numOffspring >= 0) ||
          (parentLongevity >= 0) ||
          (behaveQuorum >= 0) ||
          (synapseWeightsParm.randomProbability >= 0.0f) ||
          (gotRandomSeed))
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
      morphoGenesis = new NetworkHomomorphoGenesis(behaviors, morphLoadFile);
      assert(morphoGenesis != NULL);
      if (fitnessMotorList.size() > 0)
      {
         n = morphoGenesis->homomorph->numMotors;
         ((NetworkMorphoGenesis *)morphoGenesis)->fitnessMotorList.resize(n, false);
         for (i = 0, j = (int)fitnessMotorList.size(); i < j; i++)
         {
            k = fitnessMotorList[i];
            assert(k >= 0 && k < n);
            ((NetworkMorphoGenesis *)morphoGenesis)->fitnessMotorList[k] = true;
         }
      }
      else if (gotFitnessMotorList)
      {
         ((NetworkMorphoGenesis *)morphoGenesis)->fitnessMotorList.clear();
      }
      if (crossoverRate != -1.0f)
      {
         morphoGenesis->crossoverRate = crossoverRate;
      }
      if (mutationRate != -1.0f)
      {
         morphoGenesis->mutationRate = mutationRate;
      }
      if (synapseCrossoverBondStrength != -1.0f)
      {
         morphoGenesis->synapseCrossoverBondStrength = synapseCrossoverBondStrength;
      }
      if (synapseOptimizedPathLength != -1)
      {
         morphoGenesis->synapseOptimizedPathLength = synapseOptimizedPathLength;
      }
   }
#ifdef THREADS
   morphoGenesis->morph(numGenerations, numThreads, behaveCutoff, logFile);
#else
   morphoGenesis->morph(numGenerations, behaveCutoff, logFile);
#endif
   if (morphSaveFile != NULL)
   {
      if (!morphoGenesis->save(morphSaveFile))
      {
         fprintf(stderr, "Cannot save morph to file %s\n", morphSaveFile);
         result = 1;
      }
   }
   if (saveNetworks)
   {
      ((NetworkMorphoGenesis *)morphoGenesis)->saveNetworks(filesPrefix);
   }
   delete morphoGenesis;
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      delete behaviors[i];
   }
   behaviors.clear();
   if (homomorph != NULL)
   {
      delete homomorph;
   }
   return(result);
}


// Merge homomorphic networks.
int mergeHomomorphicNetworks(int argc, char *argv[])
{
   int    i, n;
   char   *morphLoadFile0 = NULL;
   char   *morphLoadFile1 = NULL;
   char   *morphSaveFile  = NULL;
   RANDOM randomSeed      = Network::DEFAULT_RANDOM_SEED;

   vector<Behavior *>       behaviors;
   NetworkHomomorphoGenesis *morphoGenesis0;
   NetworkHomomorphoGenesis *morphoGenesis1;
   Random *randomizer;

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-mergeHomomorphicNetworks") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-loadMorph") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         morphLoadFile0 = argv[i];
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         morphLoadFile1 = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-saveMorph") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         morphSaveFile = argv[i];
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
         randomSeed = atoi(argv[i]);
         continue;
      }
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((morphLoadFile0 == NULL) || (morphLoadFile1 == NULL))
   {
      printUsageError((char *)"missing load option");
      return(1);
   }
   if (morphSaveFile == NULL)
   {
      printUsageError((char *)"missing save option");
      return(1);
   }

   morphoGenesis0 = new NetworkHomomorphoGenesis(behaviors, morphLoadFile0);
   assert(morphoGenesis0 != NULL);
   n = (int)((NetworkMorphoGenesis *)morphoGenesis0)->populationSize;
   morphoGenesis1 = new NetworkHomomorphoGenesis(behaviors, morphLoadFile1);
   assert(morphoGenesis1 != NULL);
   assert(n == (int)((NetworkMorphoGenesis *)morphoGenesis1)->populationSize);
   randomizer = new Random(randomSeed);
   assert(randomizer != NULL);
   for (i = 0; i < n; i++)
   {
      if (randomizer->RAND_BOOL())
      {
         ((NetworkMorphoGenesis *)morphoGenesis0)->population[i] =
            ((NetworkMorphoGenesis *)morphoGenesis1)->population[i];
      }
   }
   if (!morphoGenesis0->save(morphSaveFile))
   {
      fprintf(stderr, "Cannot save morph to file %s\n", morphSaveFile);
      return(1);
   }
   return(0);
}


// Create isomorphic networks.
int createIsomorphicNetworks(int argc, char *argv[])
{
   int         i, n, result;
   char        *behaviorsLoadFile         = NULL;
   int         populationSize             = -1;
   int         numMutants                 = -1;
   int         numGenerations             = -1;
   int         behaveQuorum               = -1;
   int         behaveQuorumMaxGenerations = -1;
   MutableParm excitatoryNeuronsParm;
   MutableParm inhibitoryNeuronsParm;
   MutableParm synapsePropensitiesParm;
   MutableParm synapseWeightsParm;
   bool        saveNetworks   = false;
   char        *filesPrefix   = (char *)"network_";
   RANDOM      randomSeed     = Network::DEFAULT_RANDOM_SEED;
   bool        gotRandomSeed  = false;
   char        *morphSaveFile = NULL;
   char        *morphLoadFile = NULL;
   char        *logFile       = NULL;

#ifdef THREADS
   int numThreads = -1;
#endif

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-createIsomorphicNetworks") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-loadBehaviors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaviorsLoadFile = argv[i];
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
      if (strcmp(argv[i], "-numMutants") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         numMutants = atoi(argv[i]);
         if (numMutants < 0)
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
      if (strcmp(argv[i], "-behaveQuorum") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         behaveQuorum = atoi(argv[i]);
         if (behaveQuorum < 0)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         if (((i + 1) < argc) && (argv[i + 1][0] != '-'))
         {
            i++;
            behaveQuorumMaxGenerations = atoi(argv[i]);
            if (behaveQuorumMaxGenerations < 0)
            {
               printUsageError(argv[i - 2]);
               return(1);
            }
         }
         continue;
      }
      if ((strcmp(argv[i], "-excitatoryNeurons") == 0) ||
          (strcmp(argv[i], "-inhibitoryNeurons") == 0) ||
          (strcmp(argv[i], "-synapsePropensities") == 0) ||
          (strcmp(argv[i], "-synapseWeights") == 0))
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
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 4]);
            return(1);
         }
         float prob = (float)atof(argv[i]);
         if ((prob < 0.0f) || (prob > 1.0f))
         {
            printUsageError(argv[i - 4]);
            return(1);
         }
         if (strcmp(argv[i - 4], "-excitatoryNeurons") == 0)
         {
            excitatoryNeuronsParm.init(min, max, maxDelta, prob);
         }
         else if (strcmp(argv[i - 4], "-inhibitoryNeurons") == 0)
         {
            inhibitoryNeuronsParm.init(min, max, maxDelta, prob);
         }
         else if (strcmp(argv[i - 4], "-synapsePropensities") == 0)
         {
            synapsePropensitiesParm.init(min, max, maxDelta, prob);
         }
         else if (strcmp(argv[i - 4], "-synapseWeights") == 0)
         {
            synapseWeightsParm.init(min, max, maxDelta, prob);
         }
         continue;
      }
      if (strcmp(argv[i], "-saveNetworks") == 0)
      {
         saveNetworks = true;
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            i--;
         }
         else
         {
            filesPrefix = argv[i];
         }
         continue;
      }
      if (strcmp(argv[i], "-saveMorph") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         morphSaveFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-loadMorph") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         morphLoadFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-randomSeed") == 0)
      {
         gotRandomSeed = true;
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         randomSeed = atoi(argv[i]);
         continue;
      }
      if (strcmp(argv[i], "-logMorph") == 0)
      {
         i++;
         if (i >= argc)
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
      printUsageError((char *)"invalid option");
      return(1);
   }

   if ((behaviorsLoadFile == NULL))
   {
      printUsageError((char *)"missing loadBehaviors option");
      return(1);
   }
   if (numGenerations < 0)
   {
      printUsageError((char *)"missing numGenerations option");
      return(1);
   }
   if ((morphSaveFile == NULL) && !saveNetworks)
   {
      printUsageError((char *)"missing save option");
      return(1);
   }

   vector<Behavior *> behaviors;
   if (!Behavior::loadBehaviors(behaviorsLoadFile, behaviors))
   {
      fprintf(stderr, "Cannot load behaviors from file %s\n", behaviorsLoadFile);
      return(1);
   }
   NetworkIsomorphoGenesis *morphoGenesis = NULL;
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
   result = 0;
   if (morphLoadFile == NULL)
   {
      if ((populationSize < 0) ||
          (numMutants < 0) ||
          (excitatoryNeuronsParm.randomProbability < 0.0f) ||
          (inhibitoryNeuronsParm.randomProbability < 0.0f) ||
          (synapsePropensitiesParm.randomProbability < 0.0f) ||
          (synapseWeightsParm.randomProbability < 0.0f))
      {
         printUsageError((char *)"missing required option");
         return(1);
      }
      if (behaveQuorum > populationSize)
      {
         printUsageError((char *)"behaveQuorum > populationSize");
         return(1);
      }
      if (numMutants > populationSize)
      {
         printUsageError((char *)"numMutants > populationSize");
         return(1);
      }
      morphoGenesis =
         new NetworkIsomorphoGenesis(
            behaviors,
            populationSize, numMutants,
            behaveQuorum, behaveQuorumMaxGenerations,
            excitatoryNeuronsParm, inhibitoryNeuronsParm,
            synapsePropensitiesParm, synapseWeightsParm,
            randomSeed);
      assert(morphoGenesis != NULL);
   }
   else   // Resume morph.
   {
      if ((populationSize >= 0) ||
          (numMutants >= 0) ||
          (behaveQuorum >= 0) ||
          (synapseWeightsParm.randomProbability >= 0.0f) ||
          (gotRandomSeed))
      {
         printUsageError((char *)"invalid option");
         return(1);
      }
      morphoGenesis = new NetworkIsomorphoGenesis(behaviors, morphLoadFile);
      assert(morphoGenesis != NULL);
   }
#ifdef THREADS
   morphoGenesis->morph(numGenerations, numThreads, logFile);
#else
   morphoGenesis->morph(numGenerations, logFile);
#endif
   if (morphSaveFile != NULL)
   {
      if (!morphoGenesis->save(morphSaveFile))
      {
         fprintf(stderr, "Cannot save morph to file %s\n", morphSaveFile);
         result = 1;
      }
   }
   if (saveNetworks)
   {
      ((NetworkMorphoGenesis *)morphoGenesis)->saveNetworks(filesPrefix);
   }
   delete morphoGenesis;
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      delete behaviors[i];
   }
   behaviors.clear();
   return(result);
}


int behaviorSearch(int argc, char *argv[]);


int main(int argc, char *argv[])
{
   enum
   {
      CREATE_NETWORK,
      PRINT_NETWORK,
      GRAPH_NETWORK,
      CREATE_NETWORK_BEHAVIORS,
      TEST_NETWORK_BEHAVIORS,
      PRINT_NETWORK_BEHAVIORS,
      CREATE_HOMOMORPHIC_NETWORKS,
      MERGE_HOMOMORPHIC_NETWORKS,
      CREATE_ISOMORPHIC_NETWORKS,
      BEHAVIOR_SEARCH,
      UNASSIGNED
   }
   command = UNASSIGNED;

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-createNetwork") == 0)
      {
         if ((command == UNASSIGNED) || (command == GRAPH_NETWORK))
         {
            command = CREATE_NETWORK;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-printNetwork") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = PRINT_NETWORK;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-graphNetwork") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = GRAPH_NETWORK;
         }
         else if (command != CREATE_NETWORK)
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-createNetworkBehaviors") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = CREATE_NETWORK_BEHAVIORS;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-testNetworkBehaviors") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = TEST_NETWORK_BEHAVIORS;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-printNetworkBehaviors") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = PRINT_NETWORK_BEHAVIORS;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-createHomomorphicNetworks") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = CREATE_HOMOMORPHIC_NETWORKS;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-mergeHomomorphicNetworks") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = MERGE_HOMOMORPHIC_NETWORKS;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-createIsomorphicNetworks") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = CREATE_ISOMORPHIC_NETWORKS;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
      if (strcmp(argv[i], "-behaviorSearch") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = BEHAVIOR_SEARCH;
         }
         else
         {
            printUsageError((char *)"multiple commands");
            return(1);
         }
      }
   }

   switch (command)
   {
   case CREATE_NETWORK:
      return(createNetwork(argc, argv));

   case PRINT_NETWORK:
      return(printNetwork(argc, argv));

   case GRAPH_NETWORK:
      return(graphNetwork(argc, argv));

   case CREATE_NETWORK_BEHAVIORS:
      return(createNetworkBehaviors(argc, argv));

   case TEST_NETWORK_BEHAVIORS:
      return(testNetworkBehaviors(argc, argv));

   case PRINT_NETWORK_BEHAVIORS:
      return(printNetworkBehaviors(argc, argv));

   case CREATE_HOMOMORPHIC_NETWORKS:
      return(createHomomorphicNetworks(argc, argv));

   case MERGE_HOMOMORPHIC_NETWORKS:
      return(mergeHomomorphicNetworks(argc, argv));

   case CREATE_ISOMORPHIC_NETWORKS:
      return(createIsomorphicNetworks(argc, argv));

   case BEHAVIOR_SEARCH:
      return(behaviorSearch(argc, argv));

   case UNASSIGNED:
      printUsageError((char *)"missing command");
      return(1);
   }
}


// Usage.
char *Usage2[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -behaviorSearch",
   (char *)"   -numTrials <number of trials>",
   (char *)"   -sequenceLength <behavior sequence length>",
   (char *)"   -numNeurons <number of neurons>",
   (char *)"   -numSensors <number of sensors>",
   (char *)"   -numMotors <number of motors>",
   (char *)"   [-inhibitorDensity <inhibitory neuron density [0.0,1.0]>]",
   (char *)"   [-synapsePropensity <synapse generation propensity (0.0,1.0]>]",
   (char *)"   [-minSynapseWeight <minimum synapse weight (defaults to 0.0)>]",
   (char *)"   [-maxSynapseWeight <maximum synapse weight (defaults to 1.0)>]",
   (char *)"   [-randomSeed <random seed>]",
   NULL
};

void printUsage2()
{
   for (int i = 0; Usage2[i] != NULL; i++)
   {
      fprintf(stderr, (char *)"%s\n", Usage2[i]);
   }
}


void printUsageError2(char *error)
{
   fprintf(stderr, (char *)"Error: %s\n", error);
   printUsage2();
}


// Search for interesting behaviors.
int behaviorSearch(int argc, char *argv[])
{
   int    numTrials         = -1;
   int    seqLength         = -1;
   int    numNeurons        = -1;
   int    numSensors        = -1;
   int    numMotors         = -1;
   float  inhibitorDensity  = Network::DEFAULT_INHIBITOR_DENSITY;
   float  synapsePropensity = Network::DEFAULT_SYNAPSE_PROPENSITY;
   float  minSynapseWeight  = 0.0f;
   float  maxSynapseWeight  = 1.0f;
   RANDOM randomSeed        = Network::DEFAULT_RANDOM_SEED;
   Random *randomizer;

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-behaviorSearch") == 0)
      {
         continue;
      }
      if (strcmp(argv[i], "-numTrials") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         numTrials = atoi(argv[i]);
         if (numTrials < 0)
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-sequenceLength") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         seqLength = atoi(argv[i]);
         if (seqLength < 0)
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numNeurons") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         numNeurons = atoi(argv[i]);
         if (numNeurons < 0)
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numSensors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         numSensors = atoi(argv[i]);
         if (numSensors < 0)
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-numMotors") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         numMotors = atoi(argv[i]);
         if (numMotors < 0)
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-inhibitorDensity") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         inhibitorDensity = (float)atof(argv[i]);
         if ((inhibitorDensity < 0.0f) || (inhibitorDensity > 1.0f))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-synapsePropensity") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         synapsePropensity = (float)atof(argv[i]);
         if ((synapsePropensity <= 0.0f) || (synapsePropensity > 1.0f))
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-minSynapseWeight") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         minSynapseWeight = (float)atof(argv[i]);
         if ((minSynapseWeight <= 0.0f) || (minSynapseWeight > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-maxSynapseWeight") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         maxSynapseWeight = (float)atof(argv[i]);
         if ((maxSynapseWeight <= 0.0f) || (maxSynapseWeight > 1.0f))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         continue;
      }
      if (strcmp(argv[i], "-randomSeed") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError2(argv[i - 1]);
            return(1);
         }
         randomSeed = atoi(argv[i]);
         continue;
      }
      printUsageError2((char *)"invalid option");
      return(1);
   }

   if ((numTrials < 0) || (seqLength < 0))
   {
      printUsageError2((char *)"missing required option");
      return(1);
   }
   if ((numNeurons < 0) || (numSensors < 0) || (numMotors < 0))
   {
      printUsageError2((char *)"missing required option");
      return(1);
   }
   if (numNeurons <= (numSensors + numMotors))
   {
      printUsageError2((char *)"numNeurons <= (numSensors + numMotors)");
      return(1);
   }
   if (minSynapseWeight > maxSynapseWeight)
   {
      printUsageError2((char *)"minSynapseWeight > maxSynapseWeight");
      return(1);
   }
   randomizer = new Random(randomSeed);
   assert(randomizer != NULL);

   for (int trial = 0; trial < numTrials; trial++)
   {
      printf("trial=%d\n", trial);
      randomSeed = randomizer->RAND();
      printf("randomSeed=%lu\n", randomSeed);
      Network *network = new Network(numNeurons, numSensors, numMotors,
                                     inhibitorDensity, synapsePropensity, minSynapseWeight, maxSynapseWeight, randomSeed);
      assert(network != NULL);
      Behavior *behavior = new Behavior(network, seqLength, randomizer);
      assert(behavior != NULL);
      int n = (int)behavior->sensorSequence.size();
      for (int i = 0; i < numMotors; i++)
      {
         if (fabs(behavior->motorSequence[n - 2][i] - behavior->motorSequence[n - 1][i]) >= 0.01f)
         {
            printf("behavior found:\n");
            behavior->print();
            break;
         }
      }
      delete behavior;
      delete network;
   }

   return(0);
}
