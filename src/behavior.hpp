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
   Behavior(FILE *fp);
   ~Behavior();

   // Sensory-motor sequence.
   vector<vector<float> > sensorSequence;
   vector<vector<float> > motorSequence;

   // Load.
   void load(FILE *fp);

   // Save.
   void save(FILE *fp);

   // Print.
   void print();

   // Print motor deltas.
   void printMotorDeltas(Behavior *behavior, float tolerance = 0.0f);

   // Load behaviors from file.
   static bool loadBehaviors(char *filename, vector<Behavior *>& behaviors);

   // Save behaviors to file.
   static bool saveBehaviors(char *filename, vector<Behavior *>& behaviors);
};
#endif
