//Project 3b
//image class, file parser and vec3 are from provided code in HW3 with modification
//openGL render code is from provided code in HW1.

#include "glad/glad.h"  //Include order can matter here

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

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
float rand_n1();
int log2_asm(int x);
void UpdateImage(Image* image, BoundingBox* BB, int numThreads);
void UpdateImage(Image* image, BoundingBox* BB, int sample_size, int numThreads);
void RerenderImage(Image* image, BoundingBox* BB, int numThreads);
static void ShowUsage(void);
void OpenGLRender(SDL_Window* window, Image* image);
void PixelRender(SDL_Window* window, Image* image, int step, int sample_size, BoundingBox* BB);

//Global variables
Scene* scene = new Scene();
Camera* camera = new Camera();
Camera* camera0 = new Camera(*camera); //store original data

int img_width = 640, img_height = 480;
int max_vertices, max_normals;
std::vector<vec3> vertexList;
std::vector<vec3> normalList;
std::vector<Material> materialList;




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
int main(int argc, char *argv[]){
   int sample_size = 1; //set default sample_size, may be overwritten by command input
   int numThreads = 8; //default number of threads, may be overwritten by command input
   
  //Read command line paramaters to get scene file
   if (argc < 2){
      // std::cout << "Usage: ./a.out scenefile\n";
      ShowUsage();
      return(0);
   }
   argv += 1, argc -= 1;
   std::string secenFileName = argv[0];
   argv += 1, argc -= 1;
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
         } else {
				fprintf(stderr, "ray: invalid option: %s\n", *argv);
				ShowUsage();
			}
		} else {
			fprintf(stderr, "ray: invalid option: %s\n", *argv);
			ShowUsage();
		}
   }
   printf("sample size = %d, number of threads = %d\n",sample_size,numThreads);
   vertexList.reserve(500);
   normalList.reserve(500);
   materialList.reserve(50);
   //for live pixel update: controls how many pixels are updated at once
   int step = (img_width>img_height)?img_width/25 : img_height/25; 
   
   auto t_start = std::chrono::high_resolution_clock::now();
   //Parse Scene File
   parseSceneFile(secenFileName, scene, camera);
   auto t_end = std::chrono::high_resolution_clock::now();
   printf("Parsing file took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

   t_start = std::chrono::high_resolution_clock::now();
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
   t_end = std::chrono::high_resolution_clock::now();
   printf("Building BVH took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

   Image* image = new Image(img_width,img_height);  
   if (sample_size>1) {UpdateImage(image, BB,sample_size,numThreads);} 
   else {UpdateImage(image, BB,numThreads);}

  //Update file name with time stamp
  char date[50];
  time_t t = time(NULL);
  struct tm *tm;
  int dotp = imgName.find_last_of(".");
  std::string fileName = imgName.substr(0,dotp);
  std::string ext = imgName.substr(dotp);
  std::string outFileName;
  
   t_start = std::chrono::high_resolution_clock::now();
   SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
   
   //Ask SDL to get a fairly recent version of OpenGL (3.2 or greater)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("Hailin's Project 3", 100, 100, img_width, img_height, SDL_WINDOW_OPENGL);
	
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

   image->Write(imgName.c_str());
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
   // bool gammaOn = false;   

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
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o) //If "b" is pressed
         {
            tm = localtime(&t);
            strftime(date, sizeof(date), "_%y%m%d_%H%M%S",tm);
            outFileName = fileName + string(date) + ext;
            image->Write(outFileName.c_str());
         }  
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_b) //If "w" is pressed
            {
               *camera = *camera0;
               printf("resetting the scene.\n");
               camera->PrintState();
               if (sample_size>1) {UpdateImage(image, BB,sample_size,numThreads);} 
               else {UpdateImage(image, BB,numThreads);}
               image->UpdateRawPixels();
               glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
               glGenerateMipmap(GL_TEXTURE_2D);
            }  
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE) //If "w" is pressed
            {
               PixelRender(window, image, step, sample_size,BB);
            }          
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_w) //If "w" is pressed
            {
               key_w_pressed();
               RerenderImage(image, BB,numThreads);
               }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_s) //If "s" is pressed
            {
               key_s_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_a) //If "a" is pressed
            {
               key_a_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_d) //If "d" is pressed
            {
               key_d_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_q) //If "o" is pressed
            {
               key_q_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_e) //If "o" is pressed
            {
               key_e_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT) //If "left arrow" is pressed
            {
               key_left_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT) //If "right arrow" is pressed
            {
               key_right_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP) //If "left arrow" is pressed
            {
               key_up_pressed();
               RerenderImage(image, BB,numThreads);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN) //If "right arrow" is pressed
            {
               key_down_pressed();
               RerenderImage(image, BB,numThreads);
            }
         // SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
//          t_end = std::chrono::high_resolution_clock::now();
//   printf("OpenGL Display took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());

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

float rand_n1(){
  return (rand()%100-50)/100.0;
}

int log2_asm(int x){
	uint32_t y;
	asm ("\tbsr %1, %0\n"
		: "=r"(y)
		: "r" (x)
	);
	return y;
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
      Color c_sum_p = Color(1,1,1);
      float diff = 1;
      int k=0;
      while ((k<5) || ((k<sample_size) && (diff > 0.1))){ //
         float u = (img_width/2.0 - img_width*((i+(k>0)*rand_n1())/img_width));
         float v = (img_height/2.0 - img_height*((j+(k>0)*rand_n1())/img_height));
         vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
         ray.d = (p - camera->eye).normalized();  //Normalizing here is optional
         Color c = scene->EvaluateRayTree(ray, BB);
         c_sum = c_sum + c;
         diff = c_sum.diff(c_sum_p);
         c_sum_p = c_sum;
         k++;    
      }
      image->SetPixel(i,j,c_sum * (1.0/k));
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());
}

//when sample_size is 1(default)
void UpdateImage(Image* image, BoundingBox* BB, int numThreads){
   // printf("samplesize=1\n");
   float d = img_height/2.0/ tanf(camera->fov_h * (M_PI / 180.0f));
  
   auto t_start = std::chrono::high_resolution_clock::now();
   #pragma omp parallel for schedule(dynamic, numThreads)
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      Ray ray = Ray();
      ray.p = camera->eye;
      ray.depth = max_depth;
      HitInfo hitInfo =HitInfo();
      float u = (img_width/2.0 - img_width*((float)i/img_width));
      float v = (img_height/2.0 - img_height*((float)j/img_height));
      vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
      ray.d = (p - camera->eye).normalized();  //Normalizing here is optional
      Color c = scene->EvaluateRayTree(ray, BB);
       image->SetPixel(i,j,c);
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  printf("Rendering took %.2f ms\n",std::chrono::duration<double, std::milli>(t_end-t_start).count());
}

void RerenderImage(Image* image, BoundingBox* BB, int numThreads){
   // sample_size = 1; max_depth = 2; //change parameter to speed up rendering at the expense of the image quality
   camera->Update(0.1); 
   camera->PrintState();
   UpdateImage(image, BB,numThreads); //sample_size=1 to speed up rendering
   image->UpdateRawPixels();
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->rawPixels);
   glGenerateMipmap(GL_TEXTURE_2D);
}

/**
 * ShowUsage
 **/
static char options[] =
"-samplesize <n>\n"
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

void PixelRender(SDL_Window* window, Image* image, int step, int sample_size, BoundingBox* BB){
   // *camera = *camera0;
   image = new Image(img_width,img_height); 
   memset(image->rawPixels,0,4*img_width*img_height); //Load all zeros
   OpenGLRender(window,image);
   camera->PrintState();
   float d = img_height/2.0/ tanf(camera->fov_h * (M_PI / 180.0f)); 
   
   int in = img_width/step, jn = img_height/step;
   int i,j;
   for (int j1 = 0; j1 < jn; j1++){ 
      for (int i1 = 0; i1 < in; i1++){  
         for (int j2=0;j2<step;j2++){
            for (int i2=0;i2<step;i2++){
               i=i1*step+i2; j=j1*step+j2;
               Ray ray = Ray();
               ray.p = camera->eye;
               ray.depth = max_depth;
               HitInfo hitInfo =HitInfo();
               Color c_sum = Color(0,0,0);
               Color c_sum_p = Color(1,1,1);
               float diff = 1;
               int k=0;
               while ((k<5) || ((k<sample_size) && (diff > 0.1))){ //
                  float u = (img_width/2.0 - img_width*((i+(k>0)*rand_n1())/img_width));
                  float v = (img_height/2.0 - img_height*((j+(k>0)*rand_n1())/img_height));
                  vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
                  ray.d = (p - camera->eye).normalized();  //Normalizing here is optional
                  Color c = scene->EvaluateRayTree(ray, BB);         
                  c_sum = c_sum + c;
                  diff = c_sum.diff(c_sum_p);
                  c_sum_p = c_sum;
                  k++;    
               }
               c_sum = c_sum * (1.0/k);
               image->SetPixel(i,j,c_sum);
               image->rawPixels[4*(i+j*img_width)+0] = uint8_t(fmin(c_sum.r,1)*255);
               image->rawPixels[4*(i+j*img_width)+1] = uint8_t(fmin(c_sum.g,1)*255);
               image->rawPixels[4*(i+j*img_width)+2] = uint8_t(fmin(c_sum.b,1)*255);
               image->rawPixels[4*(i+j*img_width)+3] = 255; //alpha  
            }
         }
         OpenGLRender(window,image);                
      }
      for (int i1=i;i1<img_width;i1++){
         for (int j2=0;j2<step;j2++){
            j=j1*step+j2;
            Ray ray = Ray();
            ray.p = camera->eye;
            ray.depth = max_depth;
            HitInfo hitInfo =HitInfo();
            Color c_sum = Color(0,0,0);
            Color c_sum_p = Color(1,1,1);
            float diff = 1;
            int k=0;
            while ((k<5) || ((k<sample_size) && (diff > 0.1))){ //
               float u = (img_width/2.0 - img_width*((i1+(k>0)*rand_n1())/img_width));
               float v = (img_height/2.0 - img_height*((j+(k>0)*rand_n1())/img_height));
               vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
               ray.d = (p - camera->eye).normalized();  //Normalizing here is optional
               Color c = scene->EvaluateRayTree(ray, BB);         
               c_sum = c_sum + c;
               diff = c_sum.diff(c_sum_p);
               c_sum_p = c_sum;
               k++;    
            }
            c_sum = c_sum * (1.0/k);
            image->SetPixel(i1,j,c_sum);
            image->rawPixels[4*(i1+j*img_width)+0] = uint8_t(fmin(c_sum.r,1)*255);
            image->rawPixels[4*(i1+j*img_width)+1] = uint8_t(fmin(c_sum.g,1)*255);
            image->rawPixels[4*(i1+j*img_width)+2] = uint8_t(fmin(c_sum.b,1)*255);
            image->rawPixels[4*(i1+j*img_width)+3] = 255; //alpha  
         }
      }
      OpenGLRender(window,image);      
   }
     
   for (int i1=0;i1<img_width;i1++){
      for (int j1=j;j1<img_height;j1++){
         Ray ray = Ray();
               ray.p = camera->eye;
               ray.depth = max_depth;
               HitInfo hitInfo =HitInfo();
               Color c_sum = Color(0,0,0);
               Color c_sum_p = Color(1,1,1);
               float diff = 1;
               int k=0;
               while ((k<5) || ((k<sample_size) && (diff > 0.1))){ //
                  float u = (img_width/2.0 - img_width*((i1+(k>0)*rand_n1())/img_width));
                  float v = (img_height/2.0 - img_height*((j1+(k>0)*rand_n1())/img_height));
                  vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
                  ray.d = (p - camera->eye).normalized();  //Normalizing here is optional
                  Color c = scene->EvaluateRayTree(ray, BB);         
                  c_sum = c_sum + c;
                  diff = c_sum.diff(c_sum_p);
                  c_sum_p = c_sum;
                  k++;    
               }
               c_sum = c_sum * (1.0/k);
               image->SetPixel(i1,j1,c_sum);
               image->rawPixels[4*(i1+j1*img_width)+0] = uint8_t(fmin(c_sum.r,1)*255);
               image->rawPixels[4*(i1+j1*img_width)+1] = uint8_t(fmin(c_sum.g,1)*255);
               image->rawPixels[4*(i1+j1*img_width)+2] = uint8_t(fmin(c_sum.b,1)*255);
               image->rawPixels[4*(i1+j1*img_width)+3] = 255; //alpha  
      }
   }
   OpenGLRender(window,image); 
}