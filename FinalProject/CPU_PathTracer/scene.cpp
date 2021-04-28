#include "scene.h"

void Scene::AddObject(Obj* o){
    objects.emplace_back(o);
    numObj++;
}

void Scene::AddLight(Light* l){
    lights.emplace_back(l);
    numLights++;
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

// bool Scene::Hit(const Ray& ray, HitInfo& hi){
//     HitInfo hi_temp=HitInfo();    
//     bool hit = false;
//     //for (std::list<int>::iterator it=mylist.begin(); it != mylist.end(); ++it)
//     for (int i=0; i<numObj; i++){
//         bool curHit = objects[i]->Hit(ray,hi_temp);
//         if (curHit && (hi_temp.t < hi.t)){
//             // printf("hit\n");assert(false);
//             hi = hi_temp;
//             // hi.objI = i;   
//             hit = true;
//         }
//     }
//     return hit;
// };

Color Scene::ApplyLightingModel (Ray& ray, HitInfo& hi, BoundingBox* BB){
    Color c_out = Color();
    for (int i=0; i<numLights; i++){
        Light* l = lights[i];       
        Ray ray_shadow = Ray(hi.hitPos, l->CalDir(hi.hitPos), 1);
        HitInfo shadow_hitInfo;
        // bool shadow_hit = Hit(ray_shadow,shadow_hitInfo);
        bool shadow_hit = SearchBVHTree(ray_shadow, BB, shadow_hitInfo);
        //check if the ray is blocked from the light source
        if (l->type == DIRECTION_LIGHT){
            if (shadow_hit) continue;
        } else if (shadow_hit && shadow_hitInfo.t < Distance(l->p, hi.hitPos )) 
            continue;
        c_out = c_out + l->Shading(hi);        
    }
    if (hi.rayDepth>0){
        Color c_reflect = Color(0,0,0);
        float ctheta = dot(ray.d,hi.hitNorm); //cos of angle between ray and hit normal
        //Reflection
        vec3 rdir = ray.d - 2 * ctheta *hi.hitNorm;
        Ray ray_reflect = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
        // c_out = c_out + hi.m.sc * EvaluateRayTree(ray_reflect, BB);
        c_reflect = c_reflect + hi.m.sc * EvaluateRayTree(ray_reflect, BB);

        //Indirect lighting
        // float rotMat[3][3] =
        //     {{0.0, -hi.hitNorm.z, hi.hitNorm.y}, 
        //     {hi.hitNorm.z, 0.0, -hi.hitNorm.x},
        //     {-hi.hitNorm.y, -hi.hitNorm.x, 0.0}
        // };
        
        int numPaths = 5;
        int numBounces = hi.rayDepth-1;
        
        for (int i=0; i<numPaths; i++){
            float th =drand48()*M_PI * 2.0;
            float phi = acos(1-2*drand48());
            float x = sin(phi)*cos(th);
            float y = sin(phi)*sin(th);
            float z = cos(phi);
            vec3 new_dir = vec3(x,y,z);
            if (dot(new_dir,hi.hitNorm)<0) new_dir = -new_dir;
            Ray ray_indirect = Ray(hi.hitPos, new_dir, numBounces);
            c_reflect = c_reflect + hi.m.sc * EvaluateRayTree(ray_indirect, BB);
            // printf("norm=%.2f,%.2f,%.2f\n",norm.x,norm.y, norm.z);
            // printf("th=%f,r=%.4f,%.4f,%.4f, c=%.2f,%.2f,%.2f\n",th,new_dir.x,new_dir.y,new_dir.z,c_reflect.r,c_reflect.g,c_reflect.b);
        }
        // c_ind = Color(1.0,1.0,1.0);
        // printf("c=%.2f,%.2f,%.2f\n",c_ind.r,c_ind.g,c_ind.b);
        // c_ind = c_ind*(1.0/float(numPaths));
        // printf("c=%.2f,%.2f,%.2f\n",c_ind.r,c_ind.g,c_ind.b);
        // assert(false);
        c_out = c_out + c_reflect*(1.0/(float(numPaths+1)));

        //Refraction
        float eta = (ctheta<0)?1.0/hi.m.ior:hi.m.ior;
        vec3 n = (ctheta<0)? hi.hitNorm : -hi.hitNorm;
        ctheta = (ctheta>0)? -ctheta : ctheta;
        float eta_sq = eta * eta;
        float rf_term = 1 - eta_sq + ctheta * ctheta * eta_sq;
        if (rf_term > 0) { //there is refration
            rdir = (-sqrt(rf_term) - ctheta * eta) * n + eta * ray.d;
            Ray ray_refract = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
            c_out = c_out + hi.m.tc * EvaluateRayTree(ray_refract, BB);
        }
    }
    c_out = c_out+ ambientlight * hi.m.ac;
    return c_out;
};
Color Scene::EvaluateRayTree(Ray& ray, BoundingBox* BB){
    bool hit;
    HitInfo hitInfo; // structure containing hit point, normal, etc
    // hit = Hit(ray, hitInfo );
    hit = SearchBVHTree(ray, BB, hitInfo);
    if (hit){
        return ApplyLightingModel(ray, hitInfo, BB);
    }else{
        return GetBackground();
    }
};