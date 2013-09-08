/*
 * A C-program for MT19937, with initialization improved 2002/1/26.
 * Coded by Takuji Nishimura and Makoto Matsumoto.
 *
 * Before using, initialize the state by using init_genrand(seed)
 * or init_by_array(init_key, key_length).
 *
 * Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The names of its contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Any feedback is very welcome.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 * email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
 */

#include "random.hpp"

/* initializes mt[N] with a seed */
void Random::init_genrand(RANDOM s)
{
   mt[0] = s & 0xffffffffUL;
   for (mti = 1; mti < RAND_N; mti++)
   {
      mt[mti] =
         (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
      /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
      /* In the previous versions, MSBs of the seed affect   */
      /* only MSBs of the array mt[].                        */
      /* 2002/01/09 modified by Makoto Matsumoto             */
      mt[mti] &= 0xffffffffUL;
      /* for >32 bit machines */
   }
}


/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void Random::init_by_array(RANDOM init_key[], int key_length)
{
   int i, j, k;

   init_genrand(19650218UL);
   i = 1;
   j = 0;
   k = (RAND_N > key_length ? RAND_N : key_length);
   for ( ; k; k--)
   {
      mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL))
              + init_key[j] + j;                  /* non linear */
      mt[i] &= 0xffffffffUL;                      /* for WORDSIZE > 32 machines */
      i++;
      j++;
      if (i >= RAND_N)
      {
         mt[0] = mt[RAND_N - 1];
         i     = 1;
      }
      if (j >= key_length)
      {
         j = 0;
      }
   }
   for (k = RAND_N - 1; k; k--)
   {
      mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL))
              - i;                                /* non linear */
      mt[i] &= 0xffffffffUL;                      /* for WORDSIZE > 32 machines */
      i++;
      if (i >= RAND_N)
      {
         mt[0] = mt[RAND_N - 1];
         i     = 1;
      }
   }

   mt[0] = 0x80000000UL;                          /* MSB is 1; assuring non-zero initial array */
}


/* generates a random number on [0,0xffffffff]-interval */
RANDOM Random::genrand_int32(void)
{
   RANDOM        y;
   static RANDOM mag01[2] = { 0x0UL, RAND_MATRIX_A };

   /* mag01[x] = x * RAND_MATRIX_A  for x=0,1 */

   if (mti >= RAND_N)                             /* generate N words at one time */
   {
      int kk;

      if (mti == RAND_N + 1)                      /* if init_genrand() has not been called, */
      {
         init_genrand(5489UL);                    /* a default initial seed is used */
      }
      for (kk = 0; kk < RAND_N - RAND_M; kk++)
      {
         y      = (mt[kk] & RAND_UPPER_MASK) | (mt[kk + 1] & RAND_LOWER_MASK);
         mt[kk] = mt[kk + RAND_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
      }
      for ( ; kk < RAND_N - 1; kk++)
      {
         y      = (mt[kk] & RAND_UPPER_MASK) | (mt[kk + 1] & RAND_LOWER_MASK);
         mt[kk] = mt[kk + (RAND_M - RAND_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
      }
      y = (mt[RAND_N - 1] & RAND_UPPER_MASK) | (mt[0] & RAND_LOWER_MASK);
      mt[RAND_N - 1] = mt[RAND_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

      mti = 0;
   }

   y = mt[mti++];

   /* Tempering */
   y ^= (y >> 11);
   y ^= (y << 7) & 0x9d2c5680UL;
   y ^= (y << 15) & 0xefc60000UL;
   y ^= (y >> 18);

   // Prevent return of INVALID_RANDOM
   if (y == INVALID_RANDOM)
   {
      y = 0x7fffffffUL;
   }
   return(y);
}


/* generates a random number on [0,0x7fffffff]-interval */
long Random::genrand_int31(void)
{
   return((unsigned long)(genrand_int32() >> 1));
}


/* generates a random number on [0,1]-real-interval */
double Random::genrand_real1(void)
{
   return(genrand_int32() * (1.0 / 4294967295.0));
   /* divided by 2^32-1 */
}


/* generates a random number on [0,1)-real-interval */
double Random::genrand_real2(void)
{
   return(genrand_int32() * (1.0 / 4294967296.0));
   /* divided by 2^32 */
}


/* generates a random number on (0,1)-real-interval */
double Random::genrand_real3(void)
{
   return((((double)genrand_int32()) + 0.5) * (1.0 / 4294967296.0));
   /* divided by 2^32 */
}


/* generates a random number on [0,1) with 53-bit resolution*/
double Random::genrand_res53(void)
{
   RANDOM a = genrand_int32() >> 5, b = genrand_int32() >> 6;

   return((a * 67108864.0 + b) * (1.0 / 9007199254740992.0));
}


/* These real versions are due to Isaku Wada, 2002/01/09 added */

/* Load and save added by TEP: */

/* load state from file pointer */
void Random::load_genrand(FILE *fp)
{
   for (int i = 0; i < RAND_N; i++)
   {
      FREAD_LONG(&mt[i], fp);
   }
   FREAD_INT(&mti, fp);
}


/* save state to file pointer */
void Random::save_genrand(FILE *fp)
{
   for (int i = 0; i < RAND_N; i++)
   {
      FWRITE_LONG(&mt[i], fp);
   }
   FWRITE_INT(&mti, fp);
}


// Save random state.
void Random::push_genrand()
{
   RANDOM *mtp;

   mtp = new RANDOM[RAND_N];
   assert(mtp != NULL);
   for (int i = 0; i < RAND_N; i++)
   {
      mtp[i] = mt[i];
   }
   smt.push(mtp);
   smti.push(mti);
}


// Restore random state.
void Random::pop_genrand()
{
   RANDOM *mtp;

   assert(smt.size() > 0);
   mtp = smt.top();
   smt.pop();
   for (int i = 0; i < RAND_N; i++)
   {
      mt[i] = mtp[i];
   }
   delete mtp;
   mti = smti.top();
   smti.pop();
}


// Seed random numbers.
void Random::SRAND(RANDOM seed)
{
   init_genrand(seed);
}


// Get random number
RANDOM Random::RAND()
{
   return(genrand_int32());
}


// Random probability >= 0.0 && <= 1.0
double Random::RAND_PROB()
{
   return(genrand_real1());
}


// Random within interval >= min && <= max
double Random::RAND_INTERVAL(double min, double max)
{
   return((genrand_real1() * (max - min)) + min);
}


// Random chance for given probability
bool Random::RAND_CHANCE(double p)
{
   if (p <= 0.0)
   {
      return(false);
   }
   if (RAND_PROB() <= p)
   {
      return(true);
   }
   return(false);
}


// Random choice of 0 to n-1.
int Random::RAND_CHOICE(int n)
{
   return(genrand_int32() % n);
}


// Random boolean.
bool Random::RAND_BOOL()
{
   return((RAND() % 2) == 0 ? false : true);
}


// Load random state
void Random::RAND_LOAD(FILE *fp)
{
   load_genrand(fp);
}


// Save random state
void Random::RAND_SAVE(FILE *fp)
{
   save_genrand(fp);
}


// Save random state
void Random::RAND_PUSH()
{
   push_genrand();
}


// Restore random state
void Random::RAND_POP()
{
   pop_genrand();
}


// Clear random stack
void Random::RAND_CLEAR()
{
   while (smt.size() > 0)
   {
      pop_genrand();
   }
}


// Clone random state
void Random::RAND_CLONE(Random& random)
{
   stack<RANDOM *> smt2;
   stack<int>      smti2;
   RANDOM          *mtp, *mtp2;
   RANDOM          r;

   random.RAND_CLEAR();
   for (int i = 0; i < RAND_N; i++)
   {
      random.mt[i] = mt[i];
   }
   random.mti = mti;

   while (smt.size() > 0)
   {
      mtp = smt.top();
      smt.pop();
      smt2.push(mtp);
      r = smti.top();
      smti.pop();
      smti2.push(r);
   }

   while (smt2.size() > 0)
   {
      mtp = smt2.top();
      smt2.pop();
      smt.push(mtp);
      mtp2 = new RANDOM[RAND_N];
      assert(mtp2 != NULL);
      for (int i = 0; i < RAND_N; i++)
      {
         mtp2[i] = mtp[i];
      }
      random.smt.push(mtp2);
      r = smti2.top();
      smti2.pop();
      smti.push(r);
      random.smti.push(r);
   }
}


// Return random probability for given random object.
double external_rand(void *random)
{
   return(((Random *)random)->RAND_PROB());
}
