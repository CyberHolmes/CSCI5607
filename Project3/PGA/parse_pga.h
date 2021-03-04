
//Set the global scene parameter variables
//TODO: Set the scene parameters based on the values in the scene file

#ifndef PARSE_PGA_H
#define PARSE_PGA_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>

#define MAX_LEN 1024 //maximum length of a line

//Camera & Scene Parmaters (Global Variables)
//Here we set default values, override them in parseSceneFile()

//Image Parmaters
int img_width = 800, img_height = 600;
std::string imgName = "raytraced.png";

//Camera Parmaters
Point3D eye = Point3D(0,0,0); 
Dir3D forward = Dir3D(0,0,-1).normalized();
Dir3D up = Dir3D(0,1,0).normalized();
Dir3D right = Dir3D(-1,0,0).normalized();
float halfAngleVFOV = 35; 

//Scene (Sphere) Parmaters
Point3D spherePos = Point3D(0,0,2);
float sphereRadius = 1; 

void parseSceneFile(std::string fileName){
  //TODO: Override the default values with new data from the file "fileName"
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
  fseek(fp,0,SEEK_END); //move position to the end
  length =ftell(fp); //value of current position
  printf("File '%s' is %ld bytes long.\n\n",fileName.c_str(),length);
  fseek(fp,0, SEEK_SET); //Move position to the start

  //read each line
  while (fgets(line, MAX_LEN, fp)){
    if (line[0]=='#') continue; //skip comment

    char command[100]; //container for command    
    
    if (sscanf(line,"%s ",command)<1) continue;
    if (!strcmp(command, "sphere:")){ //sphere: -3 1 0 0.7
      printf("reading sphere par\n");
      float x,y,z,r;
      sscanf(line, "sphere: %f %f %f %f",&x,&y,&z,&r);
      spherePos = Point3D(x,y,z);
      sphereRadius = r;        
    }
    if (!strcmp(command, "image_resolution:")){ //image_resolution: 500 300
      printf("reading sphere par\n");
      sscanf(line, "image_resolution: %d %d",&img_width,&img_height);
    }
    if (!strcmp(command, "output_image:")){ //output_image: f2-down-and-left.jpg
      printf("reading output_image par\n");
      char temp[1024];
      sscanf(line, "output_image: %s",temp);
      imgName = temp;
    }
    if (!strcmp(command, "camera_pos:")){ //camera_pos: -6 2 -4
      printf("reading camera_pos par\n");
      float x,y,z;
      sscanf(line, "camera_pos: %f %f %f",&x,&y,&z);
      eye = Point3D(x,y,z);
    }
    if (!strcmp(command, "camera_fwd:")){ //camera_fwd: -0.77 0 -0.66
      printf("reading camera_fwd par\n");
      float x,y,z;
      sscanf(line, "camera_fwd: %f %f %f",&x,&y,&z);
      forward = Dir3D(x,y,z).normalized();
    }
    if (!strcmp(command, "camera_up:")){ //camera_up:  0 1 0
      printf("reading camera_up par\n");
      float x,y,z;
      sscanf(line, "camera_up: %f %f %f",&x,&y,&z);
      up = Dir3D(x,y,z).normalized();
    }
    if (!strcmp(command, "camera_fov_ha:")){ //camera_fov_ha: 35
      printf("camera_fov_ha\n");
      sscanf(line, "camera_fov_ha: %f",&halfAngleVFOV);
      // halfAngleVFOV = halfAngleVFOV/2;
    }
  }

  //TODO: Create an orthagonal camera basis, based on the provided up and right vectors
  forward = forward.normalized();
  up = (up - forward*dot(up,forward)).normalized();
  right = cross(up,forward);
  // right = (right - forward*dot(right,forward)).normalized();
  printf("Orthagonal Camera Basis:\n");
  forward.print("forward");
  right.print("right");
  up.print("up");
  printf("sphere:\n");
  spherePos.print();
}

#endif