#include "scene.h"

void Scene::AddObject(Obj* o){
    objects.emplace_back(o);
    numObj++;
}

void Scene::AddLight(Light* l){
    lights.emplace_back(l);
    numLights++;
}

Obj* Scene::GetObject(int i){
    return objects[i];
}

Light* Scene::GetLight(int i){
    return lights[i];
}

int Scene::GetNumLights(){
    return numLights;
}

int Scene::GetNumObjects(){
    return numObj;
}

Color Scene::GetAmbientLight(){
    return ambientlight;
};
Color Scene::GetBackground(){
    return background;
};

void Scene::SetBackground(Color c){
    background = c;
};
void Scene::SetAmbientlight(Color c){
    ambientlight = c;
}; 

bool Scene::Hit(const Ray& ray, HitInfo& hi){
    HitInfo hi_temp=HitInfo();    
    bool hit = false;
    for (int i=0; i<numObj; i++){
        bool curHit = objects[i]->Hit(ray,hi_temp);
        if (curHit && (hi_temp.t < hi.t)){
            hi = hi_temp;
            hi.objI = i;   
            hit = true;
        }
    }
    return hit;
};

Color Scene::ApplyLightingModel (Ray& ray, HitInfo& hi){
    Color c_out = Color();
    for (int i=0;i<numLights;i++){
        Light* l = lights[i];       
        Ray ray_shadow = Ray(hi.hitPos, l->CalDir(hi.hitPos), 1);
        HitInfo shadow_hitInfo;
        bool shadow_hit = Hit(ray_shadow,shadow_hitInfo);
        //check if the ray is blocked from the light source
        if (l->type == DIRECTION_LIGHT){
            if (shadow_hit) continue;
        } else if (shadow_hit && shadow_hitInfo.t < Distance(l->p, hi.hitPos )) 
            continue;
        c_out = c_out + l->Shading(hi);
    }
    if (hi.rayDepth>0){
        float ctheta = dot(ray.d,hi.hitNorm); //cos of angle between ray and hit normal
        //Reflection
        vec3 rdir = ray.d - 2 * ctheta *hi.hitNorm;
        Ray ray_reflect = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
        c_out = c_out + hi.m.sc * EvaluateRayTree(ray_reflect);
        //Refraction
        float eta = (ctheta<0)?1.0/hi.m.ior:hi.m.ior;
        vec3 n = (ctheta<0)? hi.hitNorm : -hi.hitNorm;
        ctheta = (ctheta>0)? -ctheta : ctheta;
        float eta_sq = eta * eta;
        float rf_term = 1 - eta_sq + ctheta * ctheta * eta_sq;
        if (rf_term > 0) { //there is refration
            rdir = (-sqrt(rf_term) - ctheta * eta) * n + eta * ray.d;
            Ray ray_refract = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
            c_out = c_out + hi.m.tc * EvaluateRayTree(ray_refract);
        }
    }
    c_out = c_out+ ambientlight * hi.m.ac;
    // contribution += Emission( hit ); // for area light sources only
    return c_out;
};
Color Scene::EvaluateRayTree(Ray& ray){
    bool hit;
    HitInfo hitInfo; // structure containing hit point, normal, etc
    hit = Hit(ray, hitInfo );
    if (hit){
        return ApplyLightingModel(ray, hitInfo );
    }else{
        return GetBackground();
    }
};