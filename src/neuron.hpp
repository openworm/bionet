// Neuron.

#ifndef __NEURON_HPP__
#define __NEURON_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

class Network;
class Neuron;

class Synapse
{
public:
   Synapse(float weight);
   Synapse();

   float weight;
   float signal;

   void load(FILE *fp);
   void save(FILE *fp);
   void print(bool terse = true, bool labels = false);
};

class Neuron
{
public:
   enum ACTIVATION_FUNCTION { LINEAR, LOGISTIC };

   Neuron(Network *network, int index = (-1),
          bool excitatory = true, ACTIVATION_FUNCTION function = LOGISTIC);

   Network             *network;
   int                 index;
   bool                excitatory;
   ACTIVATION_FUNCTION function;
   float               bias;
   float               activation;

   void fire();
   void propagate();
   void load(FILE *fp);
   void save(FILE *fp);
   void print(bool terse = true, bool labels = false);
};
#endif
