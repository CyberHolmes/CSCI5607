#include "scene.h"

Scene::Scene() : ambientlight(0,0,0), background(0,0,0), numSpheres(0), numPointlights(0), numSpotlights(0){
    //Reserve spaces
    // std::vector<Sphere> spheres;
    spheres.reserve(100); //reserve 100 sphere space at initialization
    // std::vector<PointLight> pointlights;
    pointlights.reserve(10);
    // std::vector<DirectionalLight> dirlights;
    dirlights.reserve(1);
    // std::vector<SpotLight> spotlights;
    spotlights.reserve(5);
}

void Scene::AddSphere(Sphere sphere){
    spheres.emplace_back(sphere);
    numSpheres++;
};
void Scene::AddSpotlight(SpotLight spotlight){
    spotlights.emplace_back(spotlight);
    numSpotlights++;
};
void Scene::AddPointlight(PointLight pointlight){
    pointlights.emplace_back(pointlight);
    numPointlights++;
};
void Scene::AddDirlight(DirectionalLight dlight){
    dirlights.emplace_back(dlight);
    numDirlights++;
};
void Scene::SetBackColor(Color c){
    background = c;
};
void Scene::SetAmbientLight(Color c){
    ambientlight = c;
};

//Get
// inline Sphere& Scene::GetSphere(int x) { return spheres[x];}

Sphere Scene::GetSphere(int i){
    return spheres[i];
};
PointLight Scene::GetPointlight(int i){
    return pointlights[i];
};
SpotLight Scene::GetSpotLight(int i){
    return spotlights[i];
};
DirectionalLight Scene::GetDirLight(int i){
    return dirlights[i];
};
int Scene::GetNumSpheres(){
    return numSpheres;
};
int Scene::GetNumPointLight(){
    return numPointlights;
};
int Scene::GetNumSpotLight(){
    return numSpotlights;
};
int Scene::GetNumDirLight(){
    return numDirlights;
};
Color Scene::GetAmbientLight(){
    return ambientlight;
};
Color Scene::GetBackground(){
    return background;
};

bool Scene::Hit (vec3 p, vec3 d, int objID){
    bool hit = false;
    for (int i=0; i<=numSpheres; i++){
        // if (i==objID) continue;
        hit = spheres[i].Hit(p,d);
        if (hit) break;
    }
    return hit;
};

bool Scene::HitWInfo (vec3 p, vec3 d, HitInfo& hi){
    HitInfo hi_temp=HitInfo();    
    bool hit = false;
    for (int i=0; i<=numSpheres; i++){
        bool curHit = spheres[i].HitWInfo(p,d,hi_temp);
        if (curHit && (hi_temp.t < hi.t)){
            // printf("sphere=%d, hit.t=%f, t0=%f\n",i, hi_temp.t, hi.t);
            hi = hi_temp;
            hi.objI = i;   
            // printf("sphere=%d, hit.t=%f, t0=%f\n",i, hi_temp.t, hi.t);
            // assert(false);
            hit = true;
        }
    }
    if (hit){
        Color c_out;
        c_out = c_out+ ambientlight*hi.m.ac;
        for (int i=0;i<numPointlights;i++){
            PointLight pl = pointlights[i];
            //check if the ray is blocked from the light source
            if (!Hit(hi.hitPos, pl.p - hi.hitPos, hi.objI)){
                vec3 ldir = pl.p - hi.hitPos;
                vec3 h = (1/(hi.v + ldir).length())*(hi.v + ldir);
                c_out = c_out + pl.Shading(hi.m,hi.hitPos,hi.hitNorm,ldir,h);                
            }
        }
        hi.c = c_out;
    }
    return hit;
}