// Load and save in binary or readable format.

#include "fileio.h"

// Get path to resource file.
// Application responsible for freeing malloc'd memory.
char *getResourcePath(char *file)
{
   return(getPath((char *)"resource", file));
}


// Get path to data file.
// Application responsible for freeing malloc'd memory.
char *getDataPath(char *file)
{
   return(getPath((char *)"data", file));
}


// Get path to file within directory.
// Application responsible for freeing malloc'd memory.
char *getPath(char *dir, char *file)
{
   char *home, *path, *path2;

   // Sanity check.
   if ((file == NULL) || (*file == '\0'))
   {
      return(NULL);
   }

   // Fixed path?
   if ((file[0] == '/') || (file[0] == '.'))
   {
      if ((path = (char *)malloc(strlen(file) + 1)) == NULL)
      {
         fprintf(stderr, "getPath: cannot malloc path memory\n");
         exit(1);
      }
      strcpy(path, file);
      return(path);
   }

   // Check MONA_HOME directory path environment variable.
   if ((home = getenv("MONA_HOME")) != NULL)
   {
#ifdef WIN32
      // Replace Cygwin path with Windows path.
      if ((strncmp(home, "/cygdrive/", 10) == 0) && (strlen(home) >= 11))
      {
         if ((path = (char *)malloc(strlen(home) + strlen(dir) + 2)) == NULL)
         {
            fprintf(stderr, "getPath: cannot malloc path memory\n");
            exit(1);
         }
         sprintf(path, "c:%s/%s", &home[11], dir);
      }
      else
      {
         if ((path = (char *)malloc(strlen(home) + strlen(dir) + 2)) == NULL)
         {
            fprintf(stderr, "getPath: cannot malloc path memory\n");
            exit(1);
         }
         sprintf(path, "%s/%s", home, dir);
      }
      if (GetFileAttributes(path) != 0xffffffff)
#else
      if ((path = (char *)malloc(strlen(home) + strlen(dir) + 2)) == NULL)
      {
         fprintf(stderr, "getPath: cannot malloc path memory\n");
         exit(1);
      }
      sprintf(path, "%s/%s", home, dir);
      if (access(path, F_OK) != -1)
#endif
      {
         // Add the file.
         if ((path2 = (char *)malloc(strlen(path) + strlen(file) + 2)) == NULL)
         {
            fprintf(stderr, "getPath: cannot malloc path memory\n");
            exit(1);
         }
         sprintf(path2, "%s/%s", path, file);
         free(path);
         return(path2);
      }
      else
      {
         free(path);
      }
   }

   // Try relative paths.
   if ((path = (char *)malloc(strlen(dir) + 1)) == NULL)
   {
      fprintf(stderr, "getPath: cannot malloc path memory\n");
      exit(1);
   }
   sprintf(path, "%s", dir);
#ifdef WIN32
   if (GetFileAttributes(path) != 0xffffffff)
#else
   if (access(path, F_OK) != -1)
#endif
   {
      free(path);
      if ((path = (char *)malloc(strlen(dir) + strlen(file) + 2)) == NULL)
      {
         fprintf(stderr, "getPath: cannot malloc path memory\n");
         exit(1);
      }
      sprintf(path, "%s/%s", dir, file);
      return(path);
   }
   else
   {
      free(path);
   }
   if ((path = (char *)malloc(strlen(dir) + 4)) == NULL)
   {
      fprintf(stderr, "getPath: cannot malloc path memory\n");
      exit(1);
   }
   sprintf(path, "../%s", dir);
#ifdef WIN32
   if (GetFileAttributes(path) != 0xffffffff)
#else
   if (access(path, F_OK) != -1)
#endif
   {
      free(path);
      if ((path = (char *)malloc(strlen(dir) + strlen(file) + 5)) == NULL)
      {
         fprintf(stderr, "getPath: cannot malloc path memory\n");
         exit(1);
      }
      sprintf(path, "../%s/%s", dir, file);
      return(path);
   }
   else
   {
      free(path);
   }
   if ((path = (char *)malloc(strlen(dir) + 7)) == NULL)
   {
      fprintf(stderr, "getPath: cannot malloc path memory\n");
      exit(1);
   }
   sprintf(path, "../../%s", dir);
#ifdef WIN32
   if (GetFileAttributes(path) != 0xffffffff)
#else
   if (access(path, F_OK) != -1)
#endif
   {
      free(path);
      if ((path = (char *)malloc(strlen(dir) + strlen(file) + 8)) == NULL)
      {
         fprintf(stderr, "getPath: cannot malloc path memory\n");
         exit(1);
      }
      sprintf(path, "../../%s/%s", dir, file);
      return(path);
   }
   else
   {
      free(path);
   }

   // Default to input file.
   if ((path = (char *)malloc(strlen(file) + 1)) == NULL)
   {
      fprintf(stderr, "getPath: cannot malloc path memory\n");
      exit(1);
   }
   strcpy(path, file);
   return(path);
}


int myfreadInt(int *i, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(i, sizeof(int), 1, fp));

#else
   return(fscanf(fp, "%d", i));
#endif
}


int myfwriteInt(int *i, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(i, sizeof(int), 1, fp));

#else
   return(fprintf(fp, "%d\n", *i));
#endif
}


int myfreadShort(short *s, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(s, sizeof(short), 1, fp));

#else
   short v;
   int   ret = fscanf(fp, "%hd", &v);
   *s = v;
   return(ret);
#endif
}


int myfwriteShort(short *s, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(s, sizeof(short), 1, fp));

#else
   short v;
   v = *s;
   return(fprintf(fp, "%hd\n", v));
#endif
}


int myfreadLong(unsigned long *l, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(l, sizeof(unsigned long), 1, fp));

#else
   return(fscanf(fp, "%lu", l));
#endif
}


int myfwriteLong(unsigned long *l, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(l, sizeof(unsigned long), 1, fp));

#else
   return(fprintf(fp, "%lu\n", *l));
#endif
}


int myfreadLongLong(unsigned long long *l, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(l, sizeof(unsigned long long), 1, fp));

#else
   return(fscanf(fp, "%llu", l));
#endif
}


int myfwriteLongLong(unsigned long long *l, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(l, sizeof(unsigned long long), 1, fp));

#else
   return(fprintf(fp, "%llu\n", *l));
#endif
}


int myfreadFloat(float *f, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(f, sizeof(float), 1, fp));

#else
   char buf[100];
   int  ret = fscanf(fp, "%s", buf);
   *f = (float)atof(buf);
   return(ret);
#endif
}


int myfwriteFloat(float *f, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(f, sizeof(float), 1, fp));

#else
   return(fprintf(fp, "%f\n", *f));
#endif
}


int myfreadDouble(double *d, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(d, sizeof(double), 1, fp));

#else
   char buf[100];
   int  ret = fscanf(fp, "%s", buf);
   *d = strtod(buf, NULL);
   return(ret);
#endif
}


int myfwriteDouble(double *d, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(d, sizeof(double), 1, fp));

#else
   return(fprintf(fp, "%f\n", *d));
#endif
}


int myfreadBool(bool *b, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(b, sizeof(bool), 1, fp));

#else
   int v;
   int ret = fscanf(fp, "%d", &v);
   if (v == 1)
   {
      *b = true;
   }
   else
   {
      *b = false;
   }
   return(ret);
#endif
}


int myfwriteBool(bool *b, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(b, sizeof(bool), 1, fp));

#else
   if (*b)
   {
      return(fprintf(fp, "1\n"));
   }
   else
   {
      return(fprintf(fp, "0\n"));
   }
#endif
}


int myfreadChar(unsigned char *c, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(c, sizeof(unsigned char), 1, fp));

#else
   char buf[10];
   int  ret = fscanf(fp, "%s", buf);
   *c = buf[0];
   return(ret);
#endif
}


int myfwriteChar(unsigned char *c, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(c, sizeof(unsigned char), 1, fp));

#else
   return(fprintf(fp, "%c\n", *c));
#endif
}


int myfreadBytes(unsigned char *bytes, int size, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(bytes, size, 1, fp));

#else
   int           len  = (2 * size) + 1;
   unsigned char *buf = new unsigned char[len];
   assert(buf != NULL);
   int ret = fscanf(fp, "%s", buf);
   int i, j, d1, d2;
   for (i = 0; i < size; i++)
   {
      j = 2 * i;
      if ((buf[j] >= '0') && (buf[j] <= '9'))
      {
         d1 = buf[j] - '0';
      }
      else
      {
         d1 = buf[j] - 'a' + 10;
      }
      j++;
      if ((buf[j] >= '0') && (buf[j] <= '9'))
      {
         d2 = buf[j] - '0';
      }
      else
      {
         d2 = buf[j] - 'a' + 10;
      }
      bytes[i] = (d1 * 16) + d2;
   }
   delete [] buf;
   return(ret);
#endif
}


int myfwriteBytes(unsigned char *bytes, int size, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(bytes, size, 1, fp));

#else
   int  len  = (2 * size) + 1;
   char *buf = new char[len];
   assert(buf != NULL);
   for (int i = 0; i < size; i++)
   {
      sprintf(&buf[2 * i], "%02x", bytes[i]);
   }
   buf[len - 1] = '\0';
   int ret = fprintf(fp, "%s\n", buf);
   delete [] buf;
   return(ret);
#endif
}


int myfreadString(char *str, int size, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fread(str, size, 1, fp));

#else
   // String is delimited by double quotes.
   char c;
   while (true)
   {
      c = fgetc(fp);
      if (c == EOF)
      {
         return(0);
      }
      if (c == '"')
      {
         break;
      }
   }
   int i = 0;
   for ( ; i < size; i++)
   {
      c = fgetc(fp);
      if (c == EOF)
      {
         return(i);
      }
      if (c == '"')
      {
         str[i] = '\0';
         return(i);
      }
      else
      {
         str[i] = c;
      }
   }
   fgetc(fp);
   return(i);
#endif
}


int myfwriteString(char *str, int size, FILE *fp)
{
#ifdef BINARY_FILE_FORMAT
   return((int)fwrite(str, size, 1, fp));

#else
   // Delimit string with double quotes.
   fputc('"', fp);
   int ret = 0;
   for (int i = 0; i < size && str[i] != '\0'; i++)
   {
      if (str[i] != '"')
      {
         fputc(str[i], fp);
         ret++;
      }
   }
   fputc('"', fp);
   fputc('\n', fp);
   return(ret);
#endif
}
