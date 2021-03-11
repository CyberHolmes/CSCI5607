#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"

struct Camera
{
   vec3 eye; 
    vec3 forward;
    vec3 up;
    vec3 right;
    float fov_h;
};
#endif