#ifndef RAY_H
#define RAY_H

#include "vec3.h"
// #include "image_lib.h"

struct Ray{
    vec3 p;
    vec3 d;
    int depth;
    Ray() : p(vec3()), d(vec3()), depth(0) {}
    Ray(vec3 p_, vec3 d_, int depth_) : p(p_), d(d_), depth(depth_) {}
};

#endif