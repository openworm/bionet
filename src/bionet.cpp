// bionet: a "biological" (irregular) neural network.

#include "networkMorph.hpp"

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
   (char *)"",
   (char *)"Print network behaviors:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -printNetworkBehaviors",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"",
   (char *)"Generate isomorphic/homomorphic networks:",
   (char *)"",
   (char *)"bionet",
   (char *)"   -createMorphicNetworks",
   (char *)"   -loadBehaviors <behaviors file name>",
   (char *)"   -excitatoryNeurons <minimum number> <maximum> <max delta> <probability of random change>",
   (char *)"   -inhibitoryNeurons <minimum number> <maximum> <max delta> <probability of random change>",
   (char *)"   -synapsePropensities <minimum> <maximum> <max delta> <probability of random change>",
   (char *)"   -synapseWeights <max delta> <probability of random change>",
   (char *)"   -populationSize <number population members>",
   (char *)"   -numMutants <number mutants per generation>",
   (char *)"   [-loadNetwork <homomorph network file name> (to create homomorphic networks)",
   (char *)"      [-homomorphClones (start with population of homomorphic network clones)]",
   (char *)"      [-numOffspring <number mating offspring per generation> (for homomorphic networks)]]",
   (char *)"   -numGenerations <number of evolution generations>",
   (char *)"   [-fitnessQuorum <fit member quorum required to advance behavior testing to next sensory-motor step>",
   (char *)"      (defaults to immediate testing of entire behavior sequences)]",
   (char *)"   -saveNetworks [<files prefix (default=\"network_\")>]",
   (char *)"   [-randomSeed <random seed>]",
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

   // Create network.
   Network *network = new Network(numNeurons, numSensors, numMotors,
                                  inhibitorDensity, synapsePropensity, randomSeed);
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
   RANDOM randomSeed       = Network::DEFAULT_RANDOM_SEED;
   char   *networkLoadFile = NULL;

   vector<int> behaviorSequenceLengths;
   char        *behaviorsSaveFile = NULL;

   for (int i = 1; i < argc; i++)
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
         for (int j = i + 1; j < argc && argv[j][0] != '-'; i = j, j++)
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
   Random *randomizer = new Random(randomSeed);
   FILE   *fp         = NULL;
   if (behaviorsSaveFile != NULL)
   {
      fp = fopen(behaviorsSaveFile, "w");
      if (fp == NULL)
      {
         fprintf(stderr, "Cannot open behaviors save file %s\n", behaviorsSaveFile);
         delete network;
         return(1);
      }
      fprintf(fp, "%d\n", (int)behaviorSequenceLengths.size());
   }
   for (int i = 0; i < (int)behaviorSequenceLengths.size(); i++)
   {
      Behavior *behavior = new Behavior(network, behaviorSequenceLengths[i], randomizer);
      assert(behavior != NULL);
      printf("Behavior %d:\n", i);
      behavior->print();
      if (fp != NULL)
      {
         behavior->save(fp);
      }
      delete behavior;
   }
   if (fp != NULL)
   {
      fclose(fp);
   }
   delete network;
   return(0);
}


// Test network behaviors.
int testNetworkBehaviors(int argc, char *argv[])
{
   char *networkLoadFile   = NULL;
   char *behaviorsLoadFile = NULL;

   for (int i = 1; i < argc; i++)
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
   FILE *fp = fopen(behaviorsLoadFile, "r");
   if (fp == NULL)
   {
      fprintf(stderr, "Cannot open behaviors load file %s\n", behaviorsLoadFile);
      delete network;
      return(1);
   }
   int n;
   fscanf(fp, "%d", &n);
   for (int i = 0; i < n; i++)
   {
      Behavior *behavior = new Behavior(fp);
      assert(behavior != NULL);
      if ((behavior->sensorSequence.size() > 0) &&
          ((int)behavior->sensorSequence[0].size() != network->numSensors))
      {
         fprintf(stderr, "Number of sensors not equal\n");
         delete behavior;
         delete network;
         return(1);
      }
      printf("Behavior %d:\n", i);
      behavior->print();
      Behavior *testBehavior = new Behavior(network, behavior->sensorSequence);
      assert(testBehavior != NULL);
      printf("Test:\n");
      testBehavior->print();
      delete behavior;
      delete testBehavior;
   }
   fclose(fp);
   delete network;
   return(0);
}


// Print network behaviors.
int printNetworkBehaviors(int argc, char *argv[])
{
   char *behaviorsLoadFile = NULL;

   for (int i = 1; i < argc; i++)
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
   FILE *fp = fopen(behaviorsLoadFile, "r");
   if (fp == NULL)
   {
      fprintf(stderr, "Cannot open behaviors load file %s\n", behaviorsLoadFile);
      return(1);
   }
   int n;
   fscanf(fp, "%d", &n);
   for (int i = 0; i < n; i++)
   {
      Behavior *behavior = new Behavior(fp);
      assert(behavior != NULL);
      printf("Behavior %d:\n", i);
      behavior->print();
      delete behavior;
   }
   fclose(fp);
   return(0);
}


// Create morphic networks.
int createMorphicNetworks(int argc, char *argv[])
{
   char        *behaviorsLoadFile = NULL;
   MutableParm excitatoryNeuronsParm;
   MutableParm inhibitoryNeuronsParm;
   MutableParm synapsePropensitiesParm;
   MutableParm synapseWeightsParm;
   int         populationSize   = -1;
   int         numMutants       = -1;
   char        *networkLoadFile = NULL;
   bool        homomorphClones  = false;
   int         numOffspring     = -1;
   int         numGenerations   = -1;
   int         fitnessQuorum    = -1;
   RANDOM      randomSeed       = Network::DEFAULT_RANDOM_SEED;
   bool        saveNetworks     = false;
   char        *filesPrefix     = (char *)"network_";

   for (int i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-createMorphicNetworks") == 0)
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
      if ((strcmp(argv[i], "-excitatoryNeurons") == 0) ||
          (strcmp(argv[i], "-inhibitoryNeurons") == 0) ||
          (strcmp(argv[i], "-synapsePropensities") == 0))
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
         continue;
      }
      if (strcmp(argv[i], "-synapseWeights") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         float maxDelta = (float)atof(argv[i]);
         if (maxDelta < 0.0f)
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
         float prob = (float)atof(argv[i]);
         if ((prob < 0.0f) || (prob > 1.0f))
         {
            printUsageError(argv[i - 2]);
            return(1);
         }
         synapseWeightsParm.init(0.0f, 1.0f, maxDelta, prob);
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
      if (strcmp(argv[i], "-homomorphClones") == 0)
      {
         homomorphClones = true;
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
      if (strcmp(argv[i], "-fitnessQuorum") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
         fitnessQuorum = atoi(argv[i]);
         if (fitnessQuorum < 0)
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

   if ((behaviorsLoadFile == NULL) ||
       (excitatoryNeuronsParm.randomProbability < 0.0f) ||
       (inhibitoryNeuronsParm.randomProbability < 0.0f) ||
       (synapsePropensitiesParm.randomProbability < 0.0f) ||
       (synapseWeightsParm.randomProbability < 0.0f) ||
       (populationSize < 0) ||
       (numMutants < 0) ||
       ((numGenerations < 0) ||
        !saveNetworks))
   {
      printUsageError((char *)"missing required option");
      return(1);
   }
   if (fitnessQuorum > populationSize)
   {
      printUsageError((char *)"fitnessQuorum > populationSize");
      return(1);
   }
   if (homomorphClones && (networkLoadFile == NULL))
   {
      printUsageError((char *)"homomorphClones option requires homomorph network");
      return(1);
   }
   if ((numOffspring != -1) && (networkLoadFile == NULL))
   {
      printUsageError((char *)"numOffspring option requires homomorph network");
      return(1);
   }
   if (numOffspring == -1)
   {
      numOffspring = 0;
   }
   if ((numMutants + numOffspring) > populationSize)
   {
      printUsageError((char *)"(numMutants + numOffspring) > populationSize");
      return(1);
   }
   if ((numOffspring > 0) && (populationSize < 2))
   {
      printUsageError((char *)"numOffspring > 0 && populationSize < 2");
      return(1);
   }

   FILE *fp = fopen(behaviorsLoadFile, "r");
   if (fp == NULL)
   {
      fprintf(stderr, "Cannot open behaviors load file %s\n", behaviorsLoadFile);
      return(1);
   }
   int n;
   fscanf(fp, "%d", &n);
   vector<Behavior *> behaviors;
   for (int i = 0; i < n; i++)
   {
      Behavior *behavior = new Behavior(fp);
      assert(behavior != NULL);
      behaviors.push_back(behavior);
   }
   fclose(fp);
   Network *homomorph = NULL;
   if (networkLoadFile != NULL)
   {
      homomorph = new Network(networkLoadFile);
      assert(homomorph != NULL);
   }
   NetworkMorphoGenesis *morphoGenesis = new NetworkMorphoGenesis(
      behaviors, homomorph, homomorphClones,
      excitatoryNeuronsParm, inhibitoryNeuronsParm,
      synapsePropensitiesParm, synapseWeightsParm,
      populationSize, numMutants, numOffspring,
      numGenerations, fitnessQuorum, randomSeed);
   assert(morphoGenesis != NULL);
   morphoGenesis->morph();
   morphoGenesis->saveNetworks(filesPrefix);
   delete morphoGenesis;
   for (int i = 0; i < n; i++)
   {
      Behavior *behavior = behaviors[i];
      delete behavior;
   }
   behaviors.clear();
   if (homomorph != NULL)
   {
      delete homomorph;
   }
   return(0);
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
      CREATE_MORPHIC_NETWORKS,
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
      if (strcmp(argv[i], "-createMorphicNetworks") == 0)
      {
         if (command == UNASSIGNED)
         {
            command = CREATE_MORPHIC_NETWORKS;
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

   case CREATE_MORPHIC_NETWORKS:
      return(createMorphicNetworks(argc, argv));

   case BEHAVIOR_SEARCH:
      return(behaviorSearch(argc, argv));

   case UNASSIGNED:
      printUsageError((char *)"missing command");
      return(1);
   }
}


#include <math.h>

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
   randomizer = new Random(randomSeed);
   assert(randomizer != NULL);

   for (int trial = 0; trial < numTrials; trial++)
   {
      printf("trial=%d\n", trial);
      randomSeed = randomizer->RAND();
      printf("randomSeed=%lu\n", randomSeed);
      Network *network = new Network(numNeurons, numSensors, numMotors, inhibitorDensity, synapsePropensity, randomSeed);
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
