// Network morph implementation.

#include "networkMorph.hpp"

// Maximum tolerated motor output error.
const float NetworkMorph::MAX_ERROR_TOLERANCE = 0.05f;

// Constructor.
NetworkMorph::NetworkMorph()
{
   network        = NULL;
   tag            = 0;
   error          = 0.0f;
   behaves        = false;
   offspringCount = 0;
}


// Evaluate behavior.
void NetworkMorph::evaluate(vector<Behavior *>& behaviors, int maxStep)
{
   vector<bool> fitnessMotorList;
   evaluate(behaviors, fitnessMotorList, maxStep);
}


void NetworkMorph::evaluate(vector<Behavior *>& behaviors,
                            vector<bool>& fitnessMotorList, int maxStep)
{
   int      i, j, k, n, m, o, count, exceed;
   bool     motorFitness;
   float    delta;
   Behavior *testBehavior;

   error = 0.0f;
   fill(motorErrors.begin(), motorErrors.end(), false);
   behaves = true;
   count   = exceed = 0;
   if (fitnessMotorList.size() > 0)
   {
      motorFitness = true;
   }
   else
   {
      motorFitness = false;
   }
   for (i = 0, n = (int)behaviors.size(); i < n; i++)
   {
      testBehavior = new Behavior(network, behaviors[i]->sensorSequence);
      assert(testBehavior != NULL);
      m = (int)testBehavior->motorSequence.size();
      if ((maxStep != -1) && ((maxStep + 1) < m))
      {
         m = maxStep + 1;
      }
      for (j = 0; j < m; j++)
      {
         for (k = 0, o = (int)testBehavior->motorSequence[j].size(); k < o; k++)
         {
            if (motorFitness && !fitnessMotorList[k])
            {
               continue;
            }
            delta = fabs(behaviors[i]->motorSequence[j][k] - testBehavior->motorSequence[j][k]);
            if (delta > MAX_ERROR_TOLERANCE)
            {
               exceed++;
               behaves = false;
               if (motorErrors.size() > 0)
               {
                  motorErrors[k] = true;
               }
            }
            error += delta;
            count++;
         }
      }
      delete testBehavior;
   }
   if (count > 0)
   {
      error /= (float)count;
   }
   error += (float)exceed;
}
