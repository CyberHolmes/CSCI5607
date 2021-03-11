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
#include <set>

float rand_n1(){
  return (rand()%100-50)/100.0;
}

Color ComputeColor(const Camera& camera, Scene &scene, const float &d, int sample_size, const float &imgW, const float &imgH, Ray &ray, const int &i, const int &j){
  HitInfo hitInfo =HitInfo();
  Color c_sum = Color(0,0,0);
  Color c_sum_p = Color(1,1,1);
  float diff = 1;
  int k=0;

  while ((k<3) || ((k<sample_size) && (diff > 0.01))){
    float u = (imgW/2.0 - imgW*((i+(k>0)*rand_n1())/imgW));
    float v = (imgH/2.0 - imgH*((j+(k>0)*rand_n1())/imgH));
    vec3 p = camera.eye - d*camera.forward + u*camera.right + v*camera.up;
    ray.d = (p - camera.eye).normalized();  //Normalizing here is optional

    Color color = scene.EvaluateRayTree(ray);

    c_sum = c_sum + color;
    diff = c_sum.diff(c_sum_p);
    c_sum_p = c_sum;
    k++;    
  }
  c_sum = c_sum * (1.0/k);
  return c_sum;
}

Color ComputeColor(const Camera& camera, Scene &scene, const float &d, const float &imgW, const float &imgH, Ray &ray, const int &i, const int &j){
  HitInfo hitInfo= HitInfo();
    float u = (imgW/2.0 - (imgW)*((i)/imgW));
    float v = (imgH/2.0 - (imgH)*((j)/imgH));
    vec3 p = camera.eye - d*camera.forward + u*camera.right + v*camera.up;
    ray.d = (p - camera.eye).normalized();  //Normalizing here is optional
    Color color = scene.EvaluateRayTree(ray);
  return color;
}

int main(int argc, char** argv){

  //Read command line paramaters to get scene file
  if (argc != 2){
     std::cout << "Usage: ./a.out scenefile\n";
     return(0);
  }
  std::string secenFileName = argv[1];

  Scene scene = Scene();
  Camera camera;
  int sample_size = 10;

  //Parse Scene File
  parseSceneFile(secenFileName, scene, camera, sample_size);

  float imgW = img_width, imgH = img_height;
  float halfW = imgW/2, halfH = imgH/2;
  float d = halfH / tanf(camera.fov_h * (M_PI / 180.0f));
  Ray ray;
  ray.p = camera.eye;
  ray.depth = max_depth;

  Image outputImg = Image(img_width,img_height);
  HitInfo hitInfo;
  
  
  auto t_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      Color color;
      if (sample_size>1){
        color = ComputeColor(camera, scene, d, sample_size, imgW, imgH, ray, i, j);
      } else {
        color = ComputeColor(camera, scene, d, imgW, imgH, ray, i, j);
      }

      outputImg.SetPixel(i,j,color);
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

  outputImg.Write(imgName.c_str());
  return 0;
}