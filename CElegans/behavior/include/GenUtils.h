#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include "XMLUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "GLee.h"

#define deleteObject(A){ if(A){ delete   A; A = NULL; } }
#define deleteArray(A) { if(A){ delete[] A; A = NULL; } }

#define MAX_TEX_UNITS 8

class Logger
{
  public:
    static void writeImmidiateInfoLog(const std::string &info);
    static void writeFatalErrorLog(const std::string &logString);
    static bool writeErrorLog(const std::string &error);
    static void writeInfoLog(const std::string &info);
    static void initialize(const char* logfilename = NULL);
    static void flush();

  private:
    static vector<std::string> logStrings;
    static std::string         logPath;
};

class NamedObject
{

  protected:
    std::string name;

  public:
    NamedObject(const char*  argName = NULL);
    NamedObject(const std::string &argName);
    NamedObject(const NamedObject &copy);
   ~NamedObject();

    NamedObject &operator=(const NamedObject &copy);
    void   setName(const char   *nameArg);
    void   setName(const std::string &name);

    const std::string &getName()     const;
    const char*        getCharName() const;

};

class Perlin
{
  private:
    static void   normalize2(double v[2]);
    static void   normalize3(double v[3]);

  public:
    static void   setNoiseFrequency(int frequency);
    static int    getNoiseFrequency();

    static double noise1(double arg);
    static double noise2(double vec[2]);
    static double noise3(double vec[3]);
    static void   initialize();
    static double noise1D(double x,double alpha,double beta,int n);
    static double noise2D(double x, double y, double alpha, double beta, int n);
    static double noise3D(double x, double y, double z, double alpha, double beta, int n);
};

class MediaPathManager
{
  public:
    static const std::string lookUpMediaPath(const std::string  &path);
    static const std::string getPathAt(int index);

    static bool  registerPath(const TiXmlElement *MediaPathNode);
    static bool  registerPath(const std::string  &path);

    static int   getPathCount();
    static void  printAllPaths();

  private:
    static std::vector<std::string> dataPaths;
};

template <class ODT>
class DistanceObject
{
  public:
    DistanceObject()
    {
      distance = 0.0f;
    }

    DistanceObject(const ODT &objectArg)
    {
      distance = 0.0f;
      object   = objectArg;
    }

    DistanceObject(const DistanceObject &copy)
    {
      operator=(copy);
    }

    DistanceObject &operator =(const DistanceObject &copy)
    {
      if(this != &copy)
      {
        distance = copy.distance;
        object   = copy.object;
      }
      return *this;
    }

    bool operator ==(const DistanceObject &copy)
    {
       return (distance == copy.distance);
    }

    bool operator >=(const DistanceObject &copy)
    {
       return (distance >= copy.distance);
    }

    bool operator <=(const DistanceObject &copy)
    {
       return (distance <= copy.distance);
    }

    bool operator >(const DistanceObject &copy)
    {
       return (distance > copy.distance);
    }

    bool operator < (const DistanceObject &copy)
    {
       return (distance < copy.distance);
    }

    void setDistance(float dist)
    {
      distance = dist;
    }

    float getDistance() const
    {
     return distance;
    }

    void setObject(const ODT& objectArg)
    {
      object =objectArg;
    }

    ODT &getObject()
    {
      return object;
    }
   private:
     float distance;
     ODT     object;

};

#endif
