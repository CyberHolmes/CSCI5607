#ifndef SCENE_H
#define SCENE_H

#include "objects.h"
#include <vector>

class Scene{
    std::vector<Sphere> spheres;
    std::vector<PointLight> pointlights;
    std::vector<DirectionalLight> dirlights;
    std::vector<SpotLight> spotlights;
    Color ambientlight;
    Color background;
    int numSpheres;
    int numPointlights;
    int numDirlights;
    int numSpotlights;
public:
    //constructor
    Scene();
    //destructor
    ~Scene() {};

    //Add object
    void AddSphere(Sphere);
    void AddSpotlight(SpotLight);
    void AddPointlight(PointLight);
    void AddDirlight(DirectionalLight);
    void SetBackColor(Color);
    void SetAmbientLight(Color);

    //Get
    // inline Sphere& GetSphere(int);
    Sphere GetSphere(int);
    PointLight GetPointlight(int);
    SpotLight GetSpotLight(int);
    DirectionalLight GetDirLight(int);
    int GetNumSpheres();
    int GetNumPointLight();
    int GetNumSpotLight();
    int GetNumDirLight();
    Color GetAmbientLight();
    Color GetBackground();

    //Given a point and a direction, check for if any object is in the direction
    //Return bool
    bool Hit(Ray ray);
    //Given a point and a direction, check for the closest hit, return information
    bool HitWInfo(Ray ray, HitInfo&);
};

#endif