#include "vec3.h"
#include "image_lib.h"

struct Material {
public:
    float ar,ag,ab;
    float dr,dg,db;
    float sr,sg,sb;
    int ns;
    float tr,tg,tb,ior;

    //constructor
    // Material(){ar=0;  ag=0;  ab=0; dr=1;  dg=1;  db=1; sr=0; sg=0; sb=0;  int ns=5; tr=0;  tg=0;  tb=0;  ior=0;}
    Material(float ar_=0, float ag_=0, float ab_=0,
        float dr_=1, float dg_=1, float db_=1,
        float sr_=0,float sg_=0,float sb_=0, int ns_=5,
        float tr_=0, float tg_=0, float tb_=0, float ior_=0):
        ar(ar_),ag(ag_),ab(ab_),dr(dr_),dg(dg_),db(db_),
        sr(sr_),sg(sg_),sb(sb_),ns(ns_),tr(tr_),tg(tg_),tb(tb_),ior(ior_){}
    
    ~Material(){}
    //Set
    void set(float ar_, float ag_, float ab_,
        float dr_, float dg_, float db_,
        float sr_, float sg_, float sb_, int ns_,
        float tr_, float tg_, float tb_, float ior_){
        ar=ar_; ag=ag_; ab=ab_; dr=dr_; dg=dg_; db=db_;
        sr=sr_; sg=sg_; sb=sb_; ns=ns_; tr=tr_; tg=tg_; tb=tb_; ior=ior_;
    }
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
    //Set radius
    void set_r(float r_){
        r = r_;
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
};