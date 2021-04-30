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
        float ctheta = dot(ray.d,hi.hitNorm); //cos of angle between ray and hit normal
        //Reflection
        vec3 rdir = ray.d - 2 * ctheta *hi.hitNorm;
        Ray ray_reflect = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
        c_out = c_out + hi.m.sc * EvaluateRayTree(ray_reflect, BB);

        // c_reflect = c_reflect + hi.m.sc * EvaluateRayTree(ray_reflect, BB);

        //Indirect lighting
        // float rotMat[3][3] =
        //     {{0.0, -hi.hitNorm.z, hi.hitNorm.y}, 
        //     {hi.hitNorm.z, 0.0, -hi.hitNorm.x},
        //     {-hi.hitNorm.y, -hi.hitNorm.x, 0.0}
        // };
        /*glm::mat4 rotMat(1);
        glm::vec3 norm = glm::vec3(hi.hitNorm.x,hi.hitNorm.y,hi.hitNorm.z);
        int numPaths = 10;
        int numBounces = hi.rayDepth-1;
        
        for (int i=0; i<numPaths; i++){
            float th =rand()*M_PI;            
            rotMat = glm::rotate(rotMat,th,norm);
            // printf("rotM=%.2f,%.2f,%.2f\n",rotMat[0].x,rotMat[0].y,rotMat[0].z);
            glm::vec3 temp = glm::vec3(rotMat*glm::vec4(norm,1.0));

            vec3 new_dir = vec3(temp.x,temp.y,temp.z).normalized();
            Ray ray_indirect = Ray(hi.hitPos, new_dir, numBounces);
            c_reflect = c_reflect + hi.m.sc * EvaluateRayTree(ray_indirect, BB);
            // printf("th=%f,r=%.2f,%.2f,%.2f, c=%.2f,%.2f,%.2f\n",th,new_dir.x,new_dir.y,new_dir.z,c_ind.r,c_ind.g,c_ind.b);
        }*/
        // c_ind = Color(1.0,1.0,1.0);
        // printf("c=%.2f,%.2f,%.2f\n",c_ind.r,c_ind.g,c_ind.b);
        // c_ind = c_ind*(1.0/float(numPaths));
        // printf("c=%.2f,%.2f,%.2f\n",c_ind.r,c_ind.g,c_ind.b);
        // assert(false);
        //c_out = c_out + c_reflect*(1.0/(float(numPaths+1)));

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
    // texture mapping (if possible)
    c_out = c_out + ambientlight * hi.m.ac;
    if (hi.texture_map != nullptr) {
        c_out = c_out * hi.texture_map->Sample(hi.tex_coord.x, hi.tex_coord.y);
    }

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

//Sample uniform disk, then extrude for cosine weighted hemisphere
static vec3 sampleHemisphereCosine(double u1, double u2){
  const double r = sqrtf(u1);
  const double theta = 2 * M_PI * u2;
    const double x = r * cos(theta);
  const double z = r * sin(theta);
    return vec3(x, sqrt(fmax(0.0f, 1 - u1)), z);
};

//Change of basis from BTN space to world space
static vec3 sampleAroundNormalCosine(vec3 normal, vec3 bitangent, vec3 tangent){
  vec3 sample = sampleHemisphereCosine(rand01,rand01);
  return vec3( 
    dot(sample,vec3(bitangent.x,normal.x,tangent.x)), 
    dot(sample,vec3(bitangent.y,normal.y,tangent.y)), 
    dot(sample,vec3(bitangent.z,normal.z,tangent.z)));
};

Color Scene::TracePath(Ray& ray, BoundingBox* BB, int depth, int npaths){
    if (depth<=0){
        return Color(0,0,0);
    }
    HitInfo hi;
    if (!SearchBVHTree(ray,BB,hi)) {
        // // Check for light
        // Color c_out = Color();
        // bool hitLight = false;
        // for (int i=0; i<numLights; i++){
        //     Light* l = lights[i];
        //     if (l->type == DIRECTION_LIGHT){
        //         float ctheta = dot(ray.d.normalized(), l->CalDir(ray.d).normalized());
        //         if (ctheta > MIN_T){
        //             c_out = c_out + l->c * ctheta;
        //             hitLight = true;
        //         }
        //         // printf("got dir light\n"); assert(false);
        //     } else if (l->type == POINT_LIGHT) {
        //         vec3 ldir = l->p - ray.p;
        //         float ctheta = dot(ray.d.normalized(), ldir.normalized());
        //         if (ctheta>MIN_T){
        //             c_out = c_out + l->c * ctheta *(1.0/ldir.lengthsq());
        //             hitLight = true;
        //             // printf("got dir light\n"); assert(false);
        //         }
        //     }
        // }
        // if (hitLight){
        //     return c_out;
        // }
        return background;//Color(0,0,0);
    }

    Material m = hi.m;
    Color em = m.ec;
    // if (m.ec.mag()>0.0) {
    //     return em;
    // }
    Color c_sum = Color(0,0,0);
    
    int cnt = 0;
    int new_npaths = npaths*0.4+1;
    // bool diffusive = false;//(m.dc.mag()>0.0);
    // bool reflective = false;//(m.sc.mag()>0.0);
    // bool refractive = false;//(m.tc.mag()>0.0);
    // switch (rand()%3){
    //     case 0: 
    //         diffusive = true;
    //         break;
    //     case 1:
    //         reflective = true;
    //         break;
    //     case 2:
    //         refractive = true;
    //         break;
    // }
    bool diffusive = (m.dc.mag()>0.0);
    bool reflective = (m.sc.mag()>0.0);
    bool refractive = (m.tc.mag()>0.0);
    Color c_indirect = Color(0,0,0);
    Color c_reflective = Color(0,0,0);
    Color c_refractive = Color(0,0,0);
    
    if (diffusive)
    {
        vec3 new_dir = hi.hitNorm;
        vec3 t1 = cross(hi.hitNorm,ray.d).normalized();//(-hi.hitNorm.z, 0.0, hi.hitNorm.x);
        vec3 t2 = cross(hi.hitNorm,t1).normalized();
        int i = 0;
        do {            
            new_dir = sampleAroundNormalCosine(hi.hitNorm,t1,t2);  //CosineSampleHemisphere(hi.hitNorm,t1,t2);         
            Ray ray_indirect = Ray(hi.hitPos, new_dir, depth-1);           
            float ctheta = dot(new_dir,hi.hitNorm);            
            c_indirect = c_indirect + m.dc * TracePath(ray_indirect, BB, depth-1, new_npaths);// *ctheta * ctheta;
            i++;
        } while(i<npaths);
        c_indirect = c_indirect * (1.0/float(i));// * (2*M_PI);
        if (hi.texture_map != nullptr) {
            c_indirect = c_indirect * hi.texture_map->Sample(hi.tex_coord.x, hi.tex_coord.y);
        }
    }    
    if (reflective)
    { //reflective
        float ctheta = dot(ray.d,hi.hitNorm); //cos of angle between ray and hit normal
        vec3 rdir = ray.d - 2 * ctheta *hi.hitNorm;
        Ray ray_reflect = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
        Color incoming = TracePath(ray_reflect, BB, depth-1, new_npaths);
        c_reflective =  m.sc * incoming;// * fade_f;
        cnt++;
    }
    // refraction
    if (refractive)
    {
        float ctheta = dot(ray.d,hi.hitNorm); 
        float eta = (ctheta<0)?1.0/hi.m.ior:hi.m.ior;
        vec3 n = (ctheta<0)? hi.hitNorm : -hi.hitNorm;
        ctheta = (ctheta>0)? -ctheta : ctheta;
        float eta_sq = eta * eta;
        float rf_term = 1 - eta_sq + ctheta * ctheta * eta_sq;
        if (rf_term > 0) { //there is refration
            vec3 rdir = (-sqrt(rf_term) - ctheta * eta) * n + eta * ray.d;
            Ray ray_refract = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
            c_refractive = hi.m.tc * TracePath(ray_refract, BB,depth-1, new_npaths);
            cnt++;
        }
    }
    return em + (c_reflective + c_indirect*2 + c_refractive)*(1.0/float(cnt+1));// + m.ac * ambientlight;    
}