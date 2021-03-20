#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#define PI 3.14159265

// probably don't need / want to change any of the below variables
extern vec3 negativeMovement;
extern vec3 positiveMovement;
extern vec3 negativeTurn;
extern vec3 positiveTurn;

struct Camera
{
   vec3 eye; 
    vec3 forward;
    vec3 up;
    vec3 right;
    float fov_h;

    Camera() : eye(0,0,0), forward(0,0,-1),up(0,1,0),right(-1,0,0),fov_h(45){}
    Camera(Camera& c) : eye(c.eye), forward(c.forward), up(c.up), right(c.right),fov_h(c.fov_h){}
    void operator= (Camera&c){eye = c.eye; forward = c.forward; up = c.up; right = c.right, fov_h = c.fov_h;}
    void Update(float dt){   
        float moveSpeed=5;//2.5;
        float turnSpeed=1;//0.5;     
        float theta = turnSpeed * (negativeTurn.x + positiveTurn.x)*dt; // rotation around Y axis. Starts with forward direction as ( 0, 0, -1 )
        // cap the rotation about the X axis to be less than 90 degrees to avoid gimble lock
        float maxAngleInRadians = 85 * PI / 180;
        // rotation around X axis. Starts with up direction as ( 0, 1, 0 )
        float phi = fmin( maxAngleInRadians, fmax( -maxAngleInRadians, turnSpeed * ( negativeTurn.y+ positiveTurn.y ) * dt ) );
        vec3 cforward = forward; //current forward postion
        forward.x = cos(theta)*cforward.x - sin(theta)*sin(phi)*cforward.y -sin(theta)*cos(phi)*cforward.z;
        forward.y = cos(phi)*cforward.y - sin(phi)*cforward.z;
        forward.z = sin(theta)*cforward.x + cos(theta)*sin(phi)*cforward.y + cos(theta)*cos(phi)*cforward.z;
        
        forward = forward.normalized();
        up = (up - dot(up,forward)*forward).normalized();
        right = cross(up,forward);

        vec3 velocity   = vec3( negativeMovement.x + positiveMovement.x, negativeMovement.y + positiveMovement.y, negativeMovement.z + positiveMovement.z );
        eye = eye + moveSpeed * velocity.z * dt  * forward;
        eye = eye + moveSpeed * velocity.y * dt * up;
        eye = eye + moveSpeed * velocity.x * dt * right;
        positiveMovement = vec3(0,0,0);
        negativeMovement = vec3(0,0,0);
        positiveTurn = vec3(0,0,0);
        negativeTurn = vec3(0,0,0);
    }
    void PrintState(){
        printf("eye=%f,%f,%f\n",eye.x,eye.y,eye.z);
        printf("forward=%f,%f,%f\n",forward.x,forward.y,forward.z);
        printf("up=%f,%f,%f\n",up.x,up.y,up.z);
        printf("right=%f,%f,%f\n",right.x,right.y,right.z);
    }
};

inline void key_w_pressed(){
    positiveMovement.z = 1;
}
inline void key_s_pressed(){
    negativeMovement.z = -1;
}
inline void key_a_pressed(){
    negativeMovement.x = -1;
}
inline void key_d_pressed(){
    positiveMovement.x = 1;
}
inline void key_q_pressed(){
    positiveMovement.y = 1;
}
inline void key_e_pressed(){
    negativeMovement.y = -1;
}
inline void key_left_pressed(){
    negativeTurn.x = 1;
}
inline void key_right_pressed(){
    negativeTurn.x = -1;
}
inline void key_up_pressed(){
    positiveTurn.y = 1;
}
inline void key_down_pressed(){
    positiveTurn.y = -1;
}

#endif