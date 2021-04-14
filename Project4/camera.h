#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include "glm/glm.hpp"

float posFwd=0, negFwd=0, posRight=0, negRight=0;
float hPosTurn=0, hNegTurn=0, vPosTurn=0, vNegTurn=0;
float upforce = 0, fwdspeed = 0;
glm::vec3 agentVel = glm::vec3(0,0,0);

class Camera{
public:
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;
    float hAngle, vAngle;
    float FoV;
    float turnSpeed, moveSpeed;
    float y_pos;
    const float g = -5; //gravity
    
    Camera() : pos(3,0,0), dir(0,0,0),up(0,1,0),right(0,0,-1),hAngle(3.14*1.5),vAngle(0),FoV(3.14/2.2),turnSpeed(0.8),moveSpeed(5),y_pos(0){}
    Camera(Camera& c) : pos(c.pos), dir(c.dir), up(c.up), right(c.right),hAngle(c.hAngle),vAngle(c.vAngle),FoV(c.FoV),
        turnSpeed(c.turnSpeed),moveSpeed(c.moveSpeed),y_pos(c.y_pos) {}
    void operator= (Camera&c){pos = c.pos; dir = c.dir; up = c.up; right = c.right, hAngle = c.hAngle; vAngle = c.vAngle;
        FoV = c.FoV; turnSpeed = c.turnSpeed; moveSpeed = c.moveSpeed; y_pos = c.y_pos;}
    
    void Update(float dt){
        float upspeed = (upforce+g)*30*dt;
        glm::vec3 fwd = glm::vec3(dir.x, 0 , dir.z);
        
        pos += (((posRight + negRight) * right + (posFwd + negFwd + fwdspeed) * fwd)*moveSpeed + glm::vec3(0,upspeed,0))*dt;
        // y_pos += upspeed*dt;
        // printf("upforce=%f, upspeed=%f, y_pos=%f\n",upforce, upspeed,y_pos);
        // // y_pos = (y_pos>0)?y_pos:0;
        // glm::vec3 fwd = glm::vec3(dir.x, 0 , dir.z);
        // pos += ((posRight + negRight) * right + (posFwd + negFwd) * fwd) * dt * moveSpeed; // + glm::vec3(0,y_pos,0);
        if (pos.y<0){
            pos.y = 0;
            fwdspeed = 0;
        }
        hAngle += (hPosTurn + hNegTurn) * dt * turnSpeed;
        vAngle += (vPosTurn + vNegTurn) * dt * turnSpeed;
        dir = glm::vec3 (
            cos(vAngle) * sin(hAngle),
            sin(vAngle),
            cos(vAngle) * cos(hAngle)
        );
        right = glm::vec3(
            sin(hAngle - 3.14f/2.0f),
            0,
            cos(hAngle - 3.14f/2.0f)
        );
        up = glm::cross( right, dir );
        posFwd=0; negFwd=0; posRight=0; negRight=0;
        hPosTurn=0; hNegTurn=0; vPosTurn=0; vNegTurn=0;
        upforce = 0;
        agentVel = glm::vec3(0,0,0);
    }

    void PrintState(){
        printf("pos=%f,%f,%f\n",pos.x,pos.y,pos.z);
        printf("dir=%f,%f,%f\n",dir.x,dir.y,dir.z);
        printf("up=%f,%f,%f\n",up.x,up.y,up.z);
        printf("right=%f,%f,%f\n",right.x,right.y,right.z);
    }
};

#endif