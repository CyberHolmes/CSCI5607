#ifndef RAY_H
#define RAY_H

#include "vec3.h"
// #include "image_lib.h"

struct Ray{
    vec3 eye;
    vec3 dir;
    int depth;
};

#endif