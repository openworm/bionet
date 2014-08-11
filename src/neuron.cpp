// Neuron.

#include "network.hpp"
#include "neuron.hpp"
#include "fileio.h"

// Synapse constructors.
Synapse::Synapse(float weight, TYPE type)
{
   setWeight(weight);
   this->type = type;
   signal     = 0.0f;
}


Synapse::Synapse()
{
   weight = 0.0f;
   type   = CHEMICAL;
   signal = 0.0f;
}


float Synapse::WEIGHT_DECIMAL_QUANTIZER = -1.0f;

// Set weight.
void Synapse::setWeight(float weight)
{
   // Quantize decimals?
   if (WEIGHT_DECIMAL_QUANTIZER > 0.0f)
   {
      this->weight = (float)((int)(weight / WEIGHT_DECIMAL_QUANTIZER)) * WEIGHT_DECIMAL_QUANTIZER;
   }
   else if (WEIGHT_DECIMAL_QUANTIZER == 0.0f)
   {
      this->weight = (float)((int)weight);
   }
   else
   {
      this->weight = weight;
   }
}


// Load synapse.
void Synapse::load(FilePointer *fp)
{
   int  t;
   char buf[BUFSIZ];

   FREAD_FLOAT(&weight, fp);
   FREAD_INT(&t, fp);
   switch (t)
   {
   case 0:
      type = CHEMICAL;
      break;

   case 1:
      type = ELECTRICAL;
      break;

   case 2:
      type = UNKNOWN;
      break;
   }
   FREAD_FLOAT(&signal, fp);
   FREAD_STRING(buf, BUFSIZ, fp);
   label = buf;
}


// Save synapse.
void Synapse::save(FilePointer *fp)
{
   int  t;
   char buf[BUFSIZ];

   FWRITE_FLOAT(&weight, fp);
   switch (type)
   {
   case CHEMICAL:
      t = 0;
      break;

   case ELECTRICAL:
      t = 1;
      break;

   case UNKNOWN:
      t = 2;
      break;
   }
   FWRITE_INT(&t, fp);
   FWRITE_FLOAT(&signal, fp);
   strncpy(buf, label.c_str(), BUFSIZ);
   FWRITE_STRING(buf, BUFSIZ, fp);
}


// Print synapse.
void Synapse::print(bool terse, bool labels)
{
   string t;

   if (terse)
   {
      if (labels)
      {
         printf("weight=%0.2f, label=%s", weight, label.c_str());
      }
      else
      {
         printf("%0.2f", weight);
      }
   }
   else
   {
      switch (type)
      {
      case CHEMICAL:
         t = "chemical";
         break;

      case ELECTRICAL:
         t = "electrical";
         break;

      case UNKNOWN:
         t = "unknown";
         break;
      }
      if (labels)
      {
         printf("weight=%0.2f, type=%s, signal=%0.2f, label=%s", weight, t.c_str(), signal, label.c_str());
      }
      else
      {
         printf("%0.2f\t%s\t%0.2f", weight, t.c_str(), signal);
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
      for (int j = 0, k = (int)network->synapses[i][index].size(); j < k; j++)
      {
         synapse     = network->synapses[i][index][j];
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
      for (int j = 0, k = (int)network->synapses[index][i].size(); j < k; j++)
      {
         synapse = network->synapses[index][i][j];
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
void Neuron::load(FilePointer *fp)
{
   int  i;
   char buf[BUFSIZ];

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
   FREAD_STRING(buf, BUFSIZ, fp);
   label = buf;
}


// Save neuron.
void Neuron::save(FilePointer *fp)
{
   int  i;
   char buf[BUFSIZ];

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
   strncpy(buf, label.c_str(), BUFSIZ);
   FWRITE_STRING(buf, BUFSIZ, fp);
}


// Print neuron.
void Neuron::print(bool terse, bool labels)
{
   if (terse)
   {
      if (labels)
      {
         printf("activation=%0.2f, label=%s", activation, label.c_str());
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
         printf("activation=%0.2f, ", activation);
         printf("label=%s", label.c_str());
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
