// Wriggle implementation.

#include "wriggle.hpp"
#include "behavior.hpp"

// Constructors.
Wriggle::Wriggle(int movements,
                 MutableParm& periodParm,
                 MutableParm& amplitudeParm,
                 MutableParm& phaseParm,
                 MutableParm& speedParm,
                 MutableParm& delayParm,
                 Random *randomizer, int tag)
{
   this->movements     = movements;
   this->periodParm    = periodParm;
   this->amplitudeParm = amplitudeParm;
   this->phaseParm     = phaseParm;
   this->delayParm     = delayParm;
   this->randomizer    = randomizer;
   this->tag           = tag;
}


Wriggle::Wriggle()
{
   randomizer = NULL;
   tag        = -1;
}


Wriggle::Wriggle(FILE *fp, int movements, Random *randomizer)
{
}


// Destructor.
Wriggle::~Wriggle()
{
}


// Evaluate wriggling behavior.
void Wriggle::evaluate()
{
}


// Mutate.
void Wriggle::mutate()
{
}


// Clone.
Wriggle *Wriggle::clone()
{
   return(NULL);
}


// Load.
void Wriggle::load(FILE *fp)
{
}


// Save.
void Wriggle::save(FILE *fp)
{
}


// Print.
void Wriggle::print()
{
}
