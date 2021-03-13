#ifndef OBJECTS_H
#define OBJECTS_H

#include "vec3.h"
#include "image.h"
#include "ray.h"
#include <assert.h>
// #include "camera.h"

#define MIN_T 0.0001
#define MAX_T 999999
// extern Camera* camera;

struct Material{
    Color ac; //ambient color
    Color dc; // diffuse color
    Color sc; //specular color
    int ns; //phong factor
    Color tc; //transmisive color
    float ior; //index of reflection
    //constructor
    Material() : ac(0,0,0), dc(1,1,1), sc(0,0,0), ns(5), tc(0,0,0), ior(0.0) {}
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
  
    HitInfo():hitPos(vec3()),hitNorm(vec3()), t(MAX_T), v(vec3()), objI(-1), rayDepth(0), m(Material()) {}
    ~HitInfo(){}
};

//base object class
class Obj{
public:
    Material m;
    Obj() : m(Material()){}
    Obj(Material m_) : m(m_) {}
    virtual bool Hit(Ray ray, HitInfo& hi) {return false;};
};

class Sphere : public Obj{
    vec3 pos;
    float r;
public:
    Sphere() : Obj(), pos(vec3()), r(0){}
    Sphere(Material m_, vec3 pos_, float r_) : Obj(m_), pos(pos_), r(r_) {}
    ~Sphere();
    bool Hit(Ray, HitInfo&) override;
};

enum LightType: unsigned char{
    AMBIENT_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
    DIRECTION_LIGHT
};

class Light{
    static int id;    
public:
    LightType type;
    Color c;
    vec3 p;
    
    Light(LightType t_, Color c_, vec3 p_) : type(t_), c(c_), p(p_) {}
    ~Light(){}    
    virtual Color Shading(HitInfo) {return Color();}
    //return direction to the light from a point
    virtual vec3 CalDir(vec3 p0) {return vec3();}
};

class PointLight : public Light{
public:
    PointLight() : Light(POINT_LIGHT,Color(), vec3()) {}
    PointLight(Color c_, vec3 p_): Light(POINT_LIGHT,c_, p_){}
    ~PointLight();
    Color Shading(HitInfo) override;
    vec3 CalDir(vec3 p0) override;
};

class SpotLight : public Light{
    vec3 d; //position and direction
    float a1, a2; //angles
public:
    SpotLight() : Light(SPOT_LIGHT, Color(), vec3()), d(vec3()), a1(0), a2(0) {};
    SpotLight(Color c_, vec3 p_, vec3 d_, float a1_, float a2_):
        Light(SPOT_LIGHT, c_, p_),d(d_),a1(a1_),a2(a2_) {}
    ~SpotLight(){}

    Color Shading(HitInfo hi) override;  
    vec3 CalDir(vec3 p0) override;
};

class DirectionLight : public Light{
    vec3 d; //direction
public:
    DirectionLight() : Light(DIRECTION_LIGHT, Color(), vec3()), d(vec3()) {};
    DirectionLight(Color c_, vec3 d_):
        Light(DIRECTION_LIGHT, c_, vec3()),d(d_) {}
    ~DirectionLight(){}

    Color Shading(HitInfo hi) override;
    vec3 CalDir(vec3 p0) override;
};

#endif