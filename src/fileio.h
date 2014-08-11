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

// File pointer.
class FilePointer
{
public:
   FILE *fp;
   bool binary;
   FilePointer(FILE *fp, bool binary)
   {
	   this->fp = fp;
	   this->binary = binary;
   }
};

// Find paths to files.
char *getResourcePath(char *file);
char *getDataPath(char *file);
char *getPath(char *dir, char *file);

// File I/O.
#define FOPEN_READ          myfopenRead
#define FOPEN_WRITE         myfopenWrite
#define FCLOSE              myfclose
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
FilePointer *myfopenRead(char *filename, bool binary);
FilePointer *myfopenWrite(char *filename, bool binary);
int myfclose(FilePointer *fp);
int myfreadInt(int *, FilePointer *);
int myfwriteInt(int *, FilePointer *);
int myfreadShort(short *, FilePointer *);
int myfwriteShort(short *, FilePointer *);
int myfreadLong(unsigned long *, FilePointer *);
int myfwriteLong(unsigned long *, FilePointer *);
int myfreadLongLong(unsigned long long *, FilePointer *);
int myfwriteLongLong(unsigned long long *, FilePointer *);
int myfreadFloat(float *, FilePointer *);
int myfwriteFloat(float *, FilePointer *);
int myfreadDouble(double *, FilePointer *);
int myfwriteDouble(double *, FilePointer *);
int myfreadBool(bool *, FilePointer *);
int myfwriteBool(bool *, FilePointer *);
int myfreadChar(unsigned char *, FilePointer *);
int myfwriteChar(unsigned char *, FilePointer *);
int myfreadBytes(unsigned char *, int size, FilePointer *);
int myfwriteBytes(unsigned char *, int size, FilePointer *);
int myfreadString(char *, int size, FilePointer *);
int myfwriteString(char *, int size, FilePointer *);
#endif
