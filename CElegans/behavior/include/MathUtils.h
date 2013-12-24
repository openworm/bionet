/*
 *Author: Abdul Bezrati
 *Email : abezrati@hotmail.com
 */

#ifndef MATHUTILS
#define MATHUTILS


#include <cmath>
#include <ctime>
#include <float.h>
#include <stdlib.h>
#include <iostream>

#define TWO_PI          6.2831853f
#define EPSILON         0.0001f
#define EPSILON_SQUARED EPSILON*EPSILON
#define RAD2DEG         57.2957f
#define DEG2RAD         0.0174532f

using  namespace std;

template <class T>
inline T clamp(T x, T min, T max)
{
  return (x < min) ? min : (x > max) ? max : x;
}

inline float getNextRandom(){
  return (float)((double)rand()/ ((double)RAND_MAX + (double)1));
}

inline int getClosest(int arg, int firstVal, int secondVal)
{
  int difference1 = 0,
      difference2 = 0;

  difference1 = abs(arg - firstVal);
  difference2 = abs(arg - secondVal);
  if(difference1 < difference2)
    return firstVal;

  return secondVal;
}

inline int getClosestPowerOfTwo(int digit)
{
  if(!digit)
    return 1;

  double log2  = log(double(abs(digit)))/log(2.0),
         flog2 = floor(log2),
         frac  = log2 - flog2;

  return (frac < 0.5) ? (int)pow(2.0, flog2) : (int)pow(2.0, flog2 + 1.0);
}
#ifdef WIN32
inline float fastSquareRoot(float x)
{
  __asm{
    fld x;
    fsqrt;
    fstp x;
  }
  return x;
}


inline float fastCos(float x)
{
  __asm{
    fld x;
    fcos;
    fstp x;
  }
  return x;
}

inline float fastSin(float x)
{
  __asm{
    fld x;
    fsin;
    fstp x;
  }
  return x;
}

#else
inline float fastSin(float x)
{ return sin(x);}
inline float fastCos(float x)
{ return cos(x);}
inline float fastSquareRoot(float x)
{ return sqrt(x);}
#endif


/*inline float fastSquareRootSSE(float f)	{
  __asm    {
   MOVSS xmm2,f
   SQRTSS xmm1, xmm2
   MOVSS f,xmm1
  }
  return f;
}*/

template <class T>
class Tuple2
{
  public:
    Tuple2(const T& X = 0, const T& Y = 0)
    {
      x = X;
      y = Y;
    }

    Tuple2(const Tuple2& source)
    {
      x = source.x;
      y = source.y;
    }

    inline Tuple2& operator = (const Tuple2& source)
    {
      x = source.x;
      y = source.y;
      return *this;
    }

    inline Tuple2 operator + (const Tuple2& right)
    {
      return Tuple2(right.x + x, right.y + y);
    }

    inline Tuple2 operator - (const Tuple2 &right)
    {
      return Tuple2(-right.x + x, -right.y + y);
    }

    inline Tuple2 operator * (const T& scale)
    {
      return Tuple2(x*scale, y*scale);
    }

    inline Tuple2 operator / (const T& scale)
    {
      return scale ? Tuple2(x/scale, y/scale) : Tuple2();
    }

    inline Tuple2 &operator += (const Tuple2 &right)
    {
      x+=right.x;
      y+=right.y;
      return *this;
    }

    inline Tuple2 &operator -= (const Tuple2 &right)
    {
      x-=right.x;
      y-=right.y;
      return *this;
    }

    inline Tuple2 &operator *= (const T& scale)
    {
      x*=scale;
      y*=scale;
      return *this;
    }

    inline Tuple2 &operator /= (const T& scale)
    {
      if(scale)
      {
        x/=scale;
        y/=scale;
      }
      return *this;
    }

    inline operator const T*() const { return &x; }
    inline operator T*()             { return &x; }

    inline const T  operator[](int i) const { return ((T*)&x)[i]; }
    inline       T &operator[](int i)       { return ((T*)&x)[i]; }

    inline bool operator == (const Tuple2& right)
    {
      return (x == right.x && y == right.y);
    }

    inline bool operator != (const Tuple2& right)
    {
      return !(x == right.x &&  y == right.y );
    }

    inline void set(const T& nx, const T& ny)
    {
      x = nx;
      y = ny;
    }

    inline Tuple2& clamp(const T& min, const T& max)
    {
      x = x > max ? max : x < min ? min  : x;
      y = y > max ? max : y < min ? min  : y;
      return *this;
    }

    friend std::ostream & operator<< ( std::ostream& streamOut, const Tuple2 & right){
      return streamOut << "Tuple2( " << right.x << ", " << right.y << ")\n";
    }

    T x, y;
};

typedef Tuple2<int   > Tuple2i;
typedef Tuple2<float > Tuple2f;
typedef Tuple2<double> Tuple2d;

template <class T>
class Tuple3
{
  public:
    Tuple3(const T& nx = 0, const T& ny = 0, const T& nz = 0)
    {
      x = nx;
      y = ny;
      z = nz;
    }

    Tuple3(const T& xyz)
    {
      x =
      y =
      z = xyz;
    }

    Tuple3(const Tuple3& source)
    {
      x = source.x;
      y = source.y;
      z = source.z;
    }

    Tuple3(const Tuple2<T>& source, const T& nz = 1)
    {
      x = source.x;
      y = source.y;
      z = nz;
    }

    inline Tuple3 &operator = (const Tuple3& right)
    {
      x = right.x;
      y = right.y;
      z = right.z;
      return *this;
    }

    inline Tuple3 operator + (const Tuple3& right)
    {
      return Tuple3(right.x + x, right.y + y, right.z + z);
    }

    inline Tuple3 operator - (const Tuple3& right)
    {
      return Tuple3(-right.x + x, -right.y + y, -right.z + z);
    }

    inline Tuple3 operator * (const T& scale)
    {
      return Tuple3(x*scale, y*scale, z*scale);
    }

    inline Tuple3 operator / (const T& scale)
    {
      return scale? Tuple3f(x/scale, y/scale, z/scale) : Tuple3();
    }

    inline Tuple3& operator += (const Tuple3& right)
    {
      x+=right.x;
      y+=right.y;
      z+=right.z;
      return *this;
    }

    inline Tuple3& operator += (const T& xyz)
    {
      x += xyz;
      y += xyz;
      z += xyz;
      return *this;
    }

    inline Tuple3& operator -= (const Tuple3& right)
    {
      x-=right.x;
      y-=right.y;
      z-=right.z;
      return *this;
    }

    inline Tuple3& operator -= (const T& xyz)
    {
      x -= xyz;
      y -= xyz;
      z -= xyz;
      return *this;
    }

    inline Tuple3& operator *= (const T& scale)
    {
      x*=scale;
      y*=scale;
      z*=scale;
      return *this;
    }

    inline Tuple3& operator /= (const T& scale)
    {
      if(scale)
	  {
        x/=scale;
        y/=scale;
        z/=scale;
      }
      return *this;
    }

    bool operator == (const Tuple3& right)
    {
      return (x == right.x && y == right.y &&  z == right.z);
    }

    bool operator != (const Tuple3& right)
    {
      return !(x == right.x && y == right.y && z == right.z);
    }

    inline operator const T*() const { return &x; }
    inline operator T*()             { return &x; }

	inline const T  operator[](int i) const { return ((T*)&x)[i]; }
	inline       T &operator[](int i)       { return ((T*)&x)[i]; }

    inline void set(const T& nx, const T& ny, const T& nz)
    {
      x = nx;
      y = ny;
      z = nz;
    }

    inline void set(const Tuple2<T>& vec, const T& Z)
    {
      x = vec.x;
      y = vec.y;
      z = Z;
    }

    inline void set(const T& xyz)
    {
      x =
      y =
      z = xyz;
    }

    inline void set(const Tuple3& t)
    {
      x = t.x;
      y = t.y;
      z = t.z;
    }

    inline Tuple3 &normalize()
    {
      T length  = sqrtf(x*x + y*y + z*z);

      if(!length){
        set(0,0,0);
        return *this;
      }
      x/=length;
      y/=length;
      z/=length;
      return *this;
    }

    inline T getLengthSquared() const { return x*x + y*y + z*z; }
    inline T getLength()        const { return sqrtf(x*x + y*y + z*z); }

    inline T getDotProduct(const Tuple3& t) const
    {
      return x*t.x + y*t.y + z*t.z;
    }

    inline Tuple3 operator ^(const Tuple3 &t)
    {
      return Tuple3(y   * t.z - z   * t.y,
                    t.x * z   - t.z * x,
                    x   * t.y - y   * t.x);
    }

    inline Tuple3 &operator ^=(const Tuple3 &t)
    {
       set(y   * t.z - z   * t.y,
           t.x * z   - t.z * x,
           x   * t.y - y   * t.x);
       return *this;
    }

    inline Tuple3 &crossProduct(const Tuple3& t1, const Tuple3& t2)
    {
      set(t1.y * t2.z - t1.z * t2.y,
          t2.x * t1.z - t2.z * t1.x,
          t1.x * t2.y - t1.y * t2.x);
      return *this;
    }

    inline T getDistance(const Tuple3& v2) const
    {
      return sqrtf((v2.x - x) * (v2.x - x) +
                   (v2.y - y) * (v2.y - y) +
                   (v2.z - z) * (v2.z - z));
    }

    inline T getAngle(const Tuple3& v2) const
    {

      T angle = acos(getDotProduct(v2) / (getLength() * v2.getLength()));
      if(_isnan(angle))
        return 0;
      return angle;
    }

    inline Tuple3& clamp(const T& min, const T& max)
    {
      x = x > max ? max : x < min ? min  : x;
      y = y > max ? max : y < min ? min  : y;
      z = z > max ? max : z < min ? min  : z;
      return *this;
    }

    friend std::ostream & operator << (std::ostream & streamOut, const Tuple3 & right){
      return streamOut << "Tuple(" << right.x << ", " << right.y << ", " << right.z <<")";
    }

    T x, y, z;
};

typedef Tuple3<int>           Tuple3i;
typedef Tuple3<float>         Tuple3f;
typedef Tuple3<double>        Tuple3d;
typedef Tuple3<char>          Tuple3b;
typedef Tuple3<unsigned char> Tuple3ub;

template <class T>
class Tuple4
{
  public:
    Tuple4(const T& X = 0, const T& Y = 0, const T& Z = 0, const T& W = 0)
    {
      x = X;
      y = Y;
      z = Z;
      w = W;
    }

    Tuple4(const T& XYZW)
    {
      x =
      y =
      z =
      w = XYZW;
    }


    Tuple4(const Tuple4& source){
      x = source.x;
      y = source.y;
      z = source.z;
      w = source.w;
    }

    Tuple4(const Tuple3<T>& source, const T& Z)
    {
      x = source.x;
      y = source.y;
      z = source.z;
      w = Z;
    }

    inline operator const T*() const { return &x; }
    inline operator T*()             { return &x; }

	  inline const T  operator[](int i) const { return ((T*)&x)[i]; }
	  inline       T &operator[](int i)       { return ((T*)&x)[i]; }

    inline Tuple4 &operator = (const Tuple4& source)
    {
      x = source.x;
      y = source.y;
      z = source.z;
      w = source.w;
      return *this;
    }

    inline Tuple4 &operator = (const Tuple3<T> &source)
    {
      x = source.x;
      y = source.y;
      z = source.z;
      w = 1.0f;
      return *this;
    }

    inline Tuple4 operator + (const Tuple4& right)
    {
      return Tuple4(right.x + x, right.y + y, right.z + z, right.w + w );
    }

    inline Tuple4 operator - (const Tuple4& right)
    {
      return Tuple4(-right.x + x, -right.y + y, -right.z + z, -right.w + w );
    }

    inline Tuple4 operator * (const T& scale)
    {
      return scale? Tuple4(x*scale, y*scale, z*scale, w*scale) : Tuple4();
    }

    inline Tuple4 operator / (const T& scale)
    {
      return scale? Tuple4(x/scale, y/scale, z/scale, w/scale) : Tuple4();
    }

    inline Tuple4& operator += (const Tuple4 &right)
    {
      x +=right.x;
      y +=right.y;
      z +=right.z;
      w +=right.w;
      return *this;
    }

    inline Tuple4& operator -= (const Tuple4 &right)
    {
      x-=right.x;
      y-=right.y;
      z-=right.z;
      w-=right.w;
      return *this;
    }

    inline Tuple4& clamp(const T& min, const T& max)
    {
      x = x < min ? min : x > max ? max : x;
      y = y < min ? min : y > max ? max : y;
      z = z < min ? min : z > max ? max : z;
      w = w < min ? min : w > max ? max : w;
      return *this;
    }

    inline Tuple4& operator *= (const T& scale)
    {
      x*=scale;
      y*=scale;
      z*=scale;
      w*=scale;
      return *this;
    }

    inline Tuple4& operator /= (const T& scale)
    {
      if(scale)
      {
        x/=scale;
        y/=scale;
        z/=scale;
        w/=scale;
      }
      return *this;
    }

    inline bool operator == (const Tuple4& right)
    {
      return (x == right.x && y == right.y &&
              z == right.z && w == right.w);
    }

    inline bool operator != (const Tuple4& right)
    {
      return  (x != right.x ||  y != right.y ||
               z != right.z ||  w != right.w);
    }

    inline void set(const T& xyzw)
    {
      x =
      y =
      z =
      w = xyzw;
    }

    inline void set(const T& nx, const T& ny, const T& nz, const T& nw)
    {
      x = nx;
      y = ny;
      z = nz;
      w = nw;
    }

    inline void set(const Tuple4& vec)
    {
      x = vec.x;
      y = vec.y;
      z = vec.z;
      w = vec.w;
    }

    inline void set(const Tuple3<T>& vec, const T& W = 1)
    {
      x = vec.x;
      y = vec.y;
      z = vec.z;
      w = W;
    }

    friend std::ostream & operator<< ( std::ostream & streamOut, const Tuple4& right)
    {
      return streamOut << "Tuple4( " << right.x  << ", " << right.y
                                     << ", "
                                     << right.z  << ", " << right.w << ")\n";
    }

    T x, y, z, w;
};

typedef Tuple4<int   > Tuple4i;
typedef Tuple4<float > Tuple4f;
typedef Tuple4<double> Tuple4d;

#endif
