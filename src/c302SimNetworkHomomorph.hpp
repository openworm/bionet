// Network homomorph with c302 simulator.

#ifndef __C302_SIM_NETWORK_HOMOMORPH_HPP__
#define __C302_SIM_NETWORK_HOMOMORPH_HPP__

#include "networkHomomorph.hpp"
#include "c302Sim.hpp"

// Network homomorph with c302 simulator fitness evaluation.
class c302SimNetworkHomomorph : public NetworkHomomorph
{
public:

   // Constructors.
   c302SimNetworkHomomorph(Network *homomorph,
                           MutableParm& synapseWeightsParm,
                           vector<vector<pair<int, int> > > *motorConnections,
                           Random *randomizer, int tag = (-1));
   c302SimNetworkHomomorph(FilePointer *fp,
                           vector<vector<pair<int, int> > > *motorConnections,
                           Random *randomizer);

   // Destructor.
   ~c302SimNetworkHomomorph();

   // Mean absolute error (MAE).
   float meanError;

   // Optimize synapses.
   void optimize(int synapseOptimizedPathLength,
                 c302Sim *modelSim, c302Sim *evalSim);

   // c302 simulation fitness evaluation.
   void evaluate(c302Sim *modelSim, c302Sim *evalSim);

   // Clone.
   c302SimNetworkHomomorph *clone(int tag = (-1));

   // Load.
   void load(FilePointer *fp);

   // Save.
   void save(FilePointer *fp);

   // Print.
   void print(bool printNetwork = false);
};
#endif
