#ifndef SCENE_H
#define SCENE_H

#include "objects.h"
#include <vector>
#include <memory>
#include "bvh.h"

// #define GLM_FORCE_RADIANS
// #include "glm/glm.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"

class Scene{

    Color ambientlight;
    Color background;
    int numObj;
    int numObj2;
    int numLights;
    std::vector<Obj*> objects;
    std::vector<Obj*> objects2; //for path tracing, include lighting objects
    std::vector<Light*> lights;
    int max_depth;

public:
    Scene() : ambientlight(Color()),background(Color()),numObj(0),numObj2(0),numLights(0), max_depth(0){
        objects.get_allocator().allocate(1000);
        objects2.get_allocator().allocate(1000);
        lights.get_allocator().allocate(20);
    };
    ~Scene() {
        for ( auto const& obj : objects) {
            delete obj;
        }
        for ( auto const& obj : objects2) {
            delete obj;
        }
        for ( auto const& l : lights) {
            delete l;
        }
    };
    void AddObject(Obj*);
    void AddObject2(Obj*);
    // Obj* GetObject(int);
    void AddLight(Light*);
    // Light* GetLight(int);
    int GetNumObjects();
    int GetNumLights();
    Color GetAmbientLight();
    Color GetBackground();
    void SetBackground(Color);
    void SetAmbientlight(Color);  
    void SetMaxDepth(int d) {max_depth = d;}  
    std::vector<Obj*> GetObjects() {return objects;}
    std::vector<Obj*> GetObjects2() {return objects2;}
    Color ApplyLightingModel (Ray& ray, HitInfo& hi, BoundingBox* BB);
    Color EvaluateRayTree(Ray& ray, BoundingBox* BB);
    Color TracePath(Ray& ray, BoundingBox* BB, int depth, bool uselight);
};

#endif