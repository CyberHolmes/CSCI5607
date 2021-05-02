#include "scene.h"

void Scene::AddObject(Obj* o){
    objects.emplace_back(o);
    numObj++;
}
void Scene::AddObject2(Obj* o){
    objects2.emplace_back(o);
    numObj2++;
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
    float diffusive_l = hi.m.dc.luminance();
    float reflective_l = hi.m.sc.luminance();
    float refractive_l = hi.m.tc.luminance();
    float l_sum = diffusive_l + reflective_l + refractive_l;
    float diffusive_a = diffusive_l/l_sum;
    float reflective_a = refractive_l/l_sum;
    float refractive_a = refractive_l/l_sum;
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
        c_out = c_out + l->Shading(hi)*diffusive_a;        
    }
    if (hi.rayDepth>0){
        float ctheta = dot(ray.d,hi.hitNorm); //cos of angle between ray and hit normal
        //Reflection
        vec3 rdir = ray.d - 2 * ctheta *hi.hitNorm;
        Ray ray_reflect = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
        c_out = c_out + hi.m.sc * EvaluateRayTree(ray_reflect, BB)*reflective_a;

        //Refraction
        float eta = (ctheta<0)?1.0/hi.m.ior:hi.m.ior;
        vec3 n = (ctheta<0)? hi.hitNorm : -hi.hitNorm;
        ctheta = (ctheta>0)? -ctheta : ctheta;
        float eta_sq = eta * eta;
        float rf_term = 1 - eta_sq + ctheta * ctheta * eta_sq;
        if (rf_term > 0) { //there is refration
            rdir = (-sqrt(rf_term) - ctheta * eta) * n + eta * ray.d;
            Ray ray_refract = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
            c_out = c_out + hi.m.tc * EvaluateRayTree(ray_refract, BB)*refractive_a;
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

Color Scene::TracePath(Ray& ray, BoundingBox* BB, int depth, bool uselight){
    if (depth<=0){
        return Color(0,0,0);
    }
    HitInfo hi;
    if (!SearchBVHTree(ray,BB,hi)) {
        if (uselight){
            if (depth == max_depth) return background;
            Color c_out = Color();
            bool hitLight = false;
            for (int i=0; i<numLights; i++){
                Light* l = lights[i];
                if (l->type == DIRECTION_LIGHT){
                    float ctheta = dot(ray.d.normalized(), l->CalDir(ray.d).normalized());
                    if (ctheta > MIN_T){
                        c_out = c_out + l->c * ctheta *2.0;
                        hitLight = true;
                    }
                // } else if (l->type == POINT_LIGHT) {
                //     vec3 ldir = l->p - ray.p;
                //     float ctheta = dot(ray.d.normalized(), ldir.normalized());
                //     if (ctheta>0.1){  
                //         c_out = c_out + l->c * (ctheta *(1.0/ldir.lengthsq())*3.0);
                //         hitLight = true;
                //     }
                } else if (l->type == POINT_LIGHT) {
                    vec3 ldir = l->p - ray.p;
                    float ctheta = dot(ray.d.normalized(), ldir.normalized());
                    // float a1 = 0.99, a2 = 0.9;
                    // if (ctheta>a2){  
                    //     float term = (ctheta<a1)?(1.0-(a1-ctheta)/(a1-a2)) : 1.0;       
                    //     c_out = c_out + l->c * (ctheta *(1.0/ldir.length())*20.0*term);
                    if (ctheta>MIN_T){ 
                        c_out = c_out + l->c * (ctheta *(1.0/ldir.length()));
                        hitLight = true;
                    }
                }
            }
            if (hitLight){
                return c_out;
            }
        } //end if uselight
        return background;//Color(0,0,0);
    }

    if (!uselight && hi.m.ec.mag()>0.0) {
        return hi.m.ec;
    }
   
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
    bool diffusive = (hi.m.dc.mag()>0.0);
    bool reflective = (hi.m.sc.mag()>0.0);
    bool refractive = (hi.m.tc.mag()>0.0);
    Color c_indirect = Color(0,0,0);
    Color c_reflective = Color(0,0,0);
    Color c_refractive = Color(0,0,0);

    float diffusive_l = hi.m.dc.luminance();
    float reflective_l = hi.m.sc.luminance();
    float refractive_l = hi.m.tc.luminance();
    float l_sum = diffusive_l + reflective_l + refractive_l;
    float diffusive_a = diffusive_l/l_sum;
    float reflective_a = refractive_l/l_sum;
    float refractive_a = refractive_l/l_sum;
    int cnt=0;
    
    if (diffusive)
    {
        vec3 new_dir = hi.hitNorm;
        vec3 t1 = cross(hi.hitNorm,ray.d).normalized();//(-hi.hitNorm.z, 0.0, hi.hitNorm.x);
        vec3 t2 = cross(hi.hitNorm,t1).normalized();
        new_dir = sampleAroundNormalCosine(hi.hitNorm,t1,t2);  //CosineSampleHemisphere(hi.hitNorm,t1,t2);         
        Ray ray_indirect = Ray(hi.hitPos, new_dir, depth-1);           
        float ctheta = dot(new_dir,hi.hitNorm);            
        c_indirect = hi.m.dc * TracePath(ray_indirect, BB, depth-1, uselight) * diffusive_a;// *ctheta * ctheta;
        cnt++;
    }    
    if (reflective)
    { //reflective
        float ctheta = dot(ray.d,hi.hitNorm); //cos of angle between ray and hit normal
        vec3 rdir = ray.d - 2 * ctheta *hi.hitNorm;
        Ray ray_reflect = Ray(hi.hitPos, rdir.normalized(),hi.rayDepth-1);
        Color incoming = TracePath(ray_reflect, BB, depth-1, uselight);        
        c_reflective =  hi.m.sc * incoming * reflective_a;// * fade_f;
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
            c_refractive = hi.m.tc * TracePath(ray_refract, BB,depth-1, uselight) * refractive_a;
            cnt++;
        }
    }
    return (c_reflective + c_indirect + c_refractive);// + m.ac * ambientlight;    
}