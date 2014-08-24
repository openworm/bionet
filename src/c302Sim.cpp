// c302 simulation.

#include "c302Sim.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

// Network configuration file name.
const string c302Sim::NetworkFileName = "c302_bionet.txt";

// Simulation name string.
const string c302Sim::SimulationStr = "c302_A_bionet";

// Constructor.
c302Sim::c302Sim(string jnmlCmdPath, string workDir)
{
   char buf[BUFSIZ];

   this->jnmlCmdPath = jnmlCmdPath;
   this->workDir     = workDir;
   sprintf(buf, "%s/%s", workDir.c_str(), NetworkFileName.c_str());
   bool result = parseNetworkFile(buf);
   assert(result);
}


// Destructor.
c302Sim::~c302Sim()
{
   neurons.clear();
   synapses.clear();
   stimuli.clear();
   neuronActivations.clear();
}


// Export c302 simulation synapses to network.
void c302Sim::exportSynapses(Network *network)
{
   int i, j, k, n;

   pair<string, string> key;
   string               source, target;
   float                weight;
   map<pair<string, string>, float>::const_iterator itr;

   n = network->numNeurons;
   for (i = 0; i < n; i++)
   {
      source = network->neurons[i]->label;
      for (j = 0; j < n; j++)
      {
         target = network->neurons[j]->label;
         itr    = synapses.find(pair<string, string>(source, target));
         if (itr == synapses.end())
         {
            for (k = 0; k < (int)network->synapses[i][j].size(); k++)
            {
               delete network->synapses[i][j][k];
            }
            network->synapses[i][j].clear();
         }
         else
         {
            weight = itr->second;
            for (k = 0; k < (int)network->synapses[i][j].size(); k++)
            {
               network->synapses[i][j][k]->weight = weight;
            }
         }
      }
   }
}


// Import network synapse weights into c302 simulation.
void c302Sim::importSynapseWeights(Network *network)
{
   int   i, j, n;
   float weight;

   pair<string, string> key;

   n = network->numNeurons;
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         if (network->synapses[i][j].size() > 0)
         {
            weight        = network->synapses[i][j][0]->weight;
            key           = pair<string, string>(network->neurons[i]->label, network->neurons[j]->label);
            synapses[key] = weight;
         }
      }
   }
}


// Run simulation.
bool c302Sim::run()
{
   int  i, j, o;
   char scriptName[50], lemsName[50], datPath[BUFSIZ + 1], buf[BUFSIZ + 1], *s;
   FILE *fp;

   map<pair<string, string>, float>::iterator itr;
   float         activation;
   vector<float> activations;

   // Write python script.
   sprintf(scriptName, "%s.py", SimulationStr.c_str());
   sprintf(buf, "%s/%s", workDir.c_str(), scriptName);
   if ((fp = fopen(buf, "wb")) == NULL)
   {
      return(false);
   }
   fprintf(fp, "from c302 import generate\n\n");
   fprintf(fp, "import parameters_A as params\n\n");
   fprintf(fp, "if __name__ == '__main__':\n\n");
   fprintf(fp, "   cells = [");
   for (i = 0, j = (int)neurons.size(); i < j; i++)
   {
      fprintf(fp, "\"%s\"", neurons[i].c_str());
      if (i < (j - 1))
      {
         fprintf(fp, ", ");
      }
   }
   fprintf(fp, "]\n");
   fprintf(fp, "   cells_to_stimulate = [");
   for (i = 0, j = (int)stimuli.size(); i < j; i++)
   {
      fprintf(fp, "\"%s\"", stimuli[i].c_str());
      if (i < (j - 1))
      {
         fprintf(fp, ", ");
      }
   }
   fprintf(fp, "]\n\n");
   fprintf(fp, "   scaled_conn_numbers = {");
   i = 0;
   for (itr = synapses.begin(); itr != synapses.end(); ++itr)
   {
      if (i == 1)
      {
         fprintf(fp, ", ");
      }
      i = 1;
      fprintf(fp, "\"%s-%s\":%0.2f", itr->first.first.c_str(), itr->first.second.c_str(), itr->second);
   }
   fprintf(fp, "}\n\n");
   fprintf(fp, "   generate(\"%s\", params, cells = cells, cells_to_stimulate = cells_to_stimulate, conn_number_scaling = scaled_conn_numbers, duration = 500, dt = 0.1, vmin = -72, vmax = -48)\n", SimulationStr.c_str());
   fclose(fp);

   // Run python script to build jnml files.
#ifdef WIN32
   sprintf(buf, "cd %s & python %s", workDir.c_str(), scriptName);
#else
   sprintf(buf, "cd %s; python %s >/dev/null 2>&1", workDir.c_str(), scriptName);
#endif
   if (system(buf) != 0)
   {
      fprintf(stderr, "Cannot create jnml files\n");
      exit(1);
   }

   // Run jnml simulation.
   sprintf(datPath, "%s/%s.dat", workDir.c_str(), SimulationStr.c_str());
   sprintf(lemsName, "LEMS_%s.xml", SimulationStr.c_str());
#ifdef WIN32
   _unlink(datPath);
   sprintf(buf, "cd %s & %s %s -nogui", workDir.c_str(), jnmlCmdPath.c_str(), lemsName);
#else
   unlink(datPath);
   sprintf(buf, "cd %s; %s %s -nogui >/dev/null 2>&1", workDir.c_str(), jnmlCmdPath.c_str(), lemsName);
#endif
   if (system(buf) != 0)
   {
      fprintf(stderr, "jnml failure\n");
      exit(1);
   }

   // Wait for activation file to be written.
#ifdef WIN32
   struct _stat stbuf;
   for (i = 0; i < 10000 && _stat(datPath, &stbuf) != 0; i++)
   {
      Sleep(100);
   }
#else
   struct stat stbuf;
   for (i = 0; i < 10000 && stat(datPath, &stbuf) != 0; i++)
   {
      usleep(100 * 1000);
   }
#endif
   if (i == 10000)
   {
      fprintf(stderr, "jnml simulation time out\n");
      exit(1);
   }
#ifdef WIN32
   Sleep(2000);
#else
   usleep(100 * 1000 * 20);
#endif

   // Import neuron activations.
   neuronActivations.clear();
   if ((fp = fopen(datPath, "r")) == NULL)
   {
      fprintf(stderr, "Cannot open data file %s\n", datPath);
      exit(1);
   }
   for (i = 0, j = (int)neurons.size(); i < j; i++)
   {
      neuronActivations[neurons[i]] = activations;
   }
   while (fgets(buf, BUFSIZ, fp) != NULL)
   {
      s = buf;
      sscanf(s, "%f%n", &activation, &o);
      s += o;
      for (i = 0; i < j; i++)
      {
         sscanf(s, "%f%n", &activation, &o);
         s += o;
         neuronActivations[neurons[i]].push_back(activation);
      }
   }
   fclose(fp);
   return(true);
}


// Get activation difference between simulations.
void c302Sim::activationDelta(c302Sim *sim, float& total, float& average)
{
   int i, j, p, q, n;

   vector<float> *a1, *a2;
   float         f1, f2;

   total = average = 0.0f;
   for (i = n = 0, j = (int)neurons.size(); i < j; i++)
   {
      a1 = &neuronActivations[neurons[i]];
      a2 = &sim->neuronActivations[neurons[i]];
      for (p = 0, q = (int)a1->size(); p < q; p++)
      {
         f1     = (*a1)[p];
         f2     = (*a2)[p];
         total += fabs(f1 - f2);
         n++;
      }
   }
   if (n > 0)
   {
      average = total / (float)n;
   }
}


// Parse network file.
bool c302Sim::parseNetworkFile(char *filename)
{
   ifstream inFile;
   string   strLine;

   enum
   {
      CELLS, SYNAPSES, STIMULI
   }
        state = CELLS;
   char buf[BUFSIZ + 1], n1[50], n2[50], w[50];

   neurons.clear();
   synapses.clear();
   stimuli.clear();

   inFile.open(filename);
   if (!inFile.is_open())
   {
      return(false);
   }

   while (inFile)
   {
      getline(inFile, strLine);
      memset(buf, 0, BUFSIZ + 1);
      strncpy(buf, strLine.c_str(), BUFSIZ);

      if (strstr(buf, "Cells:") != NULL)
      {
         state = CELLS;
      }
      else if (strstr(buf, "Synapses:") != NULL)
      {
         state = SYNAPSES;
      }
      else if (strstr(buf, "Stimuli:") != NULL)
      {
         state = STIMULI;
      }
      else
      {
         switch (state)
         {
         case CELLS:
            sscanf(buf, "%s", n1);
            neurons.push_back(n1);
            break;

         case SYNAPSES:
            sscanf(buf, "%s %s %s", n1, n2, w);
            synapses[pair < string, string > (n1, n2)] = (float)atof(w);
            break;

         case STIMULI:
            sscanf(buf, "%s", n1);
            stimuli.push_back(n1);
            break;
         }
      }
   }
   inFile.close();
   sort(neurons.begin(), neurons.end());
   sort(stimuli.begin(), stimuli.end());
   return(true);
}
