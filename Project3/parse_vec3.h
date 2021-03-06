
//Set the global scene parameter variables
//TODO: Set the scene parameters based on the values in the scene file

#ifndef PARSE_VEC3_H
#define PARSE_VEC3_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>
// #include "objects.h"
#include "scene.h"
#include "camera.h"

#define MAX_LEN 1024
#define PI 3.14159265

//Camera & Scene Parmaters (Global Variables)
//Here we set default values, override them in parseSceneFile()

//Image Parmaters
extern int img_width, img_height;
std::string imgName = "raytraced.bmp";

int max_depth = 5;

void parseSceneFile(std::string fileName, Scene* scene, Camera* camera, int& sample_size){
  FILE *fp;
  long length;
  char line[MAX_LEN]; //assume no line is longer than 1024 characters

  //open file for read
  fp = fopen(fileName.c_str(),"r");

  //check whether file is successfully opened
  if (!fp){
    fprintf(stderr,"Can't open files '%s'\n",fileName.c_str());
    exit(1);
  }

  Material m = Material();
  //read each line
  while (fgets(line, MAX_LEN, fp)){
    if (line[0]=='#') continue; //skip comment

    char command[100]; //container for command    
    
    if (sscanf(line,"%s ",command)<1) continue;
    if (!strcmp(command, "material:")){ //material: .75 .75 .75 .75 .75 .75 .3 .3 .3 32 .2 .2 .2 1.5
      float ar,ag,ab;
      float dr,dg,db;
      float sr,sg,sb;
      int ns;
      float tr,tg,tb,ior;
      sscanf(line, "material: %f %f %f %f %f %f %f %f %f %d %f %f %f %f",
        &ar,&ag,&ab,&dr,&dg,&db,&sr,&sg,&sb,&ns,&tr,&tg,&tb,&ior);
      m.SetAmbientColor(Color(ar,ag,ab));
      m.SetDiffuseColor(Color(dr,dg,db));
      m.SetSpeculorColor(Color(sr,sg,sb));
      m.SetTransmissiveColor(Color(tr,tg,tb));
      m.SetPhongFactor(ns);
      m.SetIOR(ior);
    }
    if (!strcmp(command, "sphere:")){ //sphere: -3 1 0 0.7
      float x,y,z,r;
      sscanf(line, "sphere: %f %f %f %f",&x,&y,&z,&r);
      vec3 v=vec3(x,y,z);
      Sphere* s = new Sphere(m,v,r);
      scene->AddObject(s);  
    }
    if (!strcmp(command, "background:")){ //sphere: -3 1 0 0.7
      float r,g,b;
      sscanf(line, "background: %f %f %f",&r,&g,&b);
      scene->SetBackground(Color(r,g,b));      
    }
    if (!strcmp(command, "ambient_light:")){ //sphere: -3 1 0 0.7
      float r,g,b;
      sscanf(line, "ambient_light: %f %f %f",&r,&g,&b);
      scene->SetAmbientlight(Color(r,g,b));    
    }
    if (!strcmp(command, "directional_light:")){ //sphere: -3 1 0 0.7
      float r,g,b,x,y,z;
      sscanf(line, "directional_light: %f %f %f %f %f %f",&r,&g,&b,&x,&y,&z);
      scene->AddLight(new DirectionLight(Color(r,g,b),vec3(x,y,z)));  
    }
    if (!strcmp(command, "point_light:")){ //sphere: -3 1 0 0.7
      float r,g,b,x,y,z;
      sscanf(line, "point_light: %f %f %f %f %f %f",&r,&g,&b,&x,&y,&z);
      scene->AddLight(new PointLight(Color(r,g,b),vec3(x,y,z)));   
    }
    if (!strcmp(command, "spot_light:")){ //sphere: -3 1 0 0.7
      float r,g,b,px,py,pz,dx,dy,dz,a1,a2;
      sscanf(line, "spot_light: %f %f %f %f %f %f %f %f %f %f %f",&r,&g,&b,&px,&py,&pz,&dx,&dy,&dz,&a1,&a2);
      a1 = cos(a1*PI/180.0); //convert once at the file load, so that this can be directly compared to dot product
      a2 = cos(a2*PI/180.0);
      scene->AddLight(new SpotLight(Color(r,g,b),vec3(px,py,pz),vec3(dx,dy,dz),a1,a2));
    }
    if (!strcmp(command, "film_resolution:")){ //image_resolution: 500 300
      sscanf(line, "film_resolution: %d %d",&img_width,&img_height);
    }
    if (!strcmp(command, "output_image:")){ //output_image: f2-down-and-left.jpg
      char temp[1024];
      sscanf(line, "output_image: %s",temp);
      imgName = temp;
    }
    if (!strcmp(command, "camera_pos:")){ //camera_pos: -6 2 -4
      float x,y,z;
      sscanf(line, "camera_pos: %f %f %f",&x,&y,&z);
      camera->eye = vec3(x,y,z);
    }
    if (!strcmp(command, "camera_fwd:")){ //camera_fwd: -0.77 0 -0.66
      float x,y,z;
      sscanf(line, "camera_fwd: %f %f %f",&x,&y,&z);
      camera->forward = vec3(x,y,z).normalized();
    }
    if (!strcmp(command, "camera_up:")){ //camera_up:  0 1 0
      float x,y,z;
      sscanf(line, "camera_up: %f %f %f",&x,&y,&z);
      camera->up = vec3(x,y,z).normalized();
    }
    if (!strcmp(command, "camera_fov_ha:")){ //camera_fov_ha: 35
      float x;
      sscanf(line, "camera_fov_ha: %f",&x);
      camera->fov_h = x;
    }
    if (!strcmp(command, "max_depth:")){ //camera_fov_ha: 35
      sscanf(line, "max_depth: %d",&max_depth);
    }
    if (!strcmp(command, "sample_size:")){ //camera_fov_ha: 35
      sscanf(line, "sample_size: %d",&sample_size);
    }
  }
  //Create an orthagonal camera basis
  camera->forward = camera->forward.normalized();
  camera->up = (camera->up - dot(camera->up,camera->forward)*camera->forward).normalized();
  camera->right = cross(camera->up,camera->forward);
  // float temp = camera->forward.length();
  // camera->theta = (fabs(temp)<MIN_T)? 0:acos(camera->forward.y/temp)-PI/2;
  // camera->phi = (fabs(camera->forward.x)<MIN_T)? 0: atan2(-camera->forward.z,camera->forward.x)-PI/2;
  // camera->theta = (fabs(camera->forward.x)<MIN_T)? 0:atan2(-camera->forward.x,-camera->forward.z);
  // camera->phi = (fabs(camera->forward.y)<MIN_T)? 0: atan2(camera->forward.y,-camera->forward.z);
  // printf("theta=%f, phi=%f\n",camera->theta,camera->phi);
  // printf("camera->eye=%f,%f,%f\n",camera->eye.x,camera->eye.y,camera->eye.z);
  // printf("camera->forward=%f,%f,%f\n",camera->forward.x,camera->forward.y,camera->forward.z);
  // printf("camera->up=%f,%f,%f\n",camera->up.x,camera->up.y,camera->up.z);
  // printf("camera->right=%f,%f,%f\n",camera->right.x,camera->right.y,camera->right.z);
}

#endif