// Sensory-motor behavior.

#include "behavior.hpp"

// Behavior constructors.
Behavior::Behavior(Network *network, int length, Random *randomizer)
{
   int i, j;

   vector<float> sensors;
   vector<float> motors;

   sensors.resize(network->numSensors);
   motors.resize(network->numMotors);
   for (i = 0; i < length; i++)
   {
      for (j = 0; j < network->numSensors; j++)
      {
         sensors[j] = (float)randomizer->RAND_PROB();
         if ((j % 2) == (i % 2))
         {
            sensors[j] = 1.0f;
         }
         else
         {
            sensors[j] = 0.0f;
         }
         network->neurons[j]->bias = sensors[j];
      }
      sensorSequence.push_back(sensors);
      network->step();
      for (j = 0; j < network->numMotors; j++)
      {
         motors[j] = network->neurons[network->numSensors + j]->activation;
      }
      motorSequence.push_back(motors);
   }
}


// Construct from given sensor sequence.
Behavior::Behavior(Network *network, vector<vector<float> >& sensorSequence)
{
   int i, j;

   vector<float> sensors;
   vector<float> motors;

   sensors.resize(network->numSensors);
   motors.resize(network->numMotors);
   network->clear();
   for (i = 0; i < (int)sensorSequence.size(); i++)
   {
      assert(network->numSensors == (int)sensorSequence[i].size());
      for (j = 0; j < network->numSensors; j++)
      {
         sensors[j] = sensorSequence[i][j];
         network->neurons[j]->bias = sensors[j];
      }
      this->sensorSequence.push_back(sensors);
      network->step();
      for (j = 0; j < network->numMotors; j++)
      {
         motors[j] = network->neurons[network->numSensors + j]->activation;
      }
      this->motorSequence.push_back(motors);
   }
}


Behavior::Behavior(FILE *fp)
{
   load(fp);
}


// Behavior destructor.
Behavior::~Behavior()
{
   sensorSequence.clear();
   motorSequence.clear();
}


// Load behavior.
void Behavior::load(FILE *fp)
{
   int   i, j, n, numSensors, numMotors;
   float f;

   FREAD_INT(&n, fp);
   sensorSequence.resize(n);
   motorSequence.resize(n);
   FREAD_INT(&numSensors, fp);
   FREAD_INT(&numMotors, fp);
   for (i = 0; i < n; i++)
   {
      sensorSequence[i].resize(numSensors);
      for (j = 0; j < numSensors; j++)
      {
         FREAD_FLOAT(&f, fp);
         sensorSequence[i][j] = f;
      }
      motorSequence[i].resize(numMotors);
      for (j = 0; j < numMotors; j++)
      {
         FREAD_FLOAT(&f, fp);
         motorSequence[i][j] = f;
      }
   }
}


// Save behavior.
void Behavior::save(FILE *fp)
{
   int   i, j, n, numSensors, numMotors;
   float f;

   n = (int)sensorSequence.size();
   FWRITE_INT(&n, fp);
   if (n == 0)
   {
      return;
   }
   numSensors = (int)sensorSequence[0].size();
   FWRITE_INT(&numSensors, fp);
   numMotors = (int)motorSequence[0].size();
   FWRITE_INT(&numMotors, fp);
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < numSensors; j++)
      {
         f = sensorSequence[i][j];
         FWRITE_FLOAT(&f, fp);
      }
      for (j = 0; j < numMotors; j++)
      {
         f = motorSequence[i][j];
         FWRITE_FLOAT(&f, fp);
      }
   }
}


// Print sequence.
void Behavior::print()
{
   int i, j;

   printf("Sensory-motor sequence:\n");
   for (i = 0; i < (int)sensorSequence.size(); i++)
   {
      printf("  sensors: ");
      for (j = 0; j < (int)sensorSequence[i].size(); j++)
      {
         printf("%0.2f ", sensorSequence[i][j]);
      }
      printf("  motors: ");
      for (j = 0; j < (int)motorSequence[i].size(); j++)
      {
         printf("%0.2f ", motorSequence[i][j]);
      }
      printf("\n");
   }
}
