#include "objects.h"

bool Sphere::Hit(Ray ray, HitInfo& hi) {
    float a = dot(ray.d,ray.d);
    vec3 toStart = (ray.p - pos);
    float b = 2 * dot(ray.d,toStart);
    float c = dot(toStart,toStart) - r*r;
    float discr = b*b - 4*a*c;
    if (discr > 0){
        float t1 = (-b + sqrt(discr))/(2*a);
        float t2 = (-b - sqrt(discr))/(2*a);
        t1 = (t1 > MIN_T)? t1 : MAX_T;
        t2 = (t2 > MIN_T)? t2 : MAX_T;
        hi.t = (t1 < t2)? t1 : t2;
        if (hi.t < MAX_T){                
            hi.hitPos = ray.p + hi.t*ray.d;
            hi.hitNorm = (hi.hitPos-pos).normalized();
            hi.v = -ray.d; //p - hi.hitPos;
            hi.m = m;
            hi.rayDepth = ray.depth;
            return true;
        }         
    }
    return false;
};

Color PointLight::Shading(HitInfo hi) {
    vec3 l = (p - hi.hitPos).normalized();
    vec3 h = (1/(hi.v + l).length())*(hi.v + l);
    float diffuse_a = dot(hi.hitNorm,l);
    diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    float specular_a = dot(hi.hitNorm,h.normalized());
    specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
    float fade_f = 1/((p.x-hi.hitPos.x)*(p.x-hi.hitPos.x)+(p.y-hi.hitPos.y)*(p.y-hi.hitPos.y)+(p.z-hi.hitPos.z)*(p.z-hi.hitPos.z));
    
    Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c * fade_f ;
    return c_out ;
};

vec3 PointLight::CalDir(vec3 p0){
    return (p-p0).normalized();
}

Color SpotLight::Shading(HitInfo hi) {
    Color c_out;
    vec3 l = p - hi.hitPos;
    vec3 h = (1/(hi.v + l).length())*(hi.v + l);
    // angle between the direction of the light and the direction to the hit
    float angle = dot((-1)*l.normalized(), d.normalized());
    // check which side the hit direction is on
    vec3 v_temp = cross(d.normalized(), l.normalized());
    float a = (dot(v_temp, vec3(0,1,0))>0)? a1 : a2; //assume camera will never be upside down
    if (angle > a){
        float diffuse_a = dot(hi.hitNorm.normalized(),l.normalized());
        diffuse_a = (diffuse_a>0)? diffuse_a : 0;
        float specular_a = dot(hi.hitNorm.normalized(),h.normalized());
        specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
        float fade_f = 1/((p.x-hi.hitPos.x)*(p.x-hi.hitPos.x)+(p.y-hi.hitPos.y)*(p.y-hi.hitPos.y)+(p.z-hi.hitPos.z)*(p.z-hi.hitPos.z));            
        c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c * fade_f ;
    } else {
        c_out = Color(0,0,0);
    }
    return c_out ;
};

vec3 SpotLight::CalDir(vec3 p0){
    return (p-p0).normalized();
}

Color DirectionLight::Shading(HitInfo hi) {
    vec3 h = (1/(hi.v + -d).length())*(hi.v + -d);
    float diffuse_a = dot(hi.hitNorm.normalized(),-d.normalized());
    diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    float specular_a = dot(hi.hitNorm.normalized(),h.normalized());
    specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
    
    Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c ;
    return c_out ;
};

vec3 DirectionLight::CalDir(vec3 p0){
    return -d.normalized();
}