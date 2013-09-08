// Neural network.

#include "network.hpp"

// Default parameters.
const float Network:: DEFAULT_INHIBITOR_DENSITY  = 0.25f;
const float Network:: DEFAULT_SYNAPSE_PROPENSITY = 0.1f;
const RANDOM Network::DEFAULT_RANDOM_SEED        = 4517;

// Constructors.
Network::Network(int numNeurons, int numSensors, int numMotors,
                 float inhibitorDensity, float synapsePropensity,
                 RANDOM randomSeed)
{
   int    i, j, n;
   float  weight;
   Neuron *neuron;

   vector<bool> sensorConnected;
   vector<bool> motorConnected;

   assert(numSensors > 0);
   assert(numMotors > 0);
   assert(numNeurons >= (numSensors + numMotors));
   assert(synapsePropensity > 0.0f);

   this->numNeurons = numNeurons;
   this->numSensors = numSensors;
   this->numMotors  = numMotors;
   n = numSensors + numMotors;
   this->randomSeed = randomSeed;
   randomizer       = new Random(randomSeed);

   // Add neurons.
   for (i = 0; i < numNeurons; i++)
   {
      if (i < numSensors)
      {
         // Sensor.
         neuron = new Neuron(this, i, true);
      }
      else if (i < n)
      {
         // Motor.
         neuron = new Neuron(this, i, true);
      }
      else
      {
         // Interneuron.
         if (randomizer->RAND_CHANCE(inhibitorDensity))
         {
            neuron = new Neuron(this, i, false);
         }
         else
         {
            neuron = new Neuron(this, i, true);
         }
      }
      assert(neuron != NULL);
      neurons.push_back(neuron);
   }

   // Add synapses until all neurons connected to sensors and motors.
   synapses.resize(numNeurons);
   for (i = 0; i < numNeurons; i++)
   {
      synapses[i].resize(numNeurons);
      for (j = 0; j < numNeurons; j++)
      {
         synapses[i][j] = NULL;
      }
   }
   sensorConnected.resize(numNeurons, false);
   for (i = 0; i < numSensors; i++)
   {
      sensorConnected[i] = true;
   }
   motorConnected.resize(numNeurons, false);
   for (i = numSensors; i < n; i++)
   {
      motorConnected[i] = true;
   }
   for (i = 0; i < numNeurons; i++)
   {
      for (j = 0; j < numNeurons; j++)
      {
         if (i == j)
         {
            continue;
         }
         if ((numNeurons > n) && (i < numSensors) && (j >= numSensors) && (j < n))
         {
            continue;
         }
         if (synapses[i][j] == NULL)
         {
            if (((i < numSensors) || (i >= n)) && (j >= numSensors))
            {
               if (randomizer->RAND_CHANCE(synapsePropensity))
               {
                  weight         = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
                  synapses[i][j] = new Synapse(weight);
                  assert(synapses[i][j] != NULL);
               }
            }
         }
      }
   }
   while (!isConnected(sensorConnected, true) || !isConnected(motorConnected, false))
   {
      i = randomizer->RAND_CHOICE(numNeurons);
      j = randomizer->RAND_CHOICE(numNeurons);
      if (i == j)
      {
         continue;
      }
      if ((numNeurons > n) && (i < numSensors) && (j >= numSensors) && (j < n))
      {
         continue;
      }
      if (synapses[i][j] == NULL)
      {
         if (((i < numSensors) || (i >= n)) && (j >= numSensors))
         {
            weight         = (float)randomizer->RAND_INTERVAL(0.0, 1.0);
            synapses[i][j] = new Synapse(weight);
            assert(synapses[i][j] != NULL);
         }
      }
   }
}


Network::Network(char *filename)
{
   neurons.clear();
   synapses.clear();
   randomizer = NULL;
   load(filename);
}


Network::Network(FILE *fp)
{
   neurons.clear();
   synapses.clear();
   randomizer = NULL;
   load(fp);
}


// Are neurons connected to sensors and motors?
bool Network::isConnected()
{
   int i, n;

   vector<bool> sensorConnected;
   vector<bool> motorConnected;

   sensorConnected.resize(numNeurons, false);
   for (i = 0; i < numSensors; i++)
   {
      sensorConnected[i] = true;
   }
   motorConnected.resize(numNeurons, false);
   for (i = numSensors, n = numSensors + numMotors; i < n; i++)
   {
      motorConnected[i] = true;
   }

   if (isConnected(sensorConnected, true) && isConnected(motorConnected, false))
   {
      return(true);
   }
   else
   {
      return(false);
   }
}


// Are neurons connected to sensors/motors?
bool Network::isConnected(vector<bool>& connectedNeurons, bool toSensor)
{
   int  i, j;
   bool done;

   if (toSensor)
   {
      done = false;
      while (!done)
      {
         done = true;
         for (i = numSensors; i < numNeurons; i++)
         {
            if (!connectedNeurons[i])
            {
               for (j = 0; j < numNeurons; j++)
               {
                  if ((i != j) && (synapses[j][i] != NULL))
                  {
                     if (connectedNeurons[j])
                     {
                        connect(i, connectedNeurons, toSensor);
                        done = false;
                     }
                  }
               }
            }
         }
      }
   }
   else
   {
      done = false;
      while (!done)
      {
         done = true;
         for (i = 0; i < numNeurons; i++)
         {
            if (!connectedNeurons[i])
            {
               for (j = 0; j < numNeurons; j++)
               {
                  if ((i != j) && (synapses[i][j] != NULL))
                  {
                     if (connectedNeurons[j])
                     {
                        connect(i, connectedNeurons, toSensor);
                        done = false;
                     }
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numNeurons; i++)
   {
      if (!connectedNeurons[i])
      {
         return(false);
      }
   }
   return(true);
}


// Connect neurons.
void Network::connect(int index, vector<bool>& connectedNeurons, bool toSensor)
{
   int i;

   connectedNeurons[index] = true;
   if (toSensor)
   {
      for (i = 0; i < numNeurons; i++)
      {
         if ((index != i) && (synapses[index][i] != NULL))
         {
            if (!connectedNeurons[i])
            {
               connect(i, connectedNeurons, toSensor);
            }
         }
      }
   }
   else
   {
      for (i = 0; i < numNeurons; i++)
      {
         if ((index != i) && (synapses[i][index] != NULL))
         {
            if (!connectedNeurons[i])
            {
               connect(i, connectedNeurons, toSensor);
            }
         }
      }
   }
}


// Destructor.
Network::~Network()
{
   int i, j;

   for (i = 0; i < numNeurons; i++)
   {
      delete neurons[i];
   }
   neurons.clear();
   for (i = 0; i < numNeurons; i++)
   {
      for (j = 0; j < numNeurons; j++)
      {
         if (synapses[i][j] != NULL)
         {
            delete synapses[i][j];
         }
      }
   }
   for (i = 0; i < numNeurons; i++)
   {
      synapses[i].clear();
   }
   synapses.clear();
   delete randomizer;
   randomizer = NULL;
}


// Clone network.
Network *Network::clone()
{
   int     i, j;
   Network *network;
   Synapse *synapse;

   network = new Network(numNeurons, numSensors, numMotors);
   assert(network != NULL);
   for (i = 0; i < numNeurons; i++)
   {
      *(network->neurons[i])       = *(neurons[i]);
      network->neurons[i]->network = network;
      for (j = 0; j < numNeurons; j++)
      {
         if (network->synapses[i][j] != NULL)
         {
            delete network->synapses[i][j];
            network->synapses[i][j] = NULL;
         }
      }
   }
   for (i = 0; i < numNeurons; i++)
   {
      for (j = 0; j < numNeurons; j++)
      {
         if (synapses[i][j] != NULL)
         {
            synapse = new Synapse();
            assert(synapse != NULL);
            network->synapses[i][j] = synapse;
            *synapse = *(synapses[i][j]);
         }
      }
   }
   return(network);
}


// Clear network.
void Network::clear()
{
   int i, j;

   for (i = 0; i < numNeurons; i++)
   {
      neurons[i]->activation = 0.0f;
      for (j = 0; j < numNeurons; j++)
      {
         if (synapses[i][j] != NULL)
         {
            synapses[i][j]->signal = 0.0f;
         }
      }
   }
}


// Step network.
void Network::step()
{
   int i, n;

   n = (int)neurons.size();

   // Fire neurons from synapses.
   for (i = 0; i < n; i++)
   {
      neurons[i]->fire();
   }

   // Propagate synaptic signals.
   for (i = 0; i < n; i++)
   {
      neurons[i]->propagate();
   }
}


// Load network.
bool Network::load(char *filename)
{
   FILE *fp = fopen(filename, "r");

   if (fp == NULL)
   {
      return(false);
   }
   load(fp);
   fclose(fp);
   return(true);
}


// Load network.
void Network::load(FILE *fp)
{
   int     i, j, n;
   Neuron  *neuron;
   Synapse *synapse;

   FREAD_INT(&numNeurons, fp);
   FREAD_INT(&numSensors, fp);
   FREAD_INT(&numMotors, fp);
   for (i = 0, n = (int)neurons.size(); i < n; i++)
   {
      delete neurons[i];
   }
   neurons.clear();
   neurons.resize(numNeurons);
   for (i = 0; i < numNeurons; i++)
   {
      neuron = new Neuron(this);
      assert(neuron != NULL);
      neurons[i] = neuron;
      neuron->load(fp);
   }
   for (i = 0, n = (int)synapses.size(); i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         if (synapses[i][j] != NULL)
         {
            delete synapses[i][j];
            synapses[i][j] = NULL;
         }
      }
   }
   synapses.resize(numNeurons);
   for (i = 0; i < numNeurons; i++)
   {
      synapses[i].resize(numNeurons);
      for (j = 0; j < numNeurons; j++)
      {
         synapses[i][j] = NULL;
         FREAD_INT(&n, fp);
         if (n == 1)
         {
            synapse = new Synapse();
            assert(synapse != NULL);
            synapses[i][j] = synapse;
            synapse->load(fp);
         }
      }
   }
   FREAD_LONG(&randomSeed, fp);
   if (randomizer == NULL)
   {
      randomizer = new Random(randomSeed);
      assert(randomizer != NULL);
   }
   randomizer->RAND_LOAD(fp);
}


// Save network.
bool Network::save(char *filename)
{
   FILE *fp = fopen(filename, "w");

   if (fp == NULL)
   {
      return(false);
   }
   save(fp);
   fclose(fp);
   return(true);
}


// Save network.
void Network::save(FILE *fp)
{
   int i, j, n;

   FWRITE_INT(&numNeurons, fp);
   FWRITE_INT(&numSensors, fp);
   FWRITE_INT(&numMotors, fp);
   for (i = 0; i < numNeurons; i++)
   {
      neurons[i]->save(fp);
   }
   for (i = 0; i < numNeurons; i++)
   {
      for (j = 0; j < numNeurons; j++)
      {
         if (synapses[i][j] != NULL)
         {
            n = 1;
            FWRITE_INT(&n, fp);
            synapses[i][j]->save(fp);
         }
         else
         {
            n = 0;
            FWRITE_INT(&n, fp);
         }
      }
   }
   FWRITE_LONG(&randomSeed, fp);
   randomizer->RAND_SAVE(fp);
}


// Print network.
void Network::print()
{
   int i, j, n;

   n = (int)neurons.size();
   printf("Neurons:\n");
   printf("type\tindex\texcitatory\tfunction\tactivation\n");
   for (i = 0; i < n; i++)
   {
      if (i < numSensors)
      {
         printf("sensor");
      }
      else if (i < (numSensors + numMotors))
      {
         printf("motor");
      }
      else
      {
         printf("inter");
      }
      printf("\t");
      neurons[i]->print(false);
      printf("\n");
   }
   printf("Synapse weights (horizontal=source/vertical=target):\n");
   printf("   ");
   for (i = 0; i < n; i++)
   {
      printf("%d", i);
      if (i < numSensors)
      {
         printf("s    ");
      }
      else if (i < (numSensors + numMotors))
      {
         printf("m    ");
      }
      else
      {
         printf("i    ");
      }
   }
   printf("\n");
   for (i = 0; i < n; i++)
   {
      printf("%d", i);
      if (i < numSensors)
      {
         printf("s ");
      }
      else if (i < (numSensors + numMotors))
      {
         printf("m ");
      }
      else
      {
         printf("i ");
      }
      for (j = 0; j < n; j++)
      {
         if (synapses[j][i] != NULL)
         {
            if (neurons[j]->excitatory)
            {
               if (synapses[j][i]->weight >= 0.0f)
               {
                  printf(" ");
               }
            }
            else
            {
               if (synapses[j][i]->weight < 0.0f)
               {
                  printf(" ");
               }
            }
            synapses[j][i]->print();
            printf(" ");
         }
         else
         {
            printf("      ");
         }
      }
      printf("\n");
   }
}


// Network graph dump in 'dot' format.
bool Network::dumpGraph(char *title, char *filename)
{
   FILE   *out;
   int    i, j, numNeurons;
   Neuron *neuron;

   if (filename == NULL)
   {
      out = stdout;
   }
   else
   {
      out = fopen(filename, "w");
      if (out == NULL)
      {
         return(false);
      }
   }

   numNeurons = (int)neurons.size();

   if (title == NULL)
   {
      title = (char *)"Network graph";
   }

   fprintf(out, "digraph bionet {\n");
   fprintf(out, "\tgraph [size=\"8.5,11\",fontsize=24];\n");
   fprintf(out, "\tsubgraph cluster_0 {\n");
   fprintf(out, "\tlabel=\"Sensors\";\n");

   for (i = 0; i < numSensors; i++)
   {
      neuron = neurons[i];
      fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=triangle];\n", (void *)neuron, neuron->index);
   }
   fprintf(out, "\t};\n");

   fprintf(out, "\tsubgraph cluster_1 {\n");
   fprintf(out, "\tlabel=\"Motors\";\n");
   for (i = 0; i < numMotors; i++)
   {
      neuron = neurons[numSensors + i];
      fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=triangle,orientation=180];\n", (void *)neuron, neuron->index);
   }
   fprintf(out, "\t};\n");

   fprintf(out, "\tsubgraph cluster_2 {\n");
   fprintf(out, "\tlabel=\"Interneurons\";\n");
   for (i = numSensors + numMotors; i < numNeurons; i++)
   {
      neuron = neurons[i];
      if (neuron->excitatory)
      {
         fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=diamond];\n", (void *)neuron, neuron->index);
      }
      else
      {
         fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=diamond,peripheries=2];\n", (void *)neuron, neuron->index);
      }
   }
   fprintf(out, "\t};\n");

   for (i = 0; i < numNeurons; i++)
   {
      neuron = neurons[i];
      for (j = 0; j < numNeurons; j++)
      {
         if (synapses[i][j] != NULL)
         {
            fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%0.2f\"];\n", (void *)neuron, (void *)neurons[j], synapses[i][j]->weight);
         }
      }
   }
   fprintf(out, "\tlabel = \"%s\";\n", title);
   fprintf(out, "}\n");

   if (out != stdout)
   {
      fclose(out);
   }
   return(true);
}
