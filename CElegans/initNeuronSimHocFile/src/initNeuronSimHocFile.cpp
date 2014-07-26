// Initialize NEURON simulation hoc file synapse weights and stimuli parameters.

#include "neuronHocDoc.hpp"
#include "random.hpp"
using namespace std;

// Usage.
char *Usage[] =
{
   (char *)"Usage:",
   (char *)"",
   (char *)"initNeuronSimHocFile",
   (char *)"   -simHocFile <simulation hoc file name>",
   (char *)"   [-synapseWeightRange <min> <max>]",
   (char *)"   [-stimuliDelayRange <min> <max>]",
   (char *)"   [-stimuliDurationRange <min> <max>]",
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

string SimHocFile;
NeuronHocDoc *SimHocDoc;
float MinWeight = 0.0f;
float  MaxWeight = 1.0f;
float MinDelay = 20.0f;
float MaxDelay = 80.0f;
float MinDuration = 60.0f;
float MaxDuration = 120.0f;
RANDOM RandomSeed = 4517;
Random *Randomizer = NULL;


int main(int argc, char *argv[])
{
	int i, j, x, y;
	char value[BUFSIZ];
	NeuronHocDoc::Synapse *synapse;
	vector<pair<int, int> > values;

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "-simHocFile") == 0)
      {
         i++;
         if ((i >= argc) || (argv[i][0] == '-'))
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
		 SimHocFile = argv[i];
         continue;
      }
      if (strcmp(argv[i], "-synapseWeightRange") == 0)
      {
         i++;
         if (i >= argc)
         {
            printUsageError(argv[i - 1]);
            return(1);
         }
		 MinWeight = (float)atof(argv[i]);
		 i++;
		 if (i >= argc)
		 {
			 printUsageError(argv[i - 2]);
			 return(1);
		 }
		 MaxWeight = (float)atof(argv[i]);
         continue;
      }
	  if (strcmp(argv[i], "-stimuliDelayRange") == 0)
	  {
		  i++;
		  if (i >= argc)
		  {
			  printUsageError(argv[i - 1]);
			  return(1);
		  }
		  MinDelay = (float)atof(argv[i]);
		  i++;
		  if (i >= argc)
		  {
			  printUsageError(argv[i - 2]);
			  return(1);
		  }
		  MaxDelay = (float)atof(argv[i]);
		  continue;
	  }
	  if (strcmp(argv[i], "-stimuliDurationRange") == 0)
	  {
		  i++;
		  if (i >= argc)
		  {
			  printUsageError(argv[i - 1]);
			  return(1);
		  }
		  MinDuration = (float)atof(argv[i]);
		  i++;
		  if (i >= argc)
		  {
			  printUsageError(argv[i - 2]);
			  return(1);
		  }
		  MaxDuration = (float)atof(argv[i]);
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
		  RandomSeed = atoi(argv[i]);
		  continue;
	  }
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
   if (SimHocFile.empty())
   {
      printUsageError((char *)"missing simHocFile option");
      return(1);
   }

   Randomizer = new Random(RandomSeed);
   assert(Randomizer != NULL);
   SimHocDoc = new NeuronHocDoc((char *)SimHocFile.c_str());
   assert(SimHocDoc != NULL);

   // Initialize synapse weights.
   map<pair<string, string>, vector<NeuronHocDoc::Synapse *> >::iterator synitr;

   for (synitr = SimHocDoc->synapses.begin(); synitr != SimHocDoc->synapses.end(); ++synitr)
   {
	   sprintf(value, "%f", Randomizer->RAND_INTERVAL(MinWeight, MaxWeight));
	   for (i = 0, j = (int)synitr->second.size(); i < j; i++)
	   {
		   synapse = synitr->second[i];
		   values = synapse->values;
		   if (synapse->type == NeuronHocDoc::Synapse::CHEMICAL)
		   {
			   x = values[1].first;
			   y = values[1].second;
			   SimHocDoc->text[x][y] = value;
		   } else {
			   x = values[0].first;
			   y = values[0].second;
			   SimHocDoc->text[x][y] = value;
			   x = values[1].first;
			   y = values[1].second;
			   SimHocDoc->text[x][y] = value;
		   }
	   }
   }

   // Initialize stimuli parameters.
   map<string, pair<pair<int, int>, pair<int, int> > >::iterator stimitr;

   for (stimitr = SimHocDoc->stimuli.begin(); stimitr != SimHocDoc->stimuli.end(); ++stimitr)
   {
	   sprintf(value, "%f", Randomizer->RAND_INTERVAL(MinDelay, MaxDelay));
	   x = stimitr->second.first.first;
	   y = stimitr->second.first.second;
	   SimHocDoc->text[x][y] = value;
	   sprintf(value, "%f", Randomizer->RAND_INTERVAL(MinDuration, MaxDuration));
	   x = stimitr->second.second.first;
	   y = stimitr->second.second.second;
	   SimHocDoc->text[x][y] = value;
   }

   SimHocDoc->writeFile((char *)SimHocFile.c_str());

   delete SimHocDoc;
   delete Randomizer;

   return(0);
}
