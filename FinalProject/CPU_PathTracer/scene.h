#ifndef SCENE_H
#define SCENE_H

#include "objects.h"
#include <vector>
#include <memory>
#include "bvh.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Scene{

    Color ambientlight;
    Color background;
    int numObj;
    int numLights;
    std::vector<Obj*> objects;
    std::vector<Light*> lights;

public:
    Scene() : ambientlight(Color()),background(Color()),numObj(0),numLights(0){
        objects.get_allocator().allocate(1000);
        lights.get_allocator().allocate(20);
    };
    ~Scene() {
        for ( auto const& obj : objects) {
            delete obj;
        }
        for ( auto const& l : lights) {
            delete l;
        }
    };
    void AddObject(Obj*);
    // Obj* GetObject(int);
    void AddLight(Light*);
    // Light* GetLight(int);
    int GetNumObjects();
    int GetNumLights();
    Color GetAmbientLight();
    Color GetBackground();
    void SetBackground(Color);
    void SetAmbientlight(Color);    
    std::vector<Obj*> GetObjects() {return objects;}
    //Given a point and a direction, check for the closest hit, return information
    // bool Hit(const Ray& ray, HitInfo&);

    Color ApplyLightingModel (Ray& ray, HitInfo& hi, BoundingBox* BB);
    Color EvaluateRayTree(Ray& ray, BoundingBox* BB);
};

#endif