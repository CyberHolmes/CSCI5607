//CSCI 5607 Project3 - Ray Tracing

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "image.h" //Defines an image class and a color class

//#Vec3 Library
#include "vec3.h"

//High resolution timer
#include <chrono>

//Scene file parser
#include "parse_vec3.h"
#include "objects.h"
#include "ray.h"
#include <math.h>

int main(int argc, char** argv){

  //Read command line paramaters to get scene file
  if (argc != 2){
     std::cout << "Usage: ./a.out scenefile\n";
     return(0);
  }
  std::string secenFileName = argv[1];

  Scene scene = Scene();
  Camera camera;

  //Parse Scene File
  parseSceneFile(secenFileName, scene, camera);

  float imgW = img_width, imgH = img_height;
  float halfW = imgW/2, halfH = imgH/2;
  float d = halfH / tanf(camera.fov_h * (M_PI / 180.0f));
  Ray ray;
  ray.eye = camera.eye;
  ray.depth = max_depth;

  Image outputImg = Image(img_width,img_height);
  HitInfo hitInfo;

  auto t_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      float u = (halfW - (imgW)*(i/imgW));
      float v = (halfH - (imgH)*(j/imgH));
      vec3 p = camera.eye - d*camera.forward + u*camera.right + v*camera.up;
      ray.dir = (p - camera.eye).normalized();  //Normalizing here is optional
      
      Color color;
      if (scene.HitWInfo(ray.eye,ray.dir,hitInfo)){
        // printf("ray = %f,%f,%f; raydir = %f,%f,%f\n",eye.x,eye.y,eye.z,rayDir.x,rayDir.y,rayDir.z);
        // printf("hitNormal = %f,%f,%f\n",hitInfo.hitNorm.x,hitInfo.hitNorm.y,hitInfo.hitNorm.z);
        // printf("v = %f,%f,%f\n",hitInfo.v.x,hitInfo.v.y,hitInfo.v.z);
        // printf("hitpos = %f,%f,%f\n",hitInfo.hitPos.x,hitInfo.hitPos.y,hitInfo.hitPos.z);
        // printf("sphere = %f,%f,%f, r = %f\n",scene.GetSphere(hitInfo.objI).pos.x,scene.GetSphere(hitInfo.objI).pos.y,scene.GetSphere(hitInfo.objI).pos.z,scene.GetSphere(hitInfo.objI).r);
        // printf("color = %f,%f,%f\n",hitInfo.c.r,hitInfo.c.g,hitInfo.c.b);
        // assert(false);
        color = hitInfo.c;
        hitInfo.t = MAX_T;
      } else {
        color = scene.GetBackground();
        // printf("no hit\n");
        // assert(false);
      }
      outputImg.SetPixel(i,j, color);
      //outputImg.setPixel(i,j, Color(fabs(i/imgW),fabs(j/imgH),fabs(0))); //TODO: Try this, what is it visualizing?
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

  outputImg.Write(imgName.c_str());
  // delete scene;
  // scene = nullptr;
  return 0;
}