// Network homomorph with c302 simulator implementation.

#include "c302SimNetworkHomomorph.hpp"

// Constructors.
c302SimNetworkHomomorph::c302SimNetworkHomomorph(Network *homomorph,
                                                 MutableParm& synapseWeightsParm,
                                                 vector<vector<pair<int, int> > > *motorConnections,
                                                 Random *randomizer, int tag)
{
   this->synapseWeightsParm = synapseWeightsParm;
   this->synapseWeightsParm.initValue(randomizer);
   this->motorConnections = motorConnections;
   this->randomizer       = randomizer;
   this->tag = tag;
   network   = homomorph->clone();
   meanError = 0.0f;
   motorErrors.resize(network->numMotors, false);
}


c302SimNetworkHomomorph::c302SimNetworkHomomorph(FilePointer *fp,
                                                 vector<vector<pair<int, int> > > *motorConnections,
                                                 Random *randomizer)
{
   this->randomizer = randomizer;
   network          = NULL;
   load(fp);
   this->motorConnections = motorConnections;
}


// Destructor.
c302SimNetworkHomomorph::~c302SimNetworkHomomorph()
{
}


// Optimize synapses.
void c302SimNetworkHomomorph::optimize(int synapseOptimizedPathLength,
                                       c302Sim *modelSim, c302Sim *evalSim)
{
   int   i, j, k, n, p, q;
   float e, r;

   // Initialize optimization.
   vector<vector<Synapse *> > synapses;
   vector<vector<float> >     permutations;
   initOptimize(synapses, permutations, synapseOptimizedPathLength);

   // Hill-climb synapse weight permutations.
   n = 0;
   e = error;
   r = meanError;
   for (i = 1, j = (int)permutations.size(); i < j; i++)
   {
      for (k = 0; k < (int)synapses.size(); k++)
      {
         for (p = 0, q = (int)synapses[k].size(); p < q; p++)
         {
            synapses[k][p]->setWeight(permutations[i][k]);
         }
      }
      evaluate(modelSim, evalSim);
      if (error < e)
      {
         n = i;
         e = error;
         r = meanError;
      }
   }
   for (k = 0; k < (int)synapses.size(); k++)
   {
      for (p = 0, q = (int)synapses[k].size(); p < q; p++)
      {
         synapses[k][p]->setWeight(permutations[n][k]);
      }
   }
   error     = e;
   meanError = r;
}


// c302 simulation fitness evaluation.
void c302SimNetworkHomomorph::evaluate(c302Sim *modelSim, c302Sim *evalSim)
{
   evalSim->importSynapseWeights(network);
   evalSim->run();
   evalSim->activationDelta(modelSim, error, meanError);
}


// Clone.
c302SimNetworkHomomorph *c302SimNetworkHomomorph::clone(int tag)
{
   int i, n;
   c302SimNetworkHomomorph *simNetworkMorph;

   simNetworkMorph = new c302SimNetworkHomomorph(
      network, synapseWeightsParm,
      motorConnections, randomizer, tag);
   assert(simNetworkMorph != NULL);
   simNetworkMorph->error     = error;
   simNetworkMorph->meanError = meanError;
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      simNetworkMorph->motorErrors[i] = motorErrors[i];
   }
   return(simNetworkMorph);
}


// Load.
void c302SimNetworkHomomorph::load(FilePointer *fp)
{
   int  i, n;
   bool b;

   synapseWeightsParm.load(fp);
   if (network != NULL)
   {
      delete network;
   }
   network = new Network(fp);
   assert(network != NULL);
   FREAD_INT(&tag, fp);
   FREAD_FLOAT(&error, fp);
   FREAD_FLOAT(&meanError, fp);
   n = (int)network->numMotors;
   motorErrors.resize(n, false);
   for (i = 0; i < n; i++)
   {
      FREAD_BOOL(&b, fp);
      motorErrors[i] = b;
   }
   FREAD_INT(&offspringCount, fp);
}


// Save.
void c302SimNetworkHomomorph::save(FilePointer *fp)
{
   int  i, n;
   bool b;

   synapseWeightsParm.save(fp);
   network->save(fp);
   FWRITE_INT(&tag, fp);
   FWRITE_FLOAT(&error, fp);
   FWRITE_FLOAT(&meanError, fp);
   n = (int)motorErrors.size();
   for (i = 0; i < n; i++)
   {
      b = motorErrors[i];
      FWRITE_BOOL(&b, fp);
   }
   FWRITE_INT(&offspringCount, fp);
}


// Print.
void c302SimNetworkHomomorph::print(bool printNetwork)
{
   int i, n;

   if (printNetwork)
   {
      printf("Network:\n");
      if (network != NULL)
      {
         network->print();
      }
      else
      {
         printf("NULL\n");
      }
   }
   printf("tag=%d\n", tag);
   printf("error=%f\n", error);
   printf("meanError=%f\n", error);
   printf("motorErrors: ");
   for (i = 0, n = (int)motorErrors.size(); i < n; i++)
   {
      if (motorErrors[i])
      {
         printf("%d ", i);
      }
   }
   printf("\n");
   printf("offspringCount=%d\n", offspringCount);
   printf("synapseWeightsParm:\n");
   synapseWeightsParm.print();
}
