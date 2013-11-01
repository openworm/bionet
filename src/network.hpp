// Neural network.

#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <stdlib.h>
#include <queue>
#include <vector>
#include "neuron.hpp"
#include "random.hpp"
using namespace std;

class Network
{
public:
   Network(int numNeurons, int numSensors, int numMotors,
           float inhibitorDensity = DEFAULT_INHIBITOR_DENSITY,
           float synapsePropensity = DEFAULT_SYNAPSE_PROPENSITY,
           float minSynapseWeight = 0.0f,
           float maxSynapseWeight = 1.0f,
           RANDOM randomSeed = DEFAULT_RANDOM_SEED);

   Network(char *filename);
   Network(FILE *fp);
   ~Network();

   // Default inhibitory neuron density.
   static const float DEFAULT_INHIBITOR_DENSITY;

   // Default synapse propensity.
   static const float DEFAULT_SYNAPSE_PROPENSITY;

   // Default random seed.
   static const RANDOM DEFAULT_RANDOM_SEED;

   // Neurons.
   int              numNeurons, numSensors, numMotors;
   vector<Neuron *> neurons;

   // Synapse connection matrix.
   vector<vector<Synapse *> > synapses;

   // Clone network.
   Network *clone();

   // Clear network.
   void clear();

   // Step network.
   void step();

   // Load network.
   bool load(char *filename);
   void load(FILE *fp);

   // Save network.
   bool save(char *filename);
   void save(FILE *fp);

   // Print network.
   void print(bool network = true, bool connectivity = false);

   // Network metrics:
   struct SynapseMetrics
   {
      int   total;
      // Per neuron:
      int   minimum;
      int   maximum;
      int   median;
      float mean;
   };
   struct ConnectionMetrics
   {
      int   minimum;
      int   maximum;
      int   median;
      float mean;
   };
   struct PathLengthMetrics
   {
      int   minimum;
      int   maximum;
      int   median;
      float mean;
      float shortestMean;
   };
   void getMetrics(struct SynapseMetrics&    synapseMetrics,
                   struct ConnectionMetrics& sensorsToMotorsConnectionMetrics,
                   struct PathLengthMetrics& sensorsToMotorsPathLengthMetrics,
                   struct ConnectionMetrics& motorsToSensorsConnectionMetrics,
                   struct PathLengthMetrics& motorsToSensorsPathLengthMetrics);

   // Dump network graph in 'dot' format.
   bool dumpGraph(char *title = NULL, char *filename = NULL);

   // Are neurons connected to sensors and motors?
   bool isConnected();

private:

   // Are neurons connected?
   bool isConnected(vector<bool>& connectedNeurons, bool toSensor);
   void connect(int index, vector<bool>& connectedNeurons, bool toSensor);

   // Breadth-first visit motor/sensor endpoints.
   void visitEndpoints(queue<pair<Neuron *, int> >& open,
                       vector<Neuron *>& closed,
                       vector<pair<Neuron *, vector<int> *> >& endpoints,
                       bool motorEndpoints);

   static bool compareNeurons(pair<Neuron *, vector<int> *> a, pair<Neuron *, vector<int> *> b)
   {
      if (a.first->label.empty() || b.first->label.empty())
      {
         return(a.first->index < b.first->index);
      }
      else
      {
         return(a.first->label < b.first->label);
      }
   }
};
#endif
