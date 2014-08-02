// Neural network.

#include "network.hpp"
#include <algorithm>

// Default parameters.
const float Network:: DEFAULT_INHIBITOR_DENSITY  = 0.25f;
const float Network:: DEFAULT_SYNAPSE_PROPENSITY = 0.1f;
const RANDOM Network::DEFAULT_RANDOM_SEED        = 4517;

// Constructors.
Network::Network(int numNeurons, int numSensors, int numMotors,
                 float inhibitorDensity, float synapsePropensity,
                 float minSynapseWeight, float maxSynapseWeight,
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
   Random *randomizer = new Random(randomSeed);

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
         synapses[i][j].clear();
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
         if (synapses[i][j].size() == 0)
         {
            if (((i < numSensors) || (i >= n)) && (j >= numSensors))
            {
               if (randomizer->RAND_CHANCE(synapsePropensity))
               {
                  weight = (float)randomizer->RAND_INTERVAL(
                     minSynapseWeight, maxSynapseWeight);
                  synapses[i][j].push_back(new Synapse(weight));
                  assert(synapses[i][j][0] != NULL);
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
      if (synapses[i][j].size() == 0)
      {
         if (((i < numSensors) || (i >= n)) && (j >= numSensors))
         {
            weight = (float)randomizer->RAND_INTERVAL(
               minSynapseWeight, maxSynapseWeight);
            synapses[i][j].push_back(new Synapse(weight));
            assert(synapses[i][j][0] != NULL);
         }
      }
   }
   delete randomizer;
}


Network::Network(char *filename)
{
   neurons.clear();
   synapses.clear();
   load(filename);
}


Network::Network(FILE *fp)
{
   neurons.clear();
   synapses.clear();
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
                  if ((i != j) && (synapses[j][i].size() != 0))
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
                  if ((i != j) && (synapses[i][j].size() != 0))
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
         if ((index != i) && (synapses[index][i].size() != 0))
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
         if ((index != i) && (synapses[i][index].size() != 0))
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
   int i, j, k;

   for (i = 0; i < numNeurons; i++)
   {
      delete neurons[i];
   }
   neurons.clear();
   for (i = 0; i < numNeurons; i++)
   {
      for (j = 0; j < numNeurons; j++)
      {
         for (k = 0; k < (int)synapses[i][j].size(); k++)
         {
            delete synapses[i][j][k];
         }
         synapses[i][j].clear();
      }
   }
   for (i = 0; i < numNeurons; i++)
   {
      synapses[i].clear();
   }
   synapses.clear();
}


// Clone network.
Network *Network::clone()
{
   int     i, j, k;
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
         for (k = 0; k < (int)network->synapses[i][j].size(); k++)
         {
            delete network->synapses[i][j][k];
         }
         network->synapses[i][j].clear();
      }
   }
   for (i = 0; i < numNeurons; i++)
   {
      for (j = 0; j < numNeurons; j++)
      {
         for (k = 0; k < (int)synapses[i][j].size(); k++)
         {
            synapse = new Synapse();
            assert(synapse != NULL);
            network->synapses[i][j].push_back(synapse);
            *synapse = *(synapses[i][j][k]);
         }
      }
   }
   return(network);
}


// Clear network.
void Network::clear()
{
   int i, j, k;

   for (i = 0; i < numNeurons; i++)
   {
      neurons[i]->activation = 0.0f;
      for (j = 0; j < numNeurons; j++)
      {
         for (k = 0; k < (int)synapses[i][j].size(); k++)
         {
            synapses[i][j][k]->signal = 0.0f;
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
   FILE *fp = FOPEN_READ(filename);

   if (fp == NULL)
   {
      return(false);
   }
   load(fp);
   FCLOSE(fp);
   return(true);
}


// Load network.
void Network::load(FILE *fp)
{
   int     i, j, k, n;
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
         for (k = 0; k < (int)synapses[i][j].size(); k++)
         {
            delete synapses[i][j][k];
         }
         synapses[i][j].clear();
      }
   }
   synapses.resize(numNeurons);
   for (i = 0; i < numNeurons; i++)
   {
      synapses[i].resize(numNeurons);
      for (j = 0; j < numNeurons; j++)
      {
         synapses[i][j].clear();
         FREAD_INT(&n, fp);
         for (k = 0; k < n; k++)
         {
            synapse = new Synapse();
            assert(synapse != NULL);
            synapses[i][j].push_back(synapse);
            synapse->load(fp);
         }
      }
   }
}


// Save network.
bool Network::save(char *filename)
{
   FILE *fp = FOPEN_WRITE(filename);

   if (fp == NULL)
   {
      return(false);
   }
   save(fp);
   FCLOSE(fp);
   return(true);
}


// Save network.
void Network::save(FILE *fp)
{
   int i, j, k, n;

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
         n = (int)synapses[i][j].size();
         FWRITE_INT(&n, fp);
         for (k = 0; k < n; k++)
         {
            synapses[i][j][k]->save(fp);
         }
      }
   }
}


// Print network.
void Network::print(bool network, bool connectivity)
{
   int i, j, k, n;

   // Print network?
   if (network)
   {
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
            if (synapses[j][i].size() > 0)
            {
               for (k = 0; k < (int)synapses[j][i].size(); k++)
               {
                  if (neurons[j]->excitatory)
                  {
                     if (synapses[j][i][k]->weight >= 0.0f)
                     {
                        printf(" ");
                     }
                  }
                  else
                  {
                     if (synapses[j][i][k]->weight < 0.0f)
                     {
                        printf(" ");
                     }
                  }
                  synapses[j][i][k]->print();
                  if (k < (int)synapses[j][i].size() - 1)
                  {
                     printf("/");
                  }
               }
               printf(" ");
            }
            else
            {
               printf(" -    ");
            }
         }
         printf("\n");
      }
   }

   // Print connectivity?
   if (connectivity)
   {
      int    c, p, q;
      Neuron *sensor, *motor;
      queue<pair<Neuron *, int> >            open;
      vector<Neuron *>                       closed;
      vector<pair<Neuron *, vector<int> *> > endpoints;
      int         minConnections, maxConnections, sumConnections;
      int         minLength, maxLength;
      int         sumLengths, countLengths;
      int         sumShortestLengths, countShortestLengths;
      vector<int> *lengths;
      vector<int> values, values2;

      printf("Neurons: numSensors=%d, numMotors=%d, numNeurons=%d\n", numSensors, numMotors, numNeurons);
      values.clear();
      p = q = -1;
      for (i = c = 0; i < numNeurons; i++)
      {
         for (j = k = 0; j < numNeurons; j++)
         {
            c += (int)synapses[i][j].size();
            k += (int)synapses[i][j].size();
         }
         if (k > 0)
         {
            values.push_back(k);
            if ((p == -1) || (k < p))
            {
               p = k;
            }
            if ((q == -1) || (k > q))
            {
               q = k;
            }
         }
      }
      if (((int)values.size() > 0) && ((numNeurons - numMotors) > 0))
      {
         sort(values.begin(), values.end());
         printf("Synapses: total=%d, minimum=%d, maximum=%d, median=%d, mean=%0.2f\n",
                c, p, q, values[(int)values.size() / 2], (float)c / (float)(numNeurons - numMotors));
      }
      else
      {
         printf("Synapses: total=0, minimum=-1, maximum=-1, median=-1, mean=0.0f\n");
      }

      printf("Sensor to motors:\n");
      printf("sensor\tmotors\n");
      minConnections     = maxConnections = -1;
      sumConnections     = 0;
      minLength          = maxLength = -1;
      sumLengths         = countLengths = 0;
      sumShortestLengths = countShortestLengths = 0;
      values.clear();
      values2.clear();
      for (i = 0; i < numSensors; i++)
      {
         sensor = neurons[i];
         if (sensor->label.empty())
         {
            printf("%d", sensor->index);
         }
         else
         {
            printf("%s", sensor->label.c_str());
         }
         printf("\t");
         while (!open.empty())
         {
            open.pop();
         }
         open.push(pair<Neuron *, int>(sensor, 0));
         closed.clear();
         endpoints.clear();
         visitEndpoints(open, closed, endpoints, true);
         n = (int)endpoints.size();
         if ((minConnections == -1) || (n < minConnections))
         {
            minConnections = n;
         }
         if ((maxConnections == -1) || (n > maxConnections))
         {
            maxConnections = n;
         }
         values2.push_back(n);
         sumConnections += n;
         for (j = 0; j < n; j++)
         {
            lengths = endpoints[j].second;
            sort(lengths->begin(), lengths->end());
            if (lengths->size() > 0)
            {
               sumShortestLengths += (*lengths)[0];
               countShortestLengths++;
            }
            for (p = 0, q = (int)lengths->size(); p < q; p++)
            {
               values.push_back((*lengths)[p]);
               if ((minLength == -1) || ((*lengths)[p] < minLength))
               {
                  minLength = (*lengths)[p];
               }
               if ((maxLength == -1) || ((*lengths)[p] > maxLength))
               {
                  maxLength = (*lengths)[p];
               }
               sumLengths += (*lengths)[p];
               countLengths++;
            }
         }
         sort(endpoints.begin(), endpoints.end(), compareNeurons);
         for (j = 0, n = (int)endpoints.size(); j < n; j++)
         {
            motor = endpoints[j].first;
            if (motor->label.empty())
            {
               printf("%d", motor->index);
            }
            else
            {
               printf("%s", motor->label.c_str());
            }
            if (j < (n - 1))
            {
               printf(" ");
            }
            delete endpoints[j].second;
         }
         printf("\n");
      }
      if (((int)values2.size() > 0) && (numSensors > 0))
      {
         sort(values2.begin(), values2.end());
         printf("Connections: minimum=%d, maximum=%d, median=%d, mean=%0.2f\n",
                minConnections, maxConnections, values2[(int)values2.size() / 2],
                (float)sumConnections / (float)numSensors);
      }
      else
      {
         printf("Connections: minimum=-1, maximum=-1, median=-1, mean=0.0f\n");
      }
      if (((int)values.size() > 0) && (countLengths > 0))
      {
         sort(values.begin(), values.end());
         printf("Path lengths: minimum=%d, maximum=%d, median=%d, mean=%0.2f, mean shortest=%0.2f\n",
                minLength, maxLength, values[(int)values.size() / 2], (float)sumLengths / (float)countLengths,
                (float)sumShortestLengths / (float)countShortestLengths);
      }
      else
      {
         printf("Path lengths: minimum=-1, maximum=-1, median=-1, mean=0.0f, mean shortest=0.0f\n");
      }
      printf("Motor to sensors:\n");
      printf("motor\tsensors\n");
      minConnections     = maxConnections = -1;
      sumConnections     = 0;
      minLength          = maxLength = -1;
      sumLengths         = countLengths = 0;
      sumShortestLengths = countShortestLengths = 0;
      values.clear();
      values2.clear();
      for (i = numSensors, j = numSensors + numMotors; i < j; i++)
      {
         motor = neurons[i];
         if (motor->label.empty())
         {
            printf("%d", motor->index);
         }
         else
         {
            printf("%s", motor->label.c_str());
         }
         printf("\t");
         while (!open.empty())
         {
            open.pop();
         }
         open.push(pair<Neuron *, int>(motor, 0));
         closed.clear();
         endpoints.clear();
         visitEndpoints(open, closed, endpoints, false);
         n = (int)endpoints.size();
         if ((minConnections == -1) || (n < minConnections))
         {
            minConnections = n;
         }
         if ((maxConnections == -1) || (n > maxConnections))
         {
            maxConnections = n;
         }
         values2.push_back(n);
         sumConnections += n;
         for (k = 0; k < n; k++)
         {
            lengths = endpoints[k].second;
            sort(lengths->begin(), lengths->end());
            if (lengths->size() > 0)
            {
               sumShortestLengths += (*lengths)[0];
               countShortestLengths++;
            }
            for (p = 0, q = (int)lengths->size(); p < q; p++)
            {
               values.push_back((*lengths)[p]);
               if ((minLength == -1) || ((*lengths)[p] < minLength))
               {
                  minLength = (*lengths)[p];
               }
               if ((maxLength == -1) || ((*lengths)[p] > maxLength))
               {
                  maxLength = (*lengths)[p];
               }
               sumLengths += (*lengths)[p];
               countLengths++;
            }
         }
         sort(endpoints.begin(), endpoints.end(), compareNeurons);
         for (k = 0, n = (int)endpoints.size(); k < n; k++)
         {
            sensor = endpoints[k].first;
            if (sensor->label.empty())
            {
               printf("%d", sensor->index);
            }
            else
            {
               printf("%s", sensor->label.c_str());
            }
            if (k < (n - 1))
            {
               printf(" ");
            }
            delete endpoints[k].second;
         }
         printf("\n");
      }
      if (((int)values2.size() > 0) && (numMotors > 0))
      {
         sort(values2.begin(), values2.end());
         printf("Connections: minimum=%d, maximum=%d, median=%d, mean=%0.2f\n",
                minConnections, maxConnections, values2[(int)values2.size() / 2],
                (float)sumConnections / (float)numMotors);
      }
      else
      {
         printf("Connections: minimum=-1, maximum=-1, median=-1, mean=0.0f\n");
      }
      if (((int)values.size() > 0) && (countLengths > 0))
      {
         sort(values.begin(), values.end());
         printf("Path lengths: minimum=%d, maximum=%d, median=%d, mean=%0.2f, mean shortest=%0.2f\n",
                minLength, maxLength, values[(int)values.size() / 2], (float)sumLengths / (float)countLengths,
                (float)sumShortestLengths / (float)countShortestLengths);
      }
      else
      {
         printf("Path lengths: minimum=-1, maximum=-1, median=-1, mean=0.0f, mean shortest=0.0f\n");
      }
   }
}


// Get network metrics.
void Network::getMetrics(struct SynapseMetrics&    synapseMetrics,
                         struct ConnectionMetrics& sensorsToMotorsConnectionMetrics,
                         struct PathLengthMetrics& sensorsToMotorsPathLengthMetrics,
                         struct ConnectionMetrics& motorsToSensorsConnectionMetrics,
                         struct PathLengthMetrics& motorsToSensorsPathLengthMetrics)
{
   int    i, j, k, n, c, p, q;
   Neuron *sensor, *motor;

   queue<pair<Neuron *, int> >            open;
   vector<Neuron *>                       closed;
   vector<pair<Neuron *, vector<int> *> > endpoints;
   int         minConnections, maxConnections, sumConnections;
   int         minLength, maxLength;
   int         sumLengths, countLengths;
   int         sumShortestLengths, countShortestLengths;
   vector<int> *lengths;
   vector<int> values, values2;

   values.clear();
   p = q = -1;
   for (i = c = 0; i < numNeurons; i++)
   {
      for (j = k = 0; j < numNeurons; j++)
      {
         c += (int)synapses[i][j].size();
         k += (int)synapses[i][j].size();
      }
      if (k > 0)
      {
         values.push_back(k);
         if ((p == -1) || (k < p))
         {
            p = k;
         }
         if ((q == -1) || (k > q))
         {
            q = k;
         }
      }
   }
   if (((int)values.size() > 0) && ((numNeurons - numMotors) > 0))
   {
      synapseMetrics.total   = c;
      synapseMetrics.minimum = p;
      synapseMetrics.maximum = q;
      sort(values.begin(), values.end());
      synapseMetrics.median = values[(int)values.size() / 2];
      synapseMetrics.mean   = (float)c / (float)(numNeurons - numMotors);
   }
   else
   {
      synapseMetrics.total   = 0;
      synapseMetrics.minimum = -1;
      synapseMetrics.maximum = -1;
      synapseMetrics.median  = -1;
      synapseMetrics.mean    = 0.0f;
   }

   minConnections     = maxConnections = -1;
   sumConnections     = 0;
   minLength          = maxLength = -1;
   sumLengths         = countLengths = 0;
   sumShortestLengths = countShortestLengths = 0;
   values.clear();
   values2.clear();
   for (i = 0; i < numSensors; i++)
   {
      sensor = neurons[i];
      while (!open.empty())
      {
         open.pop();
      }
      open.push(pair<Neuron *, int>(sensor, 0));
      closed.clear();
      endpoints.clear();
      visitEndpoints(open, closed, endpoints, true);
      n = (int)endpoints.size();
      if ((minConnections == -1) || (n < minConnections))
      {
         minConnections = n;
      }
      if ((maxConnections == -1) || (n > maxConnections))
      {
         maxConnections = n;
      }
      values2.push_back(n);
      sumConnections += n;
      for (j = 0; j < n; j++)
      {
         lengths = endpoints[j].second;
         sort(lengths->begin(), lengths->end());
         if (lengths->size() > 0)
         {
            sumShortestLengths += (*lengths)[0];
            countShortestLengths++;
         }
         for (p = 0, q = (int)lengths->size(); p < q; p++)
         {
            values.push_back((*lengths)[p]);
            if ((minLength == -1) || ((*lengths)[p] < minLength))
            {
               minLength = (*lengths)[p];
            }
            if ((maxLength == -1) || ((*lengths)[p] > maxLength))
            {
               maxLength = (*lengths)[p];
            }
            sumLengths += (*lengths)[p];
            countLengths++;
         }
         delete endpoints[j].second;
      }
   }
   if ((sumConnections > 0) && (numSensors > 0))
   {
      sensorsToMotorsConnectionMetrics.minimum = minConnections;
      sensorsToMotorsConnectionMetrics.maximum = maxConnections;
      sort(values2.begin(), values2.end());
      sensorsToMotorsConnectionMetrics.median = values2[(int)values2.size() / 2];
      sensorsToMotorsConnectionMetrics.mean   = (float)sumConnections / (float)numSensors;
   }
   else
   {
      sensorsToMotorsConnectionMetrics.minimum = -1;
      sensorsToMotorsConnectionMetrics.maximum = -1;
      sensorsToMotorsConnectionMetrics.median  = -1;
      sensorsToMotorsConnectionMetrics.mean    = 0.0f;
   }
   if (((int)values.size() > 0) && (countLengths > 0))
   {
      sensorsToMotorsPathLengthMetrics.minimum = minLength;
      sensorsToMotorsPathLengthMetrics.maximum = maxLength;
      sort(values.begin(), values.end());
      sensorsToMotorsPathLengthMetrics.median       = values[(int)values.size() / 2];
      sensorsToMotorsPathLengthMetrics.mean         = (float)sumLengths / (float)countLengths;
      sensorsToMotorsPathLengthMetrics.shortestMean = (float)sumShortestLengths / (float)countShortestLengths;
   }
   else
   {
      sensorsToMotorsPathLengthMetrics.minimum      = -1;
      sensorsToMotorsPathLengthMetrics.maximum      = -1;
      sensorsToMotorsPathLengthMetrics.median       = -1;
      sensorsToMotorsPathLengthMetrics.mean         = 0.0f;
      sensorsToMotorsPathLengthMetrics.shortestMean = 0.0f;
   }

   minConnections     = maxConnections = -1;
   sumConnections     = 0;
   minLength          = maxLength = -1;
   sumLengths         = countLengths = 0;
   sumShortestLengths = countShortestLengths = 0;
   values.clear();
   values2.clear();
   for (i = numSensors, j = numSensors + numMotors; i < j; i++)
   {
      motor = neurons[i];
      while (!open.empty())
      {
         open.pop();
      }
      open.push(pair<Neuron *, int>(motor, 0));
      closed.clear();
      endpoints.clear();
      visitEndpoints(open, closed, endpoints, false);
      n = (int)endpoints.size();
      if ((minConnections == -1) || (n < minConnections))
      {
         minConnections = n;
      }
      if ((maxConnections == -1) || (n > maxConnections))
      {
         maxConnections = n;
      }
      values2.push_back(n);
      sumConnections += n;
      for (k = 0; k < n; k++)
      {
         lengths = endpoints[k].second;
         sort(lengths->begin(), lengths->end());
         if (lengths->size() > 0)
         {
            sumShortestLengths += (*lengths)[0];
            countShortestLengths++;
         }
         for (p = 0, q = (int)lengths->size(); p < q; p++)
         {
            values.push_back((*lengths)[p]);
            if ((minLength == -1) || ((*lengths)[p] < minLength))
            {
               minLength = (*lengths)[p];
            }
            if ((maxLength == -1) || ((*lengths)[p] > maxLength))
            {
               maxLength = (*lengths)[p];
            }
            sumLengths += (*lengths)[p];
            countLengths++;
         }
         delete endpoints[k].second;
      }
   }
   if ((sumConnections > 0) && (numMotors > 0))
   {
      motorsToSensorsConnectionMetrics.minimum = minConnections;
      motorsToSensorsConnectionMetrics.maximum = maxConnections;
      sort(values2.begin(), values2.end());
      motorsToSensorsConnectionMetrics.median = values2[(int)values2.size() / 2];
      motorsToSensorsConnectionMetrics.mean   = (float)sumConnections / (float)numMotors;
   }
   else
   {
      motorsToSensorsConnectionMetrics.minimum = -1;
      motorsToSensorsConnectionMetrics.maximum = -1;
      motorsToSensorsConnectionMetrics.median  = -1;
      motorsToSensorsConnectionMetrics.mean    = 0.0f;
   }
   if (((int)values.size() > 0) && (countLengths > 0))
   {
      motorsToSensorsPathLengthMetrics.minimum = minLength;
      motorsToSensorsPathLengthMetrics.maximum = maxLength;
      sort(values.begin(), values.end());
      motorsToSensorsPathLengthMetrics.median       = values[(int)values.size() / 2];
      motorsToSensorsPathLengthMetrics.mean         = (float)sumLengths / (float)countLengths;
      motorsToSensorsPathLengthMetrics.shortestMean = (float)sumShortestLengths / (float)countShortestLengths;
   }
   else
   {
      motorsToSensorsPathLengthMetrics.minimum      = -1;
      motorsToSensorsPathLengthMetrics.maximum      = -1;
      motorsToSensorsPathLengthMetrics.median       = -1;
      motorsToSensorsPathLengthMetrics.mean         = 0.0f;
      motorsToSensorsPathLengthMetrics.shortestMean = 0.0f;
   }
}


// Visit motor/sensor endpoints.
void Network::visitEndpoints(queue<pair<Neuron *, int> >& open,
                             vector<Neuron *>& closed,
                             vector<pair<Neuron *, vector<int> *> >& endpoints,
                             bool motorEndpoints)
{
   int i, j, k;

   vector<int> *lengths;

   if (open.empty())
   {
      return;
   }
   pair<Neuron *, int> current = open.front();
   open.pop();
   Neuron *neuron = current.first;
   int    index   = neuron->index;
   int    depth   = current.second;
   closed.push_back(neuron);

   if (motorEndpoints)
   {
      if ((index >= numSensors) && (index < (numSensors + numMotors)))
      {
         for (i = 0, j = (int)endpoints.size(); i < j; i++)
         {
            if (endpoints[i].first == neuron)
            {
               break;
            }
         }
         if (i == j)
         {
            lengths = new vector<int>();
            assert(lengths != NULL);
            lengths->push_back(depth);
            endpoints.push_back(pair<Neuron *, vector<int> *>(neuron, lengths));
         }
         else
         {
            lengths = endpoints[i].second;
            lengths->push_back(depth);
            endpoints[i] = pair<Neuron *, vector<int> *>(neuron, lengths);
         }
      }
      else
      {
         for (i = 0; i < numNeurons; i++)
         {
            if (synapses[index][i].size() != 0)
            {
               neuron = neurons[i];
               for (j = 0, k = (int)closed.size(); j < k; j++)
               {
                  if (neuron == closed[j])
                  {
                     break;
                  }
               }
               if (j == k)
               {
                  open.push(pair<Neuron *, int>(neuron, depth + 1));
               }
            }
         }
      }
   }
   else
   {
      if (index < numSensors)
      {
         for (i = 0, j = (int)endpoints.size(); i < j; i++)
         {
            if (endpoints[i].first == neuron)
            {
               break;
            }
         }
         if (i == j)
         {
            lengths = new vector<int>();
            assert(lengths != NULL);
            lengths->push_back(depth);
            endpoints.push_back(pair<Neuron *, vector<int> *>(neuron, lengths));
         }
         else
         {
            lengths = endpoints[i].second;
            lengths->push_back(depth);
            endpoints[i] = pair<Neuron *, vector<int> *>(neuron, lengths);
         }
      }
      else
      {
         for (i = 0; i < numNeurons; i++)
         {
            if (synapses[i][index].size() != 0)
            {
               neuron = neurons[i];
               for (j = 0, k = (int)closed.size(); j < k; j++)
               {
                  if (neuron == closed[j])
                  {
                     break;
                  }
               }
               if (j == k)
               {
                  open.push(pair<Neuron *, int>(neuron, depth + 1));
               }
            }
         }
      }
   }
   visitEndpoints(open, closed, endpoints, motorEndpoints);
}


// Network graph dump in 'dot' format.
bool Network::dumpGraph(char *title, char *filename)
{
   FILE       *out;
   int        i, j, k, numNeurons;
   Neuron     *neuron;
   const char *label;

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
      label  = neuron->label.c_str();
      if (strlen(label) == 0)
      {
         fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=triangle];\n", (void *)neuron, neuron->index);
      }
      else
      {
         fprintf(out, "\t\"%p\" [label=\"%s\",shape=triangle];\n", (void *)neuron, label);
      }
   }
   fprintf(out, "\t};\n");

   fprintf(out, "\tsubgraph cluster_1 {\n");
   fprintf(out, "\tlabel=\"Motors\";\n");
   for (i = 0; i < numMotors; i++)
   {
      neuron = neurons[numSensors + i];
      label  = neuron->label.c_str();
      if (strlen(label) == 0)
      {
         fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=triangle,orientation=180];\n", (void *)neuron, neuron->index);
      }
      else
      {
         fprintf(out, "\t\"%p\" [label=\"%s\",shape=triangle,orientation=180];\n", (void *)neuron, label);
      }
   }
   fprintf(out, "\t};\n");

   fprintf(out, "\tsubgraph cluster_2 {\n");
   fprintf(out, "\tlabel=\"Interneurons\";\n");
   for (i = numSensors + numMotors; i < numNeurons; i++)
   {
      neuron = neurons[i];
      label  = neuron->label.c_str();
      if (strlen(label) == 0)
      {
         if (neuron->excitatory)
         {
            fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=diamond];\n", (void *)neuron, neuron->index);
         }
         else
         {
            fprintf(out, "\t\"%p\" [label=\"index=%d\",shape=diamond,peripheries=2];\n", (void *)neuron, neuron->index);
         }
      }
      else
      {
         if (neuron->excitatory)
         {
            fprintf(out, "\t\"%p\" [label=\"%s\",shape=diamond];\n", (void *)neuron, label);
         }
         else
         {
            fprintf(out, "\t\"%p\" [label=\"%s\",shape=diamond,peripheries=2];\n", (void *)neuron, label);
         }
      }
   }
   fprintf(out, "\t};\n");

   for (i = 0; i < numNeurons; i++)
   {
      neuron = neurons[i];
      for (j = 0; j < numNeurons; j++)
      {
         for (k = 0; k < (int)synapses[i][j].size(); k++)
         {
            label = synapses[i][j][k]->label.c_str();
            if (strlen(label) == 0)
            {
               fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%0.2f\"];\n", (void *)neuron, (void *)neurons[j], synapses[i][j][k]->weight);
            }
            else
            {
               fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", (void *)neuron, (void *)neurons[j], label);
            }
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
