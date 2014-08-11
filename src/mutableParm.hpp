// Mutable parameter.

#ifndef __MUTABLEPARM_HPP__
#define __MUTABLEPARM_HPP__

#include "fileio.h"
#include "random.hpp"

// Mutable parameter.
class MutableParm
{
public:

   MutableParm();
   MutableParm(float minimum, float maximum, float maxDelta,
               float randomProbability);
   void init(float minimum, float maximum, float maxDelta,
             float randomProbability);

   float value;
   float minimum;
   float maximum;
   float maxDelta;
   float randomProbability;

   void initValue(Random *randomizer);
   void setValue(float value);
   void mutateValue(Random *randomizer);
   void load(FilePointer *fp);
   void save(FilePointer *fp);
   void print();
};
#endif
