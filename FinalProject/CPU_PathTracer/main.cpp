//Project 3b
//image class, file parser and vec3 are from provided code in HW3 with modification
//openGL render code is from provided code in HW1.

#include "glad/glad.h"  //Include order can matter here
#ifdef _MSC_VER
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include "./oidn/include/OpenImageDenoise/oidn.hpp"

using namespace std;

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "image.h" //Defines an image class and a color class

//#Vec3 Library
#include "vec3.h"
#include "bvh.h"

//High resolution timer
#include <chrono>

//Scene file parser
#include "parse_vec3.h"
#include "objects.h"
#include "ray.h"
#include <math.h>
#include <set>
#include <string>
#include <typeinfo>

static void CheckOption(char *option, int argc, int minargc);
void UpdateImage(Image* image, BoundingBox* BB, int sample_size, int numThreads);
void RerenderImage(Image* image, Image* image_denoise, BoundingBox* BB, int sample_size, int numThreads);
static void ShowUsage(void);
void OpenGLRender(SDL_Window* window, Image* image);
void Denoise(Image* imgin, Image* imgout);

//Global variables
Scene* scene = new Scene();
Camera* camera = new Camera();
Camera* camera0 = new Camera(*camera); //store original data

int img_width = 640, img_height = 480;
int max_vertices, max_normals;
std::vector<vec3> vertexList;
std::vector<vec3> normalList;
std::vector<vec3> uvList;
std::vector<Material> materialList;
std::vector<Image> textureList;




float vertices[] = {  //The function updateVertices() changes these values to match p1,p2,p3,p4
//  X     Y     R     G     B     U    V  
  -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left3
  -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 4 
  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right 2   
};

/////////////////////////////
/// ... below is OpenGL specifc code,
////////////////////////////

// Shader sources
const GLchar* vertexSource =
   "#version 150 core\n"
   "in vec2 position;"
   "in vec3 inColor;"
   "in vec2 inTexcoord;"
   "out vec3 Color;"
   "out vec2 texcoord;"
   "void main() {"
   "   Color = inColor;"
   "   gl_Position = vec4(position, 0.0, 1.0);"
   "   texcoord = inTexcoord;"
   "}";
    
const GLchar* fragmentSource =
   "#version 150 core\n"
   "uniform sampler2D tex0;"
   "in vec2 texcoord;"
   "out vec3 outColor;"
   "void main() {"
   "   outColor = texture(tex0, vec2(texcoord.x, 1.0-texcoord.y)).rgb;"
   "}";
   
bool fullscreen = false;
float mouse_dragging = false;
bool pathTrace = false; //default is ray trace
bool uselight = true; //path tracing lighting option control
int main(int argc, char *argv[]){
   int sample_size = 1000; //set default sample_size, may be overwritten by command input
   int numThreads = 8; //default number of threads, may be overwritten by command input
   
  //Read command line paramaters to get scene file
   if (argc < 2){
      // std::cout << "Usage: ./a.out scenefile\n";
      ShowUsage();
      int val;
      std::cin >> val;
      return(0);
   }
   argv += 1, argc -= 1;
   std::string secenFileName = argv[0];
   argv += 1, argc -= 1;

   //Parse Scene File
   parseSceneFile(secenFileName, scene, camera);

   while (argc>0){
      if (**argv == '-'){
         if (!strcmp(*argv, "-samplesize"))
         {
            // printf("argc=%d\n",argc);
            // printf("argv=%s\n",argv[1]);
            CheckOption(*argv, argc, 2);
            // printf("argc=%d\n",argc);
            // printf("argv=%s\n",argv[1]);
            sample_size = atoi(argv[1]);
            argv += 2, argc -= 2;
         } else if (!strcmp(*argv, "-numthreads"))
         {
            CheckOption(*argv, argc, 2);
            numThreads = atoi(argv[1]);
            argv += 2, argc -= 2;
         } else if (!strcmp(*argv, "-depth"))
         {
            CheckOption(*argv, argc, 2);
            max_depth = atoi(argv[1]);
            argv += 2, argc -= 2;
         } else if (!strcmp(*argv, "-pathtrace"))
         {
            pathTrace = true;
            argv += 1, argc -= 1;
         } else {
				fprintf(stderr, "ray: invalid option: %s\n", *argv);
				ShowUsage();
			}
		} else {
			fprintf(stderr, "ray: invalid option: %s\n", *argv);
			ShowUsage();
		}
   }
   
   vertexList.reserve(500);
   normalList.reserve(500);
   materialList.reserve(50);

   printf("sample size = %d, ray depth = %d, number of threads = %d\n",
      sample_size, max_depth,numThreads);
   
   scene->SetMaxDepth(max_depth);
   
   auto t_start = std::chrono::high_resolution_clock::now();
   //BVH for ray tracer
   std::vector<Obj*> objList = scene->GetObjects();
   vec3 minV = vec3(MAX_T,MAX_T,MAX_T), maxV = vec3(-MAX_T,-MAX_T,-MAX_T);
   // printf("number objects = %ld\n",objList.size());
   for ( auto const& obj : objList){
      vec3 curMinV = obj->GetBoundMin();
      vec3 curMaxV = obj->GetBoundMax();
      // printf("pos=%f,%f,%f\n",obj->GetPos().x,obj->GetPos().y,obj->GetPos().z);
      minV.x = (minV.x>curMinV.x)?curMinV.x:minV.x;
      minV.y = (minV.y>curMinV.y)?curMinV.y:minV.y;
      minV.z = (minV.z>curMinV.z)?curMinV.z:minV.z;
      maxV.x = (maxV.x<curMaxV.x)?curMaxV.x:maxV.x;
      maxV.y = (maxV.y<curMaxV.y)?curMaxV.y:maxV.y;
      maxV.z = (maxV.z<curMaxV.z)?curMaxV.z:maxV.z;
   }
   BoundingBox* BB = BuildBVHTree(objList,minV,maxV,log2_asm(objList.size())+2);

   auto t_end = std::chrono::high_resolution_clock::now();
   printf("Building BVH took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());
   img_width = 300;img_height = 300;
   Image* image = new Image(img_width,img_height); 
   Image* image_denoise = new Image(img_width,img_height); 
   UpdateImage(image, BB,sample_size,numThreads);
   Denoise(image,image_denoise);

  //Update file name with time stamp
  char date[50];
  time_t t = time(NULL);
  struct tm *tm;
  int dotp = imgName.find_last_of(".");
  std::string fileName = imgName.substr(0,dotp);
  std::string ext = imgName.substr(dotp);
  std::string outFileName, outFileName_denoise;
  
   t_start = std::chrono::high_resolution_clock::now();
   SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
   
   //Ask SDL to get a fairly recent version of OpenGL (3.2 or greater)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("CPU Path Tracer", 100, 100, img_width, img_height, SDL_WINDOW_OPENGL);
	
	//The above window cannot be resized which makes some code slightly easier.
	//Below show how to make a full screen window or allow resizing
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen

   float aspect = img_width/(float)img_height; //aspect ratio (needs to be updated if the window is resized)
	
	// updateVertices(); //set initial position of the square to match it's state
	
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//OpenGL functions using glad library
   if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
      printf("OpenGL loaded\n");
      printf("Vendor:   %s\n", glGetString(GL_VENDOR));
      printf("Renderer: %s\n", glGetString(GL_RENDERER));
      printf("Version:  %s\n", glGetString(GL_VERSION));
   }
   else {
      printf("ERROR: Failed to initialize OpenGL context.\n");
      return -1;
   }

   //// Allocate Texture 0 (Created in Load Image) ///////
   GLuint tex0;
   glGenTextures(1, &tex0);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex0);

   //What to do outside 0-1 range
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR, GL_NEAREST
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR, GL_NEAREST

   // image->Write(imgName.c_str());
   tm = localtime(&t);
   strftime(date, sizeof(date), "_%y%m%d_%H%M%S",tm);
   outFileName = fileName + string(date) + ext;   
   image->Write(outFileName.c_str());
   outFileName_denoise = fileName + string("_denoise") + string(date) + ext;
   image_denoise->Write(outFileName_denoise.c_str());
    //memset(img_data,0,4*img_w*img_h); //Load all zeros
   //Load the texture into memory
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
   glGenerateMipmap(GL_TEXTURE_2D);
   //// End Allocate Texture ///////


   //Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
   GLuint vao;
   glGenVertexArrays(1, &vao); //Create a VAO
   glBindVertexArray(vao); //Bind the above created VAO to the current context


   //Allocate memory on the graphics card to store geometry (vertex buffer object)
   GLuint vbo;
   glGenBuffers(1, &vbo);  //Create 1 buffer called vbo
   glBindBuffer(GL_ARRAY_BUFFER, vbo); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
   //GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
   //GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used


   //Load the vertex Shader
   GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
   glShaderSource(vertexShader, 1, &vertexSource, NULL);
   glCompileShader(vertexShader);

   //Let's double check the shader compiled 
   GLint status;
   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
   if (!status){
      char buffer[512];
      glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
      printf("Vertex Shader Compile Failed. Info:\n\n%s\n",buffer);
   }

   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
   glCompileShader(fragmentShader);

   //Double check the shader compiled 
   glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
   if (!status){
      char buffer[512];
      glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
      printf("Fragment Shader Compile Failed. Info:\n\n%s\n",buffer);
   }

   //Join the vertex and fragment shaders together into one program
   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
   glLinkProgram(shaderProgram); //run the linker

   glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)


   //Tell OpenGL how to set fragment shader input 

   GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
   //               Attribute, vals/attrib., type, normalized?, stride, offset
   glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
   glEnableVertexAttribArray(posAttrib); //Binds the VBO current GL_ARRAY_BUFFER 

   GLint colAttrib = glGetAttribLocation(shaderProgram, "inColor");
   glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));
   glEnableVertexAttribArray(colAttrib);

   GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
   glEnableVertexAttribArray(texAttrib);
   glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
      
   t_end = std::chrono::high_resolution_clock::now();
  printf("OpenGL Prep took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

   //Event Loop (Loop forever processing each event as fast as possible)
   SDL_Event windowEvent;
   bool done = false;
   bool dragging = false;
   vec3 mouse_pos;
   bool show_denoise = false;     

   while (!done){
      // t_start = std::chrono::high_resolution_clock::now();
      while (SDL_PollEvent(&windowEvent)){  //Process input events (e.g., mouse & keyboard)
         if (windowEvent.type == SDL_QUIT) done = true;
         //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
         //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
            done = true; //Exit event loop
         // if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
         //    fullscreen = !fullscreen;
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o) //output image file
         {
            t = time(NULL);
            tm = localtime(&t);
            strftime(date, sizeof(date), "_%y%m%d_%H%M%S",tm);
            outFileName = fileName + string(date) + ext;
            image->Write(outFileName.c_str());
            outFileName_denoise = fileName + string("_denoise") + string(date) + ext;
            image_denoise->Write(outFileName_denoise.c_str());
         }  
          
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_p) //p - toggle between path trace and ray trace
            {
               pathTrace = !pathTrace;
               RerenderImage(image, image_denoise, BB,sample_size, numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_l) //l - path trace lighting option control, does nothing if ray trace
            {
               if (pathTrace){
                  uselight = !uselight;
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
               }
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE) //space - show denoise image
            {
               show_denoise = !show_denoise;
               if (show_denoise) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_denoise->rawPixels);
               else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
               glGenerateMipmap(GL_TEXTURE_2D);
            }
         if (!pathTrace){
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_b) //b - reset the camera
            {
               *camera = *camera0;
               printf("resetting the scene.\n");
               camera->PrintState();
               UpdateImage(image, BB,sample_size,numThreads);
               image->UpdateRawPixels();
               glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
               glGenerateMipmap(GL_TEXTURE_2D);
            } 
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_w) //If "w" is pressed
               {
                  key_w_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
                  }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_s) //If "s" is pressed
               {
                  key_s_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_a) //If "a" is pressed
               {
                  key_a_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_d) //If "d" is pressed
               {
                  key_d_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_q) //If "o" is pressed
               {
                  key_q_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_e) //If "o" is pressed
               {
                  key_e_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT) //If "left arrow" is pressed
               {
                  key_left_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT) //If "right arrow" is pressed
               {
                  key_right_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP) //If "left arrow" is pressed
               {
                  key_up_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN) //If "right arrow" is pressed
               {
                  key_down_pressed();
                  RerenderImage(image, image_denoise, BB,sample_size, numThreads);
                  camera->PrintState();
               }
         }
         // SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
//          t_end = std::chrono::high_resolution_clock::now();
//   printf("OpenGL Display took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

      }     
      int mx, my;
      if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
          bool dirty = false;
          if (!dragging) {
              // mouse click
              mouse_pos = vec3(mx, my, 0);
          }
          else {
              // mouse drag
              vec3 cur_pos(mx, my, 0);
              // get difference between last mouse position and current mouse position
              vec3 offset = cur_pos - mouse_pos;
              mouse_pos = cur_pos;
              if (offset.x > 0) {
                  // moved right
                  key_right_pressed();
                  dirty = true;
              }
              else if (offset.x < 0) {
                  // moved left
                  key_left_pressed();
                  dirty = true;
              }
              if (offset.y > 0) {
                  // moved up
                  key_up_pressed();
                  dirty = true;
              }
              else if (offset.y < 0) {
                  // moved down
                  key_down_pressed();
                  dirty = true;
              }
              SDL_WarpMouseInWindow(window,img_width/2, img_height/2); //reset mouse position
          }
          dragging = true;
          if (dirty) RerenderImage(image, image_denoise, BB, sample_size, numThreads);
      }
      else {
          mouse_pos = vec3(mx, my, 0);
          dragging = false;
      }
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
      
      // Clear the screen to grey
      glClearColor(0.6f, 0.6, 0.6f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLE_STRIP, 4, 4); //Draw the two triangles (4 vertices) making up the square
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Draw the two triangles (4 vertices) making up the square

      SDL_GL_SwapWindow(window); //Double buffering
   }
   DeallocateBVHTree(BB);
   BB = NULL;
   // delete [] img_data;
   delete image;
   delete image_denoise;
   delete camera;
   delete camera0;
   delete scene;
   
   glDeleteProgram(shaderProgram);
   glDeleteShader(fragmentShader);
   glDeleteShader(vertexShader);
   glDeleteBuffers(1, &vbo);
   glDeleteVertexArrays(1, &vao);
   
   //Clean Up
   SDL_GL_DeleteContext(context);
   SDL_Quit();
   return 0;
}


/**
 * CheckOption
 **/
static void CheckOption(char *option, int argc, int minargc){
	if (argc < minargc){
		fprintf(stderr, "Too few arguments for %s\n", option);
		ShowUsage();
	}
}

void UpdateImage(Image* image, BoundingBox* BB, int sample_size, int numThreads){
   // printf("samplesize=%d\n",sample_size);
   float d = img_height/2.0/ tanf(camera->fov_h * (M_PI / 180.0f));
  
   auto t_start = std::chrono::high_resolution_clock::now();
   #pragma omp parallel for schedule(dynamic, numThreads)
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      Ray ray = Ray();
      ray.p = camera->eye;
      ray.depth = max_depth;
      HitInfo hitInfo =HitInfo();
      Color c_sum = Color(0,0,0);
      Color c = Color(0,0,0);
      Color c_p = Color(1,1,1);
      float diff = 1;
      int k=0;
      while (k<sample_size){ //
         float u = (img_width/2.0 - img_width*((i+(k>0)*randn1)/img_width));
         float v = (img_height/2.0 - img_height*((j+(k>0)*randn1)/img_height));
         vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
         ray.d = (p - camera->eye).normalized();  //Normalizing here is optional         
         if (pathTrace){
            c = scene->TracePath(ray, BB, max_depth, uselight);
         } else {
            c = scene->EvaluateRayTree(ray, BB);
            if (c.diff(c_p)<0.1) break;
            c_p = c;
         }
         c_sum = c_sum + c;
         k++;
      }
      // Tone mapping the result so colors don't white out
      Color val = (c_sum * (1.0 / k));
      Color tone_mapped(val.r / (val.r + 1), val.g / (val.g + 1), val.b / (val.b + 1));
      image->SetPixel(i,j,tone_mapped);
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());
}

void RerenderImage(Image* image, Image* image_denoise, BoundingBox* BB, int sample_size, int numThreads){
   // sample_size = 1; max_depth = 2; //change parameter to speed up rendering at the expense of the image quality
   camera->Update(0.1);    
   UpdateImage(image, BB,sample_size, numThreads); //sample_size=1 to speed up rendering
   image->UpdateRawPixels();
   Denoise(image,image_denoise);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
   glGenerateMipmap(GL_TEXTURE_2D);
}

/**
 * ShowUsage
 **/
static char options[] =
"-samplesize <n>\n"
"-depth <n>\n"
"-numthreads <n>\n"
;

static void ShowUsage(void)
{
	fprintf(stderr, "Usage: ./ray scenefile\n");
	fprintf(stderr, "%s", options);
	exit(EXIT_FAILURE);
}

void OpenGLRender(SDL_Window* window, Image* image){
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
   glGenerateMipmap(GL_TEXTURE_2D);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo      
   // Clear the screen to grey
   glClearColor(0.6f, 0.6, 0.6f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glDrawArrays(GL_TRIANGLE_STRIP, 4, 4); //Draw the two triangles (4 vertices) making up the square
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Draw the two triangles (4 vertices) making up the square
   SDL_GL_SwapWindow(window); //Double buffering   
}

void Denoise(Image* imgin, Image* imgout){
   float* img_data = NULL;
   float* out_img_data = NULL;
   int n = 0;

   img_data = new float[img_width*img_height*3];   
   out_img_data = new float[img_width*img_height*3];
   
   for (int j=0;j<img_height;j++){
    for (int i=0;i<img_width;i++){  
        Color c = imgin->GetPixel(i,j);  
        img_data[n] = c.r; n++;
        img_data[n] = c.g; n++;
        img_data[n] = c.b; n++;   
       }
   }

    // Create an Intel Open Image Denoise device
   OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
   oidnCommitDevice(device);

   // Create a denoising filter
   OIDNFilter filter = oidnNewFilter(device, "RT"); // generic ray tracing filter
   oidnSetSharedFilterImage(filter, "color",  img_data,
                           OIDN_FORMAT_FLOAT3, img_width, img_height, 0, 0, 0);
   // oidnSetSharedFilterImage(filter, "albedo", albedoPtr,
   //                          OIDN_FORMAT_FLOAT3, width, height, 0, 0, 0); // optional
   // oidnSetSharedFilterImage(filter, "normal", normalPtr,
   //                          OIDN_FORMAT_FLOAT3, width, height, 0, 0, 0); // optional
   oidnSetSharedFilterImage(filter, "output", out_img_data,
                           OIDN_FORMAT_FLOAT3, img_width, img_height, 0, 0, 0);
   oidnSetFilter1b(filter, "hdr", true); // image is HDR
   oidnCommitFilter(filter);

   // Filter the image
   oidnExecuteFilter(filter);

   // Check for errors
   const char* errorMessage;
   if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
   printf("Error: %s\n", errorMessage);
    //Output image file
    n=0;
   for (int j=0;j<img_height;j++){
    for (int i=0;i<img_width;i++){
        // printf("i=%d,j=%d\n",i,j);
        float r = out_img_data[n];n++;
        float g = out_img_data[n];n++;
        float b = out_img_data[n];n++;
        imgout->SetPixel(i,j,Color(r,g,b));
       }
   }
   delete img_data; delete out_img_data;
}