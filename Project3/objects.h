////

#ifndef OBJECTS_H
#define OBJECTS_H

#include "vec3.h"
#include "image.h"
#include "ray.h"
#include <assert.h>

#define MIN_T 0.0001
#define MAX_T 999999

struct Material{
    Color ac; //ambient color
    Color dc; // diffuse color
    Color sc; //specular color
    int ns; //phong factor
    Color tc; //transmisive color
    float ior; //index of reflection
    //constructor
    Material() : ac(0,0,0), dc(0,0,0), sc(0,0,0), ns(0), tc(0,0,0), ior(0.0) {}
    Material(Color ac_, Color dc_, Color sc_, int ns_, Color tc_, float ior_):
        ac(ac_), dc(dc_), sc(sc_), ns(ns_), tc(tc_), ior(ior_) {}

    void SetAmbientColor(const Color& c) { ac = c;}
    void SetDiffuseColor(const Color& c) {dc = c;}
    void SetSpeculorColor(const Color& c) {sc = c;}
    void SetTransmissiveColor(const Color& c) {tc = c;}
    void SetPhongFactor(const int& n) {ns = n;}
    void SetIOR(const float& f) {ior = f;}
    
    ~Material(){}
};

struct HitInfo{
    // bool hit;
    vec3 hitPos;
    vec3 hitNorm;
    float t;
    vec3 v;
    int objI; //object index
    int rayDepth;
    Material m;
    Color c;
    // vec3 r;
    // vec3 h;
    // vec3 l;    
    HitInfo():hitPos(vec3()),hitNorm(vec3()), t(MAX_T), v(vec3()), objI(-1), rayDepth(0), m(Material()), c(Color()) {}
};

struct Sphere {
public:
    vec3 pos;
    float r;
    Material m;

    //constructor
    Sphere() {pos=vec3(0,0,0); r=0.0; m=Material(); };
    Sphere(vec3 pos_, float r_, Material m_): pos(pos_), r(r_), m(m_) {}
    //Set position
    void set_pos(vec3 pos_){
        pos = pos_;
    }
    ~Sphere(){}
    //Check for hit
    bool Hit(Ray ray){
        float a = dot(ray.d,ray.d); //TODO: What do we know about "a" if "dir" is normalized on creation?
        vec3 toStart = (ray.p - pos);
        float b = 2 * dot(ray.d,toStart);
        float c = dot(toStart,toStart) - r*r;
        float discr = b*b - 4*a*c;
        if (discr > 0) {
            float t = (-b + sqrt(discr))/(2*a);
            if (t>MIN_T) return true;
            t = (-b - sqrt(discr))/(2*a);
            if (t>MIN_T) return true;
        }        
        return false;
    }
    //Check for hit
    bool HitWInfo(Ray ray, HitInfo& hi){
        // d = d.normalized();
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
                hi.v = (-1)*ray.d; //p - hi.hitPos;
                hi.m = m;
                hi.rayDepth = ray.depth;
                return true;
            }         
        }
        return false;
    }
};

struct PointLight {
public:
    Color c;
    vec3 p;

    //constructor
    PointLight(){c=Color();p=vec3();};
    PointLight(Color c_, vec3 p_):
        c(c_),p(p_) {}
    ~PointLight() {}

    //
    Color Shading(HitInfo hi){
        vec3 l = p - hi.hitPos;
        vec3 h = (1/(hi.v + l).length())*(hi.v + l);
        float diffuse_a = dot(hi.hitNorm.normalized(),l.normalized());
        diffuse_a = (diffuse_a>0)? diffuse_a : 0;
        float specular_a = dot(hi.hitNorm.normalized(),h.normalized());
        specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
        float fade_f = 1/((p.x-hi.hitPos.x)*(p.x-hi.hitPos.x)+(p.y-hi.hitPos.y)*(p.y-hi.hitPos.y)+(p.z-hi.hitPos.z)*(p.z-hi.hitPos.z));
        
        Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c * fade_f ;
        return c_out ;
    }
    //Shading
    // Color Shading(Material m, vec3 p0, vec3 n, vec3 l, vec3 h){
    //     float diffuse_a = dot(n.normalized(),l.normalized());
    //     diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    //     float specular_a = dot(n.normalized(),h.normalized());
    //     specular_a = (specular_a>0)? pow(specular_a,m.ns) : 0;
    //     float fade_f = 1/((p.x-p0.x)*(p.x-p0.x)+(p.y-p0.y)*(p.y-p0.y)+(p.z-p0.z)*(p.z-p0.z));
        
    //     Color c_out = (m.dc * diffuse_a + m.sc * specular_a) * c * fade_f ;
    //     Color c1 = m.dc * diffuse_a;
    //     Color c2 = m.sc * specular_a;
    //     Color c3 = c1 + c2;
    //     Color c4 = c3*c;
    //     Color c5 = c4*fade_f;
    //     return c_out ;
    // }
};

struct SpotLight {
    Color c;
    vec3 p, d;
    float a1, a2;

    //constructor
    SpotLight(){c=Color(); p=vec3(0,0,0); d=vec3(0,0,0);a1=0;a2=0;};
    SpotLight(Color c_, vec3 p_, vec3 d_, float a1_, float a2_):
        c(c_),p(p_),d(d_),a1(a1_),a2(a2_) {}
    ~SpotLight(){}

    Color Shading(HitInfo hi){
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
    }    
};

struct DirectionalLight {
    Color c;
    vec3 d;

    DirectionalLight(){c=Color();d=vec3();};
    DirectionalLight(Color c_, vec3 d_):
        c(c_),d(d_) {}
    ~DirectionalLight(){}
    //shading function
    Color Shading(HitInfo hi){
        vec3 h = (1/(hi.v + d).length())*(hi.v + d);
        float diffuse_a = dot(hi.hitNorm.normalized(),d.normalized());
        diffuse_a = (diffuse_a>0)? diffuse_a : 0;
        float specular_a = dot(hi.hitNorm.normalized(),h.normalized());
        specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
        
        Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c ;
        return c_out ;
    }
};

struct AmbientLight {
    Color c;
    AmbientLight(Color c_=Color()):
        c(c_) {}
    ~AmbientLight(){}

    //Shading function returns color
    Color Shading(Material m){
        return m.ac*c;
    }
};

#endif