#ifndef OBJECTS_H
#define OBJECTS_H

#include "glm/glm.hpp"
#include <stdio.h>
//define objects in the game
class Obj{
public:
    int modelID; //model ID
    int texID; //texture ID
    glm::vec3 pos; //world position of the object
    glm::vec3 scale; //scale factors in 3 dimensions
    glm::vec3 rotAxis; //rotation
    float rotRad; //rotation angle in rad
    glm::vec3 color;
    bool show; //whether to render this object
    glm::vec3 end_pos; //world position of the object
    glm::vec3 end_rotAxis; //rotation
    float end_rotRad; //rotation angle in rad

    Obj() : modelID(0), texID(-1), pos(glm::vec3(0,0,0)), scale(glm::vec3(1,1,1)), rotAxis(glm::vec3(0,1,0)), rotRad(0), 
        color(glm::vec3(1,1,1)), show(true), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0)  {}
    Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : modelID(modelID_), texID(texID_),
        pos(pos_), scale(scale_), rotAxis(glm::vec3(0,1,0)), rotRad(0.0), color(glm::vec3(1,1,1)), show(true),
        end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0)  {}

    Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) 
        : modelID(modelID_), texID(texID_), pos(pos_), scale(scale_), rotAxis(rotAxis_), rotRad(rotRad_), color(color_), show(true) {}

    Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_,
        glm::vec3 end_pos_, glm::vec3 end_rotAxis_, float end_rotRad_) : 
        modelID(modelID_), texID(texID_),
        pos(pos_), scale(scale_), rotAxis(rotAxis_), rotRad(rotRad_), color(color_), show(true),
        end_pos(end_pos_), end_rotAxis(end_rotAxis_), end_rotRad(end_rotRad_) {}
    ~Obj(){}

};

class Entity : public Obj{
    float size;
    
public:
    int keys[5]; //There are 5 possible keys 
    Entity() : Obj(), size(0), keys{0} {}
    Entity(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) : 
    Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_), size(1), keys{0} {}
    ~Entity() {}

    void AddKey(char k);
    bool HasKey(char k);
    float GetSize() {return size;}
};

class Door : public Obj{
public:
    glm::vec3 end_pos; //world position of the object
    glm::vec3 end_rotAxis; //rotation
    float end_rotRad; //rotation angle in rad

    Door() : Obj(), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0) {}
    Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : Obj(modelID_, texID_, pos_, scale_),
        end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0)   {}
    Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) : 
        Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_),
        end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0) {}
    Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_,
        glm::vec3 end_pos_, glm::vec3 end_rotAxis_, float end_rotRad_) : 
        Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_),
        end_pos(end_pos_), end_rotAxis(end_rotAxis_), end_rotRad(end_rotRad_) {}
    ~Door() {}
};

#endif