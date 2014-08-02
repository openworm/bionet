// An isomorphic network reproduces the sensory-motor behaviors of another network.

#ifndef __NETWORK_ISOMORPH_HPP__
#define __NETWORK_ISOMORPH_HPP__

#include "networkMorph.hpp"

// Network isomorph.
class NetworkIsomorph : public NetworkMorph
{
public:

   // Constructors.
   NetworkIsomorph(MutableParm& excitatoryNeuronsParm, MutableParm& inhibitoryNeuronsParm,
                   MutableParm& synapsePropensitiesParm, MutableParm& synapseWeightsParm,
                   int numSensors, int numMotors, Random *randomizer, int tag = (-1));
   NetworkIsomorph(FILE *fp, Random *randomizer);

   // Destructor.
   ~NetworkIsomorph();

   MutableParm excitatoryNeuronsParm;
   MutableParm inhibitoryNeuronsParm;
   MutableParm synapsePropensitiesParm;
   MutableParm synapseWeightsParm;

   // Mutate.
   void mutate();

   // Clone.
   NetworkIsomorph *clone(int tag = (-1));

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print(bool printNetwork = false);

private:

   void deleteIndexedNeuron(int index);
   void addIndexedNeuron(int index, bool excitatory);
};
#endif
