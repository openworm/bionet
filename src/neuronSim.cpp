// NEURON simulation.

#include "neuronSim.hpp"
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

// Constructor.
NeuronSim::NeuronSim(string neuronExecPath, string workDir, string hocFilename)
{
   char buf[BUFSIZ];

   this->neuronExecPath = neuronExecPath;
   this->workDir        = workDir;
   this->hocFilename    = hocFilename;
   sprintf(buf, "%s/%s", workDir.c_str(), (char *)hocFilename.c_str());
   hocDoc = new NeuronHocDoc(buf);
   assert(hocDoc != NULL);
   hocDoc->text[hocDoc->simReference.first][hocDoc->simReference.second] = ".";
   hocDoc->text[hocDoc->simsDir.first][hocDoc->simsDir.second]           = "./";
   hocDoc->writeFile(buf);
}


// Destructor.
NeuronSim::~NeuronSim()
{
   neuronActivations.clear();
   delete hocDoc;
}


// Export NEURON simulation synapses to network.
void NeuronSim::exportSynapses(Network *network)
{
   int i, j, k, n;

   pair<string, string>            key;
   string                          source, target, weight;
   vector<NeuronHocDoc::Synapse *> connections;
   NeuronHocDoc::Synapse           *connection;
   Synapse                         *synapse;

   n = network->numNeurons;
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         for (k = 0; k < (int)network->synapses[i][j].size(); k++)
         {
            delete network->synapses[i][j][k];
         }
         network->synapses[i][j].clear();
      }
   }

   for (map<pair<string, string>, vector<NeuronHocDoc::Synapse *> >::const_iterator itr =
           hocDoc->synapses.begin(); itr != hocDoc->synapses.end(); ++itr)
   {
      key         = itr->first;
      source      = key.first;
      target      = key.second;
      connections = itr->second;
      for (i = 0; i < n; i++)
      {
         if (network->neurons[i]->label == source)
         {
            break;
         }
      }
      assert(i < n);
      for (j = 0; j < n; j++)
      {
         if (network->neurons[j]->label == target)
         {
            break;
         }
      }
      assert(j < n);
      for (k = 0; k < (int)connections.size(); k++)
      {
         connection = connections[k];
         if (connection->type == NeuronHocDoc::Synapse::CHEMICAL)
         {
            weight  = hocDoc->text[connection->values[1].first][connection->values[1].second];
            synapse = new Synapse((float)atof(weight.c_str()), Synapse::CHEMICAL);
         }
         else
         {
            // Electrical (gap junction) synapse.
            weight  = hocDoc->text[connection->values[0].first][connection->values[0].second];
            synapse = new Synapse((float)atof(weight.c_str()), Synapse::ELECTRICAL);
         }
         assert(synapse != NULL);
         network->synapses[i][j].push_back(synapse);
      }
   }
}


// Import network synapse weights into NEURON simulation.
void NeuronSim::importSynapseWeights(Network *network)
{
   int  i, j, k, n;
   char weight[BUFSIZ];

   pair<string, string>            key;
   vector<NeuronHocDoc::Synapse *> connections;
   NeuronHocDoc::Synapse           *connection;
   Synapse *synapse;

   n = network->numNeurons;
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         key         = pair<string, string>(network->neurons[i]->label, network->neurons[j]->label);
         connections = hocDoc->synapses[key];
         for (k = 0; k < (int)connections.size(); k++)
         {
            connection = connections[k];
            synapse    = network->synapses[i][j][k];
            sprintf(weight, "%f", synapse->weight);
            if (connection->type == NeuronHocDoc::Synapse::CHEMICAL)
            {
               hocDoc->text[connection->values[1].first][connection->values[1].second] = weight;
            }
            else
            {
               // Electrical (gap junction) synapse.
               hocDoc->text[connection->values[0].first][connection->values[0].second] = weight;
               hocDoc->text[connection->values[1].first][connection->values[1].second] = weight;
            }
         }
      }
   }
}


// Run simulation.
void NeuronSim::run()
{
   int   i, j;
   char  buf[BUFSIZ];
   FILE  *fp;
   float activation;

   vector<float> activations;

   // Write hoc file.
   sprintf(buf, "%s/%s", workDir.c_str(), (char *)hocFilename.c_str());
   hocDoc->writeFile(buf);

   // Run NEURON simulator.
   sprintf(buf, "%s/time.dat", workDir.c_str());
#ifdef WIN32
   _unlink(buf);
   sprintf(buf, "cd %s & %s %s", workDir.c_str(), neuronExecPath.c_str(), hocFilename.c_str());
#else
   unlink(buf);
   sprintf(buf, "cd %s; %s %s", workDir.c_str(), neuronExecPath.c_str(), hocFilename.c_str());
#endif
   if (system(buf) != 0)
   {
      fprintf(stderr, "NEURON simulation failure\n");
      exit(1);
   }

   // Wait for neuron activation files.
   sprintf(buf, "%s/time.dat", workDir.c_str());
#ifdef WIN32
   struct _stat stbuf;
   for (i = 0; i < 10000 && _stat(buf, &stbuf) != 0; i++)
   {
      Sleep(100);
   }
#else
   struct stat stbuf;
   for (i = 0; i < 10000 && stat(buf, &stbuf) != 0; i++)
   {
      usleep(100 * 1000);
   }
#endif
   if (i == 10000)
   {
      fprintf(stderr, "NEURON simulation time out\n");
      exit(1);
   }

   // Import neuron activations.
   neuronActivations.clear();
   for (i = 0, j = (int)hocDoc->neurons.size(); i < j; i++)
   {
      sprintf(buf, "%s/%s_0.dat", workDir.c_str(), hocDoc->neurons[i].c_str());
      if ((fp = fopen(buf, "r")) != NULL)
      {
         activations.clear();
         while (fgets(buf, BUFSIZ, fp) != NULL)
         {
            activation = (float)atof(buf);
            activations.push_back(activation);
         }
         fclose(fp);
         neuronActivations[hocDoc->neurons[i]] = activations;
      }
   }
}


// Get activation difference between simulations.
float NeuronSim::activationDelta(NeuronSim *sim)
{
   int i, j, p, q;

   vector<float> *a1, *a2;
   float         delta = 0.0f;

   for (i = 0, j = (int)hocDoc->neurons.size(); i < j; i++)
   {
      a1 = &neuronActivations[hocDoc->neurons[i]];
      a2 = &sim->neuronActivations[hocDoc->neurons[i]];
      for (p = 0, q = (int)a1->size(); p < q; p++)
      {
         delta += fabs((*a1)[p] - (*a2)[p]);
      }
   }
   return(delta);
}
