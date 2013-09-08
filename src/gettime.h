/*
 * Get time in milliseconds since the initial call.
 */

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#endif

typedef unsigned long long   TIME;
#define INVALID_TIME    ((unsigned long long)(-1))
TIME gettime();
