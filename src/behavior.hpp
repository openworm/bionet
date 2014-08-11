// Sensory-motor behavior.

#ifndef __BEHAVIOR_HPP__
#define __BEHAVIOR_HPP__

#include <stdlib.h>
#include <vector>
#include "network.hpp"
using namespace std;

class Behavior
{
public:
   Behavior(Network *network, int length, Random *randomizer);
   Behavior(Network *network, vector<vector<float> >& sensors);
   Behavior() {}
   Behavior(FilePointer *fp);
   ~Behavior();

   // Sensory-motor sequence.
   vector<vector<float> > sensorSequence;
   vector<vector<float> > motorSequence;

   // Load.
   void load(FilePointer *fp);

   // Save.
   void save(FilePointer *fp);

   // Print.
   void print();

   // Print motor deltas.
   void printMotorDeltas(Behavior *behavior, float tolerance = 0.0f);

   // Load behaviors from file.
   static bool loadBehaviors(vector<Behavior *>& behaviors, char *filename, bool binary = false);

   // Save behaviors to file.
   static bool saveBehaviors(vector<Behavior *>& behaviors, char *filename, bool binary = false);
};
#endif
