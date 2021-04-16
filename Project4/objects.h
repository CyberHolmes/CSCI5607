#ifndef OBJECTS_H
#define OBJECTS_H

#include "glm/glm.hpp"
#include <stdio.h>

//base class
class Obj{
protected:
    int modelID; //model ID
    int texID; //texture ID
    glm::vec3 pos; //world position of the object
    glm::vec3 scale; //scale factors in 3 dimensions
    glm::vec3 rotAxis; //rotation
    float rotRad; //rotation angle in rad
    glm::vec3 color;
    bool show; //whether to render this object
public:
    Obj() : modelID(0), texID(-1), pos(glm::vec3(0,0,0)), scale(glm::vec3(1,1,1)), rotAxis(glm::vec3(0,1,0)), rotRad(0), 
        color(glm::vec3(1,1,1)), show(true) {}
    Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : modelID(modelID_), texID(texID_),
        pos(pos_), scale(scale_), rotAxis(glm::vec3(0,1,0)), rotRad(0.0), color(glm::vec3(1,1,1)), show(true) {}
    Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 color_) : modelID(modelID_), texID(texID_),
        pos(pos_), scale(scale_), rotAxis(glm::vec3(0,1,0)), rotRad(0.0), color(color_), show(true) {}
    Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) 
        : modelID(modelID_), texID(texID_), pos(pos_), scale(scale_), rotAxis(rotAxis_), rotRad(rotRad_), color(color_), show(true) {}

    ~Obj() {}
    //Access member vars
    int GetModelID() {return modelID;}
    int GetTexID() {return texID;}
    glm::vec3 GetPos() {return pos;}
    glm::vec3 GetScale() {return scale;}
    glm::vec3 GetRotAxis() {return rotAxis;}
    float GetRotRad() {return rotRad;}
    glm::vec3 GetColor() {return color;}
    bool IsVisible() {return show;}
    void SetVisibility(bool show_) {show = show_;}
    void SetPos(glm::vec3 p) {pos = p;}
    void SetRotRad(float rad) {rotRad = rad;}
    
    virtual bool Animate(float dt) = 0; //returns false if done animation
    // virtual glm::vec3 GetBoundMin()=0;
    // virtual glm::vec3 GetBoundMax()=0;
};

class Wall : public Obj {
public:
    Wall() : Obj() {}
    Wall(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) :
        Obj(modelID_, texID_, pos_, scale_) {}
    ~Wall() {}
    bool Animate(float dt) override; //Wall doesn't animate
};

class Entity : public Obj{
    float radius;
public:
    int keys[5]; //There are 5 possible keys 
    
    Entity() : Obj(), radius(0) {}
    Entity(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : 
        Obj(modelID_, texID_, pos_, scale_), radius(1), keys{0} {}
    //Entity(2,-1,curPos,glm::vec3(1,1,1),glm::vec3(0,1,0),3.14*3/2.0,glm::vec3(0.1,0.55,0.1),1);
    Entity(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_, float r_) : 
        Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_), radius(r_), keys{0} {}
    Entity(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, float r_) : 
        Obj(modelID_, texID_, pos_, scale_), radius(r_), keys{0} {}
    ~Entity() {}

    void AddKey(char k);
    bool HasKey(char k);
    float GetRadius() {return radius;}
    void SetColor(glm::vec3 c) {color = c;}
    bool Animate(float dt) override;
};

class Zombie : public Obj{
    float radius;
    glm::vec3 dir;
    float vel; //walking velocity
public:
    Zombie() : Obj(), radius(0), vel(0.5) {dir = glm::vec3(cos(rotRad),0,sin(rotRad));}
    Zombie(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : 
        Obj(modelID_, texID_, pos_, scale_), radius(1),
        vel(0.5) {dir = glm::vec3(sin(rotRad),0,cos(rotRad));}
    //Entity(2,-1,curPos,glm::vec3(1,1,1),glm::vec3(0,1,0),3.14*3/2.0,glm::vec3(0.1,0.55,0.1),1);
    Zombie(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_, float r_) : 
        Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_), radius(r_),
        vel(0.5) {dir = glm::vec3(sin(rotRad),0,cos(rotRad));}
    Zombie(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, float r_) : 
        Obj(modelID_, texID_, pos_, scale_), radius(r_), vel(0.5) {dir = glm::vec3(cos(rotRad),0,sin(rotRad));}
    ~Zombie() {}

    float GetRadius() {return radius;}
    void SetColor(glm::vec3 c) {color = c;}
    float GetVel() {return vel;}
    void SetVel(float v) {vel = v;}
    glm::vec3 GetDir() {return dir;}
    void UpdateDir() {dir = glm::vec3(cos(rotRad),0,sin(rotRad));}
    void SetDir(glm::vec3 d) {dir = d; rotRad = atan2(dir.z, dir.x)+3.14/2;}
    bool Animate(float dt) override;
};

class GenericObj : public Obj{
    float radius;
public:
    GenericObj() : Obj(), radius(0) {}
    //Goal(3,-1,curPos+glm::vec3(-1,-3.5,1),glm::vec3(1.5,1.5,1.5),glm::vec3(0,1,0), 0.0, glm::vec3(1,0,0));
    GenericObj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 color_) : 
        Obj(modelID_, texID_, pos_, scale_, color_), radius(1) {}
    GenericObj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_, float r_) : 
        Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_), radius(r_) {}
    ~GenericObj() {}
    bool WithinReach(Entity* e);
    bool Animate(float dt) override;
};

class Light : public Obj {
public:
    Light() : Obj() {}
    Light(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) :
        Obj(modelID_, texID_, pos_, scale_) {}
    ~Light() {}
    bool Animate(float dt) override; //Light doesn't animate
};

class Door : public Obj {
    bool endStateValid; //wheter below pars are valid
    glm::vec3 end_pos; //world position of the object
    glm::vec3 end_rotAxis; //rotation
    float end_rotRad; //rotation angle in rad
    glm:: vec3 d_pos;
    float d_rad;
public:
    Door() : Obj(), endStateValid(false), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0), d_pos(glm::vec3(0,0,0)), d_rad(0) {}
    Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 color_) :
        Obj(modelID_, texID_, pos_, scale_, color_), endStateValid(false), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0),
           d_pos(glm::vec3(0,0,0)), d_rad(0) {}
    Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_,
        glm::vec3 end_pos_, glm::vec3 end_rotAxis_, float end_rotRad_) :
        Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_),
        end_pos(end_pos_), end_rotAxis(end_rotAxis_), end_rotRad(end_rotRad_), endStateValid(true) 
        {d_pos = end_pos_ - pos_; d_rad = end_rotRad_ - rotRad_;}
    ~Door() {}

    void SetEndState(glm::vec3 e_pos, glm::vec3 e_axis, float e_rad);
    bool IsEndStateValid() {return endStateValid;}
    bool Animate(float dt) override;
};


// //define objects in the game
// class Obj{
// public:
//     int modelID; //model ID
//     int texID; //texture ID
//     glm::vec3 pos; //world position of the object
//     glm::vec3 scale; //scale factors in 3 dimensions
//     glm::vec3 rotAxis; //rotation
//     float rotRad; //rotation angle in rad
//     glm::vec3 color;
//     bool show; //whether to render this object
//     glm::vec3 end_pos; //world position of the object
//     glm::vec3 end_rotAxis; //rotation
//     float end_rotRad; //rotation angle in rad

//     Obj() : modelID(0), texID(-1), pos(glm::vec3(0,0,0)), scale(glm::vec3(1,1,1)), rotAxis(glm::vec3(0,1,0)), rotRad(0), 
//         color(glm::vec3(1,1,1)), show(true), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0)  {}
//     Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : modelID(modelID_), texID(texID_),
//         pos(pos_), scale(scale_), rotAxis(glm::vec3(0,1,0)), rotRad(0.0), color(glm::vec3(1,1,1)), show(true),
//         end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0)  {}

//     Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) 
//         : modelID(modelID_), texID(texID_), pos(pos_), scale(scale_), rotAxis(rotAxis_), rotRad(rotRad_), color(color_), show(true) {}

//     Obj(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_,
//         glm::vec3 end_pos_, glm::vec3 end_rotAxis_, float end_rotRad_) : 
//         modelID(modelID_), texID(texID_),
//         pos(pos_), scale(scale_), rotAxis(rotAxis_), rotRad(rotRad_), color(color_), show(true),
//         end_pos(end_pos_), end_rotAxis(end_rotAxis_), end_rotRad(end_rotRad_) {}
//     ~Obj(){}
// };

// class Entity : public Obj{
//     float size;
    
// public:
//     int keys[5]; //There are 5 possible keys 
//     Entity() : Obj(), size(0), keys{0} {}
//     Entity(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) : 
//     Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_), size(1), keys{0} {}
//     ~Entity() {}

//     void AddKey(char k);
//     bool HasKey(char k);
//     float GetSize() {return size;}
// };

// class Monster : public Obj{
//     float size;
    
// public:
//     bool isMobile;
//     Monster() : Obj(), size(0), isMobile(false) {}
//     Monster(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) : 
//     Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_), size(1), isMobile(false) {}
//     ~Monster() {}

//     bool seeEntity();
//     void update(dt);
//     void Chase();
// };

// class Door : public Obj{
// public:
//     glm::vec3 end_pos; //world position of the object
//     glm::vec3 end_rotAxis; //rotation
//     float end_rotRad; //rotation angle in rad

//     Door() : Obj(), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0) {}
//     Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_) : Obj(modelID_, texID_, pos_, scale_),
//         end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0)   {}
//     Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_) : 
//         Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_),
//         end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0) {}
//     Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 rotAxis_, float rotRad_, glm::vec3 color_,
//         glm::vec3 end_pos_, glm::vec3 end_rotAxis_, float end_rotRad_) : 
//         Obj(modelID_, texID_, pos_, scale_, rotAxis_, rotRad_, color_),
//         end_pos(end_pos_), end_rotAxis(end_rotAxis_), end_rotRad(end_rotRad_) {}
//     ~Door() {}
// };

#endif