#ifndef VEC3_H
#define VEC3_H

#include <math.h>
// #include "utils.h"

//Small vector library
// Represents a vector as 3 floats
#define MIN_T 0.001
#define MAX_T 999999

struct vec3{
  float x,y,z;

  vec3(float x, float y, float z) : x(x), y(y), z(z) {}
  vec3() : x(0), y(0), z(0) {}

  //Clamp each component (used to clamp pixel colors)
  vec3 clampTo1(){
    return vec3(fmin(x,1),fmin(y,1),fmin(z,1));
  }

  //Compute vector length
  long double length(){
    return sqrt(x*x+y*y+z*z);
  }

  //Compute vector length squared
  long double lengthsq(){
    return x*x+y*y+z*z;
  }

  //Create a unit-length vector
  vec3 normalized(){
    float len = sqrt(x*x+y*y+z*z);
    return vec3(x/len,y/len,z/len);
  }

};

//Multiply float and vector
inline const vec3 operator*(const float f, const vec3 a){
  return vec3(a.x*f,a.y*f,a.z*f);
}

inline vec3 operator-(vec3 a){
  return vec3(-a.x,-a.y,-a.z);
}

//Vector-vector dot product
inline const float dot(const vec3 a, const vec3 b){
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

//Vector-vector cross product
inline vec3 cross(vec3 a, vec3 b){
  return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y-a.y*b.x);
}

//Vector addition
inline vec3 operator+(vec3 a, vec3 b){
  return vec3(a.x+b.x, a.y+b.y, a.z+b.z);
}

//Vector subtraction
inline vec3 operator-(vec3 a, vec3 b){
  return vec3(a.x-b.x, a.y-b.y, a.z-b.z);
}

inline float Distance(vec3 a, vec3 b){
  return (a-b).length();
}

inline long double Area(vec3 a, vec3 b, vec3 c){
  return (long double)cross(a-b,c-b).length()/2.0;
}

inline vec3 Centroid(vec3 a, vec3 b, vec3 c){
  return vec3((a.x+b.x+c.x)/3.0, (a.y+b.y+c.y)/3.0, (a.z+b.z+c.z)/3.0);
}

inline float mymin(float a, float b, float c){
    float out = MAX_T;
    out = (out>a)? a:out;
    out = (out>b)? b:out;
    out = (out>c)? c:out;
    return out;
}

inline float mymax(float a, float b, float c){
    float out = -MAX_T;
    out = (out<a)? a:out;
    out = (out<b)? b:out;
    out = (out<c)? c:out;
    return out;
}

inline vec3 MinBound(vec3 a, vec3 b, vec3 c){
  return vec3(mymin(a.x,b.x,c.x),mymin(a.y,b.y,c.y),mymin(a.z,b.z,c.z));
}

inline vec3 MaxBound(vec3 a, vec3 b, vec3 c){
  return vec3(mymax(a.x,b.x,c.x),mymax(a.y,b.y,c.y),mymax(a.z,b.z,c.z));
}

#endif