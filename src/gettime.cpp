/*
 * Get time in milliseconds since the initial call.
 */

#include "gettime.h"
#include <assert.h>

TIME gettime()
{
   TIME t;

#ifdef WIN32
   static TIME base_time = 0;
   if (base_time == 0)
   {
      base_time = (TIME)GetTickCount64();
      return(0);
   }
   else
   {
      t = (TIME)GetTickCount64();
      assert(t >= base_time);
      return(t - base_time);
   }
#else
   static time_t      base_sec  = 0;
   static suseconds_t base_usec = 0;
   struct timeval     tv;
   gettimeofday(&tv, NULL);
   if (base_sec == 0)
   {
      base_sec  = tv.tv_sec;
      base_usec = tv.tv_usec;
      return(0);
   }
   else
   {
      t = (TIME)(((tv.tv_sec - base_sec) * 1000) + ((tv.tv_usec - base_usec) / 1000));
      return(t);
   }
#endif
}
