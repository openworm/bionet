// NEURON simulation.

#ifndef __NEURON_SIM_HPP__
#define __NEURON_SIM_HPP__

#include "neuronHocDoc.hpp"
#include "network.hpp"
#include <string>
#include <vector>
#include <map>
using namespace std;

class NeuronSim
{
public:

   // NEURON executable path.
   string neuronExecPath;

   // Working directory.
   string workDir;

   // NEURON hoc file and document.
   string       hocFilename;
   NeuronHocDoc *hocDoc;

   // Neuron activations.
   // Key: neuron name.
   // Value: activation sequence.
   map<string, vector<float> > neuronActivations;

   // Constructor.
   NeuronSim(string neuronExecPath, string workDir, string hocFilename);

   // Destructor.
   ~NeuronSim();

   // Export NEURON simulation synapses to network.
   void exportSynapses(Network *network);

   // Import network synapse weights into NEURON simulation.
   void importSynapseWeights(Network *network);

   // Run simulation.
   void run();

   // Get activation difference between simulations.
   float activationDelta(NeuronSim *sim);
};
#endif
