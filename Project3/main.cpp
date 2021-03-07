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
  
  srand(time(NULL));
  auto t_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      Color c_sum = Color(0,0,0);
      for (int sidx = 0; sidx < sample_size; sidx++){
        float u,v;
        if (sample_size>3){
          float jitter1 = (rand()%10)/10.0 - 0.5;
          float jitter2 = (rand()%10)/10.0 - 0.5;
          u = (halfW - (imgW)*((i+jitter1)/imgW));
          v = (halfH - (imgH)*((j+jitter2)/imgH));
        } else{
          u = (halfW - (imgW)*(i/imgW));
          v = (halfH - (imgH)*(j/imgH));
        }
        vec3 p = camera.eye - d*camera.forward + u*camera.right + v*camera.up;
        ray.d = (p - camera.eye).normalized();  //Normalizing here is optional
        
        Color color;
        if (scene.HitWInfo(ray,hitInfo)){
          color = hitInfo.c;
          hitInfo.t = MAX_T;
        } else {
          color = scene.GetBackground();
          // printf("bg color=%f,%f,%f\n",color.r,color.g,color.b);
          // assert(false);
        }
        c_sum = c_sum + color;
      }
      c_sum = c_sum * (1.0/sample_size);
      outputImg.SetPixel(i,j,c_sum);
      // outputImg.SetPixel(i,j,color);
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