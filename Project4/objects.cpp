#include "objects.h"

void Entity::AddKey(char k){
    keys[k-97] = 1;
};
bool Entity::HasKey(char k){
    return keys[k-97];
};

bool Wall::Animate(float dt) {return false;}
bool Light::Animate(float dt) {return false;}
bool GenericObj::Animate(float dt) {return false;}
bool Entity::Animate(float dt) {return false;}

void Door::SetEndState(glm::vec3 e_pos, glm::vec3 e_axis, float e_rad){
    end_pos = e_pos;
    end_rotAxis = e_axis;
    end_rotRad = e_rad;
    d_pos = end_pos - pos;
    d_rad = end_rotRad - rotRad;
};

bool Door::Animate(float dt) {
    if ((fabs(rotRad - end_rotRad)<0.001) || (glm::distance(pos, end_pos)<0.01)){
        rotRad = end_rotRad;
        pos = end_pos;
        return false;
    } else {
        // printf("rotRad=%f,pos=%f,%f,%f\n",rotRad,pos.x,pos.y,pos.z);
        // printf("end_rotRad=%f,end_pos=%f,%f,%f\n",end_rotRad,end_pos.x,end_pos.y,end_pos.z);
        rotRad += d_rad*dt*1;                
        pos = pos + d_pos * (dt*1);
        // printf("d_rad = %f, d_pos = %f,%f,%f\n",d_rad,d_pos.x,d_pos.y,d_pos.z);
        
        return true;                  
    } 
};

bool GenericObj::WithinReach(Entity* e){
    float d = glm::distance(e->GetPos(),pos);
    return (d*d<((e->GetRadius() + radius)*(e->GetRadius() + radius) + pos.y *pos.y));
};

bool Zombie::Animate(float dt){
    //taking random step
    //if walk into obstacle, change direction
    return false;
}