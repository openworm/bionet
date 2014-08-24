// c302 simulation.

#ifndef __C302_SIM_HPP__
#define __C302_SIM_HPP__

#include "network.hpp"
#include <string>
#include <vector>
#include <map>
using namespace std;

class c302Sim
{
public:

   // jnml command path.
   string jnmlCmdPath;

   // Working directory.
   string workDir;

   /*
    * Network configuration file name with format:
    * Cells:
    * <neuron name>
    * ...
    * Synapses:
    * <source neuron> <target neuron> <weight>
    * (Example: "ADAL AIBL 0.5")
    * ...
    * Stimuli:
    * <neuron name>
    * ...
    */
   static const string NetworkFileName;

   // Simulation name string.
   static const string SimulationStr;

   // Neurons.
   vector<string> neurons;

   // Synapses:
   // Key: source/target neuron.
   // Value: weight.
   map<pair<string, string>, float> synapses;

   // Stimuli.
   vector<string> stimuli;

   // Neuron activations.
   // Key: neuron name.
   // Value: activation sequence.
   map<string, vector<float> > neuronActivations;

   // Constructor.
   c302Sim(string neuronExecPath, string workDir);

   // Destructor.
   ~c302Sim();

   // Export c302 simulation synapses to network.
   void exportSynapses(Network *network);

   // Import network synapse weights into c302 simulation.
   void importSynapseWeights(Network *network);

   // Run simulation.
   bool run();

   // Get activation difference between simulations.
   void activationDelta(c302Sim *sim, float& total, float& average);

private:

   // Parse network file.
   bool parseNetworkFile(char *filename);
};
#endif
