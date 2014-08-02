// Neuron.

#ifndef __NEURON_HPP__
#define __NEURON_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <vector>
#include <string>
using namespace std;

class Network;
class Neuron;

class Synapse
{
public:
   enum TYPE { CHEMICAL, ELECTRICAL, UNKNOWN };

   Synapse(float weight, TYPE type = UNKNOWN);
   Synapse();

   float  weight;
   TYPE   type;
   float  signal;
   string label;

   void load(FILE *fp);
   void save(FILE *fp);
   void print(bool terse = true, bool labels = false);

   // Weight quantization: .1 for one decimal digit, .01 for two digits, etc.
   // Noop: -1.0
   static float WEIGHT_DECIMAL_QUANTIZER;
   void setWeight(float weight);
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
   string              label;

   void fire();
   void propagate();
   void load(FILE *fp);
   void save(FILE *fp);
   void print(bool terse = true, bool labels = false);
};
#endif
