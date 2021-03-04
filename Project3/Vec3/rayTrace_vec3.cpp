//CSCI 5607 HW3 - Rays & Files
//This HW has three steps:
// 1. Compile and run the program (the program takes a single command line argument)
// 2. Understand the code in this file (rayTrace_vec3.cpp), in particular be sure to understand:
//     -How ray-sphere intersection works
//     -How the rays are being generated
//     -The pipeline from rays, to intersection, to pixel color
//    After you finish this step, and understand the math, take the HW quiz on canvas
// 3. Update the file parse_vec3.h so that the function parseSceneFile() reads the passed in file
//     and sets the relevant global variables for the rest of the code to product to correct image

//To Compile: g++ -fsanitize=address -std=c++11 rayTrace_vec3.cpp

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "image_lib.h" //Defines an image class and a color class

//#Vec3 Library
#include "vec3.h"

//High resolution timer
#include <chrono>

//Scene file parser
#include "parse_vec3.h"
#include "intersection_test.h"

Color GetColor(Material m, HitInfo hitInfo){
  float lr=0, lg=0, lb=0;
  //Get ambient light
  lr += alight.c.r * m.ar;
  lg += alight.c.g * m.ag;
  lb += alight.c.b * m.ab;
  //add on Point light
  for (int i=0;i<numPlights;i++){
    //check if the ray is blocked from the light source
    bool inShadow = false;
    for (int k=0;k<numSpheres;k++){
        HitInfo hitInfotemp = raySphereIntersect(hitInfo.hitPos,hitInfo.hitPos-plights[i].p,spheres[k].pos,spheres[k].r);
        if (hitInfotemp.hit) {inShadow = true; break;}
    }
    if (!inShadow){
      vec3 d = hitInfo.hitPos - plights[i].p;
      float temp = dot(hitInfo.hitNorm,hitInfo.l);
      float f = 1/d.length();
      if (temp>0){
        // printf("temp=%f\n",temp);
        lr += m.dr * plights[i].c.r * temp*f;
        lg += m.dg * plights[i].c.g * temp*f;
        lb += m.db * plights[i].c.b * temp*f;
      }
    }
  }
  //add on Directional light
  //add on Spot light
  return Color(lr,lg,lb);
  // return Color(m.ar,m.ag,m.ab);
}

int main(int argc, char** argv){

  //Read command line paramaters to get scene file
  if (argc != 2){
     std::cout << "Usage: ./a.out scenefile\n";
     return(0);
  }
  std::string secenFileName = argv[1];

  //Parse Scene File
  parseSceneFile(secenFileName);

  printf("ambient light: %f, %f, %f\n",alight.c.r,alight.c.g,alight.c.b);

  float imgW = img_width, imgH = img_height;
  float halfW = imgW/2, halfH = imgH/2;
  float d = halfH / tanf(halfAngleVFOV * (M_PI / 180.0f));

  Image outputImg = Image(img_width,img_height);
  auto t_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      // float t[numSpheres]; //interception time
      float tmin=999999;
      int iClosest=-1;
      float u = (halfW - (imgW)*(i/imgW));
      float v = (halfH - (imgH)*(j/imgH));
      vec3 p = eye - d*forward + u*right + v*up;
      vec3 rayDir = (p - eye).normalized();  //Normalizing here is optional
      HitInfo hitInfo;
      for (int k=0;k<numSpheres;k++){
        hitInfo = raySphereIntersect(eye,rayDir,spheres[k].pos,spheres[k].r);
        if (hitInfo.hit && hitInfo.t<tmin){
          tmin=hitInfo.t;
          iClosest=k;
        }
      }
      Color color;
      if (iClosest>=0){
        Material m=spheres[iClosest].m;
        color = GetColor(m,hitInfo);
      } else {
        color = bgColor;
      }
      // Color color=s
      // else color = Color(0,0,0);
      outputImg.setPixel(i,j, color);
      //outputImg.setPixel(i,j, Color(fabs(i/imgW),fabs(j/imgH),fabs(0))); //TODO: Try this, what is it visualizing?
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

  outputImg.write(imgName.c_str());
  return 0;
}