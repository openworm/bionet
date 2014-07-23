// Network morph.

#ifndef __NETWORK_MORPH_HPP__
#define __NETWORK_MORPH_HPP__

#include "network.hpp"
#include "behavior.hpp"
#include "mutableParm.hpp"

// Network morph.
class NetworkMorph
{
public:

   // Maximum tolerated motor output error.
   static const float MAX_ERROR_TOLERANCE;

   // Constructor.
   NetworkMorph();

   Network      *network;
   int          tag;
   float        error;
   vector<bool> motorErrors;
   bool         behaves;
   int          offspringCount;

   // Evaluate behavior.
   void evaluate(vector<Behavior *>& behaviors, int maxStep);
   void evaluate(vector<Behavior *>& behaviors,
                 vector<bool>& fitnessMotorList, int maxStep);

protected:

   Random *randomizer;
};
#endif
