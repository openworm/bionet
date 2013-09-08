// Load and store functions.

#ifndef __FILEIO__
#define __FILEIO__

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Find paths to files.
char *getResourcePath(char *file);
char *getDataPath(char *file);
char *getPath(char *dir, char *file);

// File I/O.
#define FREAD_INT           myfreadInt
#define FWRITE_INT          myfwriteInt
#define FREAD_SHORT         myfreadShort
#define FWRITE_SHORT        myfwriteShort
#define FREAD_LONG          myfreadLong
#define FWRITE_LONG         myfwriteLong
#define FREAD_LONG_LONG     myfreadLongLong
#define FWRITE_LONG_LONG    myfwriteLongLong
#define FREAD_FLOAT         myfreadFloat
#define FWRITE_FLOAT        myfwriteFloat
#define FREAD_DOUBLE        myfreadDouble
#define FWRITE_DOUBLE       myfwriteDouble
#define FREAD_BOOL          myfreadBool
#define FWRITE_BOOL         myfwriteBool
#define FREAD_CHAR          myfreadChar
#define FWRITE_CHAR         myfwriteChar
#define FREAD_BYTES         myfreadBytes
#define FWRITE_BYTES        myfwriteBytes
#define FREAD_STRING        myfreadString
#define FWRITE_STRING       myfwriteString
int myfreadInt(int *, FILE *);
int myfwriteInt(int *, FILE *);
int myfreadShort(short *, FILE *);
int myfwriteShort(short *, FILE *);
int myfreadLong(unsigned long *, FILE *);
int myfwriteLong(unsigned long *, FILE *);
int myfreadLongLong(unsigned long long *, FILE *);
int myfwriteLongLong(unsigned long long *, FILE *);
int myfreadFloat(float *, FILE *);
int myfwriteFloat(float *, FILE *);
int myfreadDouble(double *, FILE *);
int myfwriteDouble(double *, FILE *);
int myfreadBool(bool *, FILE *);
int myfwriteBool(bool *, FILE *);
int myfreadChar(unsigned char *, FILE *);
int myfwriteChar(unsigned char *, FILE *);
int myfreadBytes(unsigned char *, int size, FILE *);
int myfwriteBytes(unsigned char *, int size, FILE *);
int myfreadString(char *, int size, FILE *);
int myfwriteString(char *, int size, FILE *);
#endif
