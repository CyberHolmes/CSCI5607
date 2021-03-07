
//Set the global scene parameter variables
//TODO: Set the scene parameters based on the values in the scene file

#ifndef PARSE_VEC3_H
#define PARSE_VEC3_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
// #include "objects.h"
#include "scene.h"
#include "camera.h"

#define MAX_LEN 1024
#define PI 3.14159265

//Camera & Scene Parmaters (Global Variables)
//Here we set default values, override them in parseSceneFile()

//Image Parmaters
int img_width = 800, img_height = 600;
std::string imgName = "raytraced.png";

//Camera Parmaters
// vec3 eye = vec3(0,0,0); 
// vec3 forward = vec3(0,0,-1).normalized();
// vec3 up = vec3(0,1,0).normalized();
// vec3 right = vec3(-1,0,0).normalized();
// float halfAngleVFOV = 35; 
// Camera camera;


int max_depth = 5;

void parseSceneFile(std::string fileName, Scene& scene, Camera& camera, int& sample_size){
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

  //get file size
  // fseek(fp,0,SEEK_END); //move position to the end
  // length =ftell(fp); //value of current position
  // printf("File '%s' is %ld bytes long.\n\n",fileName.c_str(),length);
  // fseek(fp,0, SEEK_SET); //Move position to the start
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
      scene.AddSphere(Sphere(v,r,m));
      // numSpheres++;       
    }
    if (!strcmp(command, "background:")){ //sphere: -3 1 0 0.7
      float r,g,b;
      sscanf(line, "background: %f %f %f",&r,&g,&b);
      scene.SetBackColor(Color(r,g,b));      
    }
    if (!strcmp(command, "ambient_light:")){ //sphere: -3 1 0 0.7
      float r,g,b;
      sscanf(line, "ambient_light: %f %f %f",&r,&g,&b);
      scene.SetAmbientLight(Color(r,g,b));    
    }
    if (!strcmp(command, "directional_light:")){ //sphere: -3 1 0 0.7
      float r,g,b,x,y,z;
      sscanf(line, "directional_light: %f %f %f %f %f %f",&r,&g,&b,&x,&y,&z);
      scene.AddDirlight(DirectionalLight(Color(r,g,b),vec3(x,y,z)));
      // numDlights++;    
    }
    if (!strcmp(command, "point_light:")){ //sphere: -3 1 0 0.7
      float r,g,b,x,y,z;
      sscanf(line, "point_light: %f %f %f %f %f %f",&r,&g,&b,&x,&y,&z);
      scene.AddPointlight(PointLight(Color(r,g,b),vec3(x,y,z)));
      // numPlights++;    
    }
    if (!strcmp(command, "spot_light:")){ //sphere: -3 1 0 0.7
      float r,g,b,px,py,pz,dx,dy,dz,a1,a2;
      sscanf(line, "spot_light: %f %f %f %f %f %f %f %f %f %f %f",&r,&g,&b,&px,&py,&pz,&dx,&dy,&dz,&a1,&a2);
      a1 = cos(a1*PI/180.0); //convert once at the file load, so that this can be directly compared to dot product
      a2 = cos(a2*PI/180.0);
      scene.AddSpotlight(SpotLight(Color(r,g,b),vec3(px,py,pz),vec3(dx,dy,dz),a1,a2));
      // numSlights++;    
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
      camera.eye = vec3(x,y,z);
    }
    if (!strcmp(command, "camera_fwd:")){ //camera_fwd: -0.77 0 -0.66
      float x,y,z;
      sscanf(line, "camera_fwd: %f %f %f",&x,&y,&z);
      camera.forward = vec3(x,y,z).normalized();
    }
    if (!strcmp(command, "camera_up:")){ //camera_up:  0 1 0
      float x,y,z;
      sscanf(line, "camera_up: %f %f %f",&x,&y,&z);
      camera.up = vec3(x,y,z).normalized();
    }
    if (!strcmp(command, "camera_fov_ha:")){ //camera_fov_ha: 35
      float x;
      sscanf(line, "camera_fov_ha: %f",&x);
      camera.fov_h = x;
    }
    if (!strcmp(command, "max_depth:")){ //camera_fov_ha: 35
      sscanf(line, "max_depth: %n",&max_depth);
    }
    if (!strcmp(command, "sample_size:")){ //camera_fov_ha: 35
      sscanf(line, "sample_size: %n",&sample_size);
    }
  }

  //Create an orthagonal camera basis
  camera.forward = camera.forward.normalized();
  camera.up = (camera.up - dot(camera.up,camera.forward)*camera.forward).normalized();
  camera.right = cross(camera.up,camera.forward);
}

#endif