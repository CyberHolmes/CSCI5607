
#include <math.h>
#include "vec3.h"
#define MIN_T 0.000001

struct HitInfo{
    bool hit;
    vec3 hitPos;
    vec3 hitNorm;
    vec3 r;
    vec3 h;
    vec3 l;
    float t;

    HitInfo():hit(false),hitPos(vec3()),hitNorm(vec3()), r(vec3()), h(vec3()), l(vec3()), t(0) {}
};

//Tests is the ray intersects the sphere
HitInfo raySphereIntersect(vec3 start, vec3 dir, vec3 center, float radius){
    HitInfo hitInfo = HitInfo();
  float a = dot(dir,dir); //TODO: What do we know about "a" if "dir" is normalized on creation?
  vec3 toStart = (start - center);
  float b = 2 * dot(dir,toStart);
  float c = dot(toStart,toStart) - radius*radius;
  float discr = b*b - 4*a*c;
  if (discr > 0){
    float t0 = (-b + sqrt(discr))/(2*a);
    if (t0>MIN_T) {
        hitInfo.t=t0;
        hitInfo.hit=true;
        hitInfo.hitPos = t0*dir;
        hitInfo.hitNorm = (hitInfo.hitPos-center).normalized();
        hitInfo.r = dir - 2 * dot(dir,hitInfo.hitNorm) * hitInfo.hitNorm;
        hitInfo.h = (hitInfo.r - dir).normalized();
        hitInfo.l=dir;
        return hitInfo;};
    float t1 = (-b - sqrt(discr))/(2*a);
    if (t1>MIN_T) { hitInfo.t=t1;
        hitInfo.hit=true;
        hitInfo.hitPos = t1*dir;
        hitInfo.hitNorm = hitInfo.hitPos-center;
        hitInfo.r = dir - 2 * dot(dir,hitInfo.hitNorm) * hitInfo.hitNorm;
        hitInfo.h = (hitInfo.r - dir).normalized();
        hitInfo.l=dir;
        return hitInfo;};
  }
  return hitInfo;
}

// bool raySphereIntersect(vec3 start, vec3 dir, vec3 center, float radius, float *t){
//   float a = dot(dir,dir); //TODO: What do we know about "a" if "dir" is normalized on creation?
//   vec3 toStart = (start - center);
//   float b = 2 * dot(dir,toStart);
//   float c = dot(toStart,toStart) - radius*radius;
//   float discr = b*b - 4*a*c;
//   if (discr < 0) return false;
//   else{
//     float t0 = (-b + sqrt(discr))/(2*a);
//     if (t0>MIN_T) { *t=t0; return true;};
//     float t1 = (-b - sqrt(discr))/(2*a);
//     if (t1>MIN_T) { *t=t1; return true;};
//   }
//   return false;
// }