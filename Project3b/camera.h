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
    // float theta; // rotation around Y axis. Starts with forward direction as ( 0, 0, -1 )
    // float phi; // rotation around X axis. Starts with up direction as ( 0, 1, 0 )

    Camera() : eye(0,0,0), forward(0,0,-1),up(0,1,0),right(-1,0,0),fov_h(45){}
    void Update(float dt){   
        float moveSpeed=1;
        float turnSpeed=0.5;     
        // theta = turnSpeed * (negativeTurn.x + positiveTurn.x)*dt;
        // cap the rotation about the X axis to be less than 90 degrees to avoid gimble lock
        // float maxAngleInRadians = 85 * PI / 180;
        // phi = fmin( maxAngleInRadians, fmax( -maxAngleInRadians, phi + turnSpeed * ( negativeTurn.y+ positiveTurn.y ) * dt ) );
        
        // // re-orienting the angles to match the wikipedia formulas: https://en.wikipedia.org/wiki/Spherical_coordinate_system
        // // except that their theta and phi are named opposite
        // float t = theta;// + PI / 2;
        // float p = phi;// + PI / 2;
        // forward = forward + vec3( sin( p ) * cos( t ),   cos( p ),   -sin( p ) * sin ( t ) );
        // up = up +vec3( sin( phi ) * cos( t ), cos( phi ), -sin( t ) * sin( phi ) );        
        // right = right + vec3( -cos( theta ), 0, sin( theta ) );
        // forward = forward + vec3(forward.x*sin( p ) * cos( t ), forward.y*cos(p), -forward.z*sin( p ) * sin ( t ));

        vec3 turnvel = vec3(positiveTurn.x+negativeTurn.x,positiveTurn.y+negativeTurn.y,positiveTurn.z+negativeTurn.z);
        forward = forward +turnSpeed*dt*turnvel;

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
inline void key_j_pressed(){
    negativeTurn.x = -1;
}
inline void key_l_pressed(){
    positiveTurn.x = 1;
}
inline void key_u_pressed(){
    negativeTurn.z = -1;
}
inline void key_o_pressed(){
    positiveTurn.z = 1;
}
inline void key_k_pressed(){
    negativeTurn.y = -1;
}
inline void key_i_pressed(){
    positiveTurn.y = 1;
}

// inline void key_left_pressed(){
//     negativeTurn.x = 1;
// }
// inline void key_right_pressed(){
//     negativeTurn.x = -1;
// }
// inline void key_up_pressed(){
//     positiveTurn.y = 1;
// }
// inline void key_down_pressed(){
//     positiveTurn.y = -1;
// }

#endif