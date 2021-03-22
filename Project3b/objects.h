#ifndef OBJECTS_H
#define OBJECTS_H

#include "vec3.h"
#include "image.h"
#include "ray.h"
#include <assert.h>
#include <vector>
// #include <algorithm>

extern std::vector<vec3> vertexList;
extern std::vector<vec3> normalList;
// #define MIN_T 0.001
// #define MAX_T 999999

struct Material{
    Color ac; //ambient color
    Color dc; // diffuse color
    Color sc; //specular color
    int ns; //phong factor
    Color tc; //transmisive color
    float ior; //index of reflection
    //constructor
    Material() : ac(0,0,0), dc(1,1,1), sc(0,0,0), ns(5), tc(0,0,0), ior(1.0) {}
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

extern std::vector<Material> materialList;

struct HitInfo{
    // bool hit;
    vec3 hitPos;
    vec3 hitNorm;
    float t;
    vec3 v;
    // int objI; //object index
    int rayDepth;
    Material m;
  
    HitInfo():hitPos(vec3()),hitNorm(vec3()), t(MAX_T), v(vec3()), rayDepth(0), m(Material()) {}
    ~HitInfo(){}
};

//base object class
class Obj{
protected:
    int midx;
    vec3 pos; //center position
public:
    // Material m;
    Obj() : midx(0), pos(vec3()){}
    Obj(int m_, vec3 p_) : midx(m_), pos(p_) {}
    vec3 GetPos();
    virtual bool Hit(Ray ray, HitInfo& hi) {return false;};
    virtual vec3 GetBoundMin()=0;
    virtual vec3 GetBoundMax()=0;
};

enum UnionType{
        OR,
        AND,
        NOT
};

class CSG : public Obj{
    Obj* obj1;
    Obj* obj2;
    int ut;
public:    
    CSG() : obj1(NULL), obj2(NULL) {}
    CSG(Obj* obj1_, Obj* obj2_, int ut_) : obj1(obj1_), obj2(obj2_), ut(ut_){
        pos = 0.5*(obj1->GetPos() + obj2->GetPos());}
    ~CSG(){delete obj1; delete obj2; obj1=NULL; obj2=NULL;}
    bool Hit(Ray, HitInfo&) override;
    vec3 GetBoundMin() override;
    vec3 GetBoundMax() override;
};

class Box : public Obj{ //box primitive, axis aligned
    float l_h; //length of the box edges
    // vec3 n; //the 'orientation' of the box
public:
    Box() : Obj(), l_h(0.5){} //default constructor
    Box(int m_, vec3 p_) : Obj(m_, p_), l_h(0.5) {}//, n(vec3(0,1,0)){} // construct a unit volume box
    Box(int m_, vec3 p_, float l_) : Obj(m_, p_), l_h(l_/2.0) {}//, n(vec3(0,1,0)){}
    // Box(int m_, vec3 p_, float l_, vec3 n_) : Obj(m_, p_), l(l_), n(n_){}
    ~Box(){}
    bool Hit(Ray, HitInfo&) override;
    vec3 GetBoundMin() override;
    vec3 GetBoundMax() override;
};

class Plane : public Obj{
    vec3 n; //plane normal
public:
    Plane() : Obj(), n(vec3()) {}
    Plane(int m_, vec3 p_, vec3 n_) : Obj(m_, p_), n(n_) {}
    ~Plane() {}
    bool Hit(Ray, HitInfo&) override;
    vec3 GetBoundMin() override;
    vec3 GetBoundMax() override;
};

class Sphere : public Obj{
    // vec3 pos;
    float r;
public:
    Sphere() : Obj(), r(0){}
    Sphere(int m_, vec3 pos_, float r_) : Obj(m_, pos_), r(r_) {}
    ~Sphere();
    bool Hit(Ray, HitInfo&) override;
    vec3 GetBoundMin() override;
    vec3 GetBoundMax() override;
    float GetR() {return r;}
};

class Triangle : public Obj{
protected:
    int vertexP[3]; // position of the vertex
    //default constructor
public:
    Triangle() : Obj(), vertexP{0,1,2} {} //not meaningful, all three position are the first element of the vertex list
    Triangle(int m_, int p0, int p1, int p2) : Obj(m_, Centroid(vertexList[p0],vertexList[p1],vertexList[p2])), vertexP{p0,p1,p2} {};
    ~Triangle() {} //destructor, do nothing
    bool Hit(Ray, HitInfo&) override;
    vec3 GetBoundMin() override;
    vec3 GetBoundMax() override;
};

class TriangleNormal : public Triangle{
    int normalP[3]; // index of the normals
public:
    TriangleNormal() : Triangle(), normalP{0,1,2} {}
    TriangleNormal(int m_, int p0, int p1, int p2, int n0, int n1, int n2) : Triangle(m_, p0, p1, p2), normalP{n0,n1,n2} {}
    ~TriangleNormal() {}
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