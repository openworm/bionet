// Mutable parameter.

#include "mutableParm.hpp"

MutableParm::MutableParm()
{
   value             = 0.0f;
   minimum           = 0.0f;
   maximum           = 0.0f;
   maxDelta          = 0.0f;
   randomProbability = -1.0f;
}


MutableParm::MutableParm(float minimum, float maximum, float maxDelta,
                         float randomProbability)
{
   this->minimum           = minimum;
   this->maximum           = maximum;
   this->maxDelta          = maxDelta;
   this->randomProbability = randomProbability;
}


void MutableParm::init(float minimum, float maximum, float maxDelta,
                       float randomProbability)
{
   this->minimum           = minimum;
   this->maximum           = maximum;
   this->maxDelta          = maxDelta;
   this->randomProbability = randomProbability;
}


void MutableParm::initValue(Random *randomizer)
{
   value = (float)randomizer->RAND_INTERVAL(minimum, maximum);
}


void MutableParm::setValue(float value)
{
   this->value = value;
   if (value > maximum)
   {
      value = maximum;
   }
   if (value < minimum)
   {
      value = minimum;
   }
}


void MutableParm::mutateValue(Random *randomizer)
{
   if (randomizer->RAND_CHANCE(randomProbability))
   {
      value = (float)randomizer->RAND_INTERVAL(minimum, maximum);
   }
   else
   {
      float delta = (float)randomizer->RAND_INTERVAL(0.0, maxDelta);
      if (randomizer->RAND_BOOL())
      {
         value += delta;
         if (value > maximum)
         {
            value = maximum;
         }
      }
      else
      {
         value -= delta;
         if (value < minimum)
         {
            value = minimum;
         }
      }
   }
}


void MutableParm::load(FilePointer *fp)
{
   FREAD_FLOAT(&value, fp);
   FREAD_FLOAT(&minimum, fp);
   FREAD_FLOAT(&maximum, fp);
   FREAD_FLOAT(&maxDelta, fp);
   FREAD_FLOAT(&randomProbability, fp);
}


void MutableParm::save(FilePointer *fp)
{
   FWRITE_FLOAT(&value, fp);
   FWRITE_FLOAT(&minimum, fp);
   FWRITE_FLOAT(&maximum, fp);
   FWRITE_FLOAT(&maxDelta, fp);
   FWRITE_FLOAT(&randomProbability, fp);
}


void MutableParm::print()
{
   printf("value=%f\n", value);
   printf("minimum=%f\n", minimum);
   printf("maximum=%f\n", maximum);
   printf("maxDelta=%f\n", maxDelta);
   printf("randomProbability=%f\n", randomProbability);
}
