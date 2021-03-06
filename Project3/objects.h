////

#ifndef OBJECTS_H
#define OBJECTS_H

#include "vec3.h"
#include "image.h"
#include "ray.h"
#include <assert.h>

#define MIN_T 0.0001

struct HitInfo{
    // bool hit;
    vec3 hitPos;
    vec3 hitNorm;
    float t;
    vec3 v;
    int objI; //object index
    Color c;
    // vec3 r;
    // vec3 h;
    // vec3 l;    
    HitInfo():hitPos(vec3()),hitNorm(vec3()), t(99999), v(vec3()), objI(-1), c(Color()) {}
};

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
    bool Hit(vec3 p, vec3 d){
        float a = dot(d,d); //TODO: What do we know about "a" if "dir" is normalized on creation?
        vec3 toStart = (p - pos);
        float b = 2 * dot(d,toStart);
        float c = dot(toStart,toStart) - r*r;
        float discr = b*b - 4*a*c;
        if (discr > 0) {
            float t = (-b + sqrt(discr))/(2*a);
            // printf("t=%f\n",t);
            // assert(false);
            if (t>MIN_T) return true;
            t = (-b - sqrt(discr))/(2*a);
            if (t>MIN_T) return true;
        }        
        return false;
    }
    //Check for hit
    bool HitWInfo(vec3 p, vec3 d, HitInfo& hi){
        // d = d.normalized();
        float a = dot(d,d); //TODO: What do we know about "a" if "dir" is normalized on creation?
        vec3 toStart = (p - pos);
        float b = 2 * dot(d,toStart);
        float c = dot(toStart,toStart) - r*r;
        float discr = b*b - 4*a*c;
        if (discr > 0){
            float t = (-b + sqrt(discr))/(2*a);
            t = (t>MIN_T)? t : (-b - sqrt(discr))/(2*a);
            if (t>MIN_T){
                hi.t=t;
                hi.hitPos = p + t*d;
                hi.hitNorm = (hi.hitPos-pos).normalized();
                hi.v = (-1)*d; //p - hi.hitPos;
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

    //Shading
    Color Shading(Material m, vec3 p0, vec3 n, vec3 l, vec3 h){
        float diffuse_a = dot(n.normalized(),l.normalized());
        diffuse_a = (diffuse_a>0)? diffuse_a : 0;
        float specular_a = dot(n.normalized(),h.normalized());
        specular_a = (specular_a>0)? pow(specular_a,m.ns) : 0;
        float fade_f = 1/((p.x-p0.x)*(p.x-p0.x)+(p.y-p0.y)*(p.y-p0.y)+(p.z-p0.z)*(p.z-p0.z));
        
        Color c_out = (m.dc * diffuse_a + m.sc * specular_a) * c * fade_f ;
        Color c1 = m.dc * diffuse_a;
        Color c2 = m.sc * specular_a;
        Color c3 = c1 + c2;
        Color c4 = c3*c;
        Color c5 = c4*fade_f;
        // if (diffuse_a>0.2){
        // printf("diff_a=%f\n",diffuse_a);
        // printf("spec_a=%f\n",specular_a);
        // printf("fade_f=%f\n",fade_f);
        // // printf("n=%f,%f,%f\n",n.x,n.y,n.z);
        // // printf("l=%f,%f,%f\n",l.x,l.y,l.z);
        // // printf("h=%f,%f,%f\n",h.x,h.y,h.z);
        // printf("m.dc=%f,%f,%f\n",m.dc.r,m.dc.g,m.dc.b);
        // printf("m.sc=%f,%f,%f\n",m.sc.r,m.sc.g,m.sc.b);
        // printf("m.sc=%f,%f,%f\n",m.sc.r,m.sc.g,m.sc.b);
        // printf("c1=%f,%f,%f\n",c1.r,c1.g,c1.b);
        // printf("c2=%f,%f,%f\n",c2.r,c2.g,c2.b);
        // printf("c3=%f,%f,%f\n",c3.r,c3.g,c3.b);
        // printf("c4=%f,%f,%f\n",c4.r,c4.g,c4.b);
        // printf("c5=%f,%f,%f\n",c5.r,c5.g,c5.b);
        // printf("c=%f,%f,%f\n",c.r,c.g,c.b);
        // printf("c=%f,%f,%f\n",c_out.r,c_out.g,c_out.b);
        // assert(false);
        // }
        return c_out ;
    }
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

    
};

struct DirectionalLight {
    Color c;
    vec3 d;

    DirectionalLight(){c=Color();d=vec3();};
    DirectionalLight(Color c_, vec3 d_):
        c(c_),d(d_) {}
    ~DirectionalLight(){}
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