// Network homomorph with NEURON simulator.

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
                             Random *randomizer, int tag = (-1));
   NeuronSimNetworkHomomorph(FilePointer *fp,
                             vector<vector<pair<int, int> > > *motorConnections,
                             Random *randomizer);

   // Destructor.
   ~NeuronSimNetworkHomomorph();

   // Mean absolute error (MAE).
   float meanError;

   // Optimize synapses.
   void optimize(int synapseOptimizedPathLength,
                 NeuronSim *modelSim, NeuronSim *evalSim);

   // NEURON simulation fitness evaluation.
   void evaluate(NeuronSim *modelSim, NeuronSim *evalSim);

   // Clone.
   NeuronSimNetworkHomomorph *clone(int tag = (-1));

   // Load.
   void load(FilePointer *fp);

   // Save.
   void save(FilePointer *fp);

   // Print.
   void print(bool printNetwork = false);
};
#endif
