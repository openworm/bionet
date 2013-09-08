// Neuron.

#include <math.h>
#include "network.hpp"
#include "neuron.hpp"
#include "fileio.h"

// Synapse constructors.
Synapse::Synapse(float weight)
{
   this->weight = weight;
   signal       = 0.0f;
}


Synapse::Synapse()
{
   this->weight = 0.0f;
   signal       = 0.0f;
}


// Load synapse.
void Synapse::load(FILE *fp)
{
   FREAD_FLOAT(&weight, fp);
   FREAD_FLOAT(&signal, fp);
}


// Save synapse.
void Synapse::save(FILE *fp)
{
   FWRITE_FLOAT(&weight, fp);
   FWRITE_FLOAT(&signal, fp);
}


// Print synapse.
void Synapse::print(bool terse, bool labels)
{
   if (terse)
   {
      if (labels)
      {
         printf("weight=%0.2f", weight);
      }
      else
      {
         printf("%0.2f", weight);
      }
   }
   else
   {
      if (labels)
      {
         printf("weight=%0.2f, signal=%0.2f", weight, signal);
      }
      else
      {
         printf("%0.2f\t%0.2f", weight, signal);
      }
   }
}


// Neuron constructor.
Neuron::Neuron(Network *network, int index,
               bool excitatory, ACTIVATION_FUNCTION function)
{
   this->network    = network;
   this->index      = index;
   this->excitatory = excitatory;
   this->function   = function;
   bias             = 0.0f;
   activation       = 0.0f;
}


// Fire neuron.
void Neuron::fire()
{
   Synapse *synapse;

   activation = bias;
   for (int i = 0, n = (int)network->synapses.size(); i < n; i++)
   {
      if ((synapse = network->synapses[i][index]) != NULL)
      {
         activation += synapse->signal * synapse->weight;
      }
   }

   switch (function)
   {
   case LINEAR:
      break;

   case LOGISTIC:
      activation = (activation * 8.0f) - 4.0f;
      activation = 1.0f / (1.0f + exp(float((-1.0f) * activation)));
      break;
   }
}


// Propagate activation to synapses.
void Neuron::propagate()
{
   Synapse *synapse;

   for (int i = 0, n = (int)network->synapses.size(); i < n; i++)
   {
      if ((synapse = network->synapses[index][i]) != NULL)
      {
         if (excitatory)
         {
            synapse->signal = activation;
         }
         else
         {
            synapse->signal = -activation;
         }
      }
   }
}


// Load neuron.
void Neuron::load(FILE *fp)
{
   int i;

   FREAD_INT(&index, fp);
   FREAD_BOOL(&excitatory, fp);
   FREAD_INT(&i, fp);
   switch (i)
   {
   case 0:
      function = LINEAR;
      break;

   case 1:
      function = LOGISTIC;
      break;
   }
   FREAD_FLOAT(&bias, fp);
   FREAD_FLOAT(&activation, fp);
}


// Save neuron.
void Neuron::save(FILE *fp)
{
   int i;

   FWRITE_INT(&index, fp);
   FWRITE_BOOL(&excitatory, fp);
   switch (function)
   {
   case LINEAR:
      i = 0;
      break;

   case LOGISTIC:
      i = 1;
      break;
   }
   FWRITE_INT(&i, fp);
   FWRITE_FLOAT(&bias, fp);
   FWRITE_FLOAT(&activation, fp);
}


// Print neuron.
void Neuron::print(bool terse, bool labels)
{
   if (terse)
   {
      if (labels)
      {
         printf("activation=%0.2f", activation);
      }
      else
      {
         printf("%0.2f", activation);
      }
   }
   else
   {
      if (labels)
      {
         printf("index=%d, ", index);
         printf("excitatory=");
         if (excitatory)
         {
            printf("true, ");
         }
         else
         {
            printf("false, ");
         }
         printf("function=");
         switch (function)
         {
         case LOGISTIC:
            printf("LOGISTIC, ");
            break;

         case LINEAR:
            printf("LINEAR, ");
            break;
         }
         printf("activation=%0.2f", activation);
      }
      else
      {
         printf("%d\t", index);
         if (excitatory)
         {
            printf("true");
         }
         else
         {
            printf("false");
         }
         printf("\t\t");
         switch (function)
         {
         case LOGISTIC:
            printf("LOGISTIC\t");
            break;

         case LINEAR:
            printf("LINEAR\t");
            break;
         }
         printf("%0.2f", activation);
      }
   }
}
