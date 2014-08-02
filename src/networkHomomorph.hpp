// A homomorphic network reproduces the sensory-motor behaviors of another network
// while mirroring its neuron configuration.

#ifndef __NETWORK_HOMOMORPH_HPP__
#define __NETWORK_HOMOMORPH_HPP__

#include "networkMorph.hpp"

// Network homomorph.
class NetworkHomomorph : public NetworkMorph
{
public:

   // Constructors.
   NetworkHomomorph(Network *homomorph,
                    MutableParm& synapseWeightsParm,
                    vector<vector<pair<int, int> > > *motorConnections,
                    Random *randomizer, int tag = (-1));
   NetworkHomomorph();
   NetworkHomomorph(FILE *fp,
                    vector<vector<pair<int, int> > > *motorConnections,
                    Random *randomizer);

   // Destructor.
   ~NetworkHomomorph();

   // Synapse weights parameter.
   MutableParm synapseWeightsParm;

   // Mutate synapses.
   void mutate();

   // Optimize synapses.
   void optimize(vector<Behavior *>& behaviors, vector<bool>& fitnessMotorList,
                 int synapseOptimizedPathLength, int maxStep);

   // Clone.
   NetworkHomomorph *clone(int tag = (-1));

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);

   // Neurons connected to motors (indices).
   vector<vector<pair<int, int> > > *motorConnections;

protected:

   // Initialize synapse optimization.
   void initOptimize(vector<vector<Synapse *> >& synapses,
                     vector<vector<float> >& permutations, int synapseOptimizedPathLength);

   // Select random neuron.
   int randomNeuron(bool nonMotor = false);

   // Permute weights.
   void permuteWeights(vector<vector<float> >& weightRanges,
                       vector<vector<float> >& permutations,
                       vector<float>& permutation,
                       int level, int depth);
};
#endif
