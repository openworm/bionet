// Neural network.

#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <stdlib.h>
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

   // save network.
   bool save(char *filename);
   void save(FILE *fp);

   // Print network.
   void print();

   // Dump network graph in 'dot' format.
   bool dumpGraph(char *title = NULL, char *filename = NULL);

   // Are neurons connected to sensors and motors?
   bool isConnected();

private:

   // Are neurons connected?
   bool isConnected(vector<bool>& connectedNeurons, bool toSensor);
   void connect(int index, vector<bool>& connectedNeurons, bool toSensor);
};
#endif
