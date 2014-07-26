// Network homomorph with NEURON network simulator.

#ifndef __NEURON_SIM_NETWORK_HOMOMORPH_HPP__
#define __NEURON_SIM_NETWORK_HOMOMORPH_HPP__

#include "networkHomomorph.hpp"
#include "neuronSim.hpp"

// Network homomorph with NEURON network simulator fitness evaluation.
class NeuronSimNetworkHomomorph : public NetworkHomomorph
{
public:

   // Constructors.
   NeuronSimNetworkHomomorph(Network *homomorph,
                             MutableParm& synapseWeightsParm,
                             vector<vector<pair<int, int> > > *motorConnections,
                             Random *randomizer, int tag = 0);
   NeuronSimNetworkHomomorph(FILE *fp,
                             vector<vector<pair<int, int> > > *motorConnections,
                             Random *randomizer);

   // Destructor.
   ~NeuronSimNetworkHomomorph();

   // Optimize synapses.
   void optimize(int synapseOptimizedPathLength,
                 NeuronSim *modelSim, NeuronSim *evalSim);

   // NEURON simulation fitness evaluation.
   void evaluate(NeuronSim *modelSim, NeuronSim *evalSim);

   // Clone.
   NeuronSimNetworkHomomorph *clone();

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);
};
#endif
