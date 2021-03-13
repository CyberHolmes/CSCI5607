//Project 1 - Moving Square
//This code assumes SDL2 and OpenGL are both properly installed on your system

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

//Global variables
Scene* scene = new Scene();
Camera* camera = new Camera();
int sample_size = 25;
Ray ray = Ray();
int img_width = 640, img_height = 480;

vec3 negativeMovement;
vec3 positiveMovement;
vec3 negativeTurn;
vec3 positiveTurn;


void UpdateImage(Image* image){

   float d = img_height/2.0/ tanf(camera->fov_h * (M_PI / 180.0f));
  ray.p = camera->eye;
  ray.depth = max_depth;
   auto t_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < img_width; i++){
    for (int j = 0; j < img_height; j++){
      HitInfo hitInfo =HitInfo();
      Color c_sum = Color(0,0,0);
      Color c_sum_p = Color(1,1,1);
      float diff = 1;
      int k=0;
      while ((k<3) || ((k<sample_size) && (diff > 0.01))){
         float u = (img_width/2.0 - img_width*((i+(k>0)*rand_n1())/img_width));
         float v = (img_height/2.0 - img_height*((j+(k>0)*rand_n1())/img_height));
         vec3 p = camera->eye - d*camera->forward + u*camera->right + v*camera->up;
         ray.d = (p - camera->eye).normalized();  //Normalizing here is optional
         Color c = scene->EvaluateRayTree(ray);
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

float vertices[] = {  //The function updateVertices() changes these values to match p1,p2,p3,p4
//  X     Y     R     G     B     U    V  
  -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left3
  -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 4 
  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right 2   
};

// float vertices[] = {  //The function updateVertices() changes these values to match p1,p2,p3,p4
// //  X     Y     R     G     B     U    V     
//   1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right 2
//   1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right 
//   -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 4 
//   -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left3   
// };



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

void RerenderImage(Image* image, unsigned char* img_data){
   sample_size = 1; max_depth = 2; //change parameter to speed up rendering at the expense of the image quality
   camera->Update(0.1); 
   camera->PrintState();
   UpdateImage(image);
   img_data = image->ToBytes();
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
   glGenerateMipmap(GL_TEXTURE_2D);
}
    
bool fullscreen = false;

float mouse_dragging = false;
int main(int argc, char *argv[]){

   //Read command line paramaters to get scene file
  if (argc != 2){
     std::cout << "Usage: ./a.out scenefile\n";
     return(0);
  }
  std::string secenFileName = argv[1];

  //Parse Scene File
  parseSceneFile(secenFileName, scene, camera, sample_size);
  Image* image = new Image(img_width,img_height);  
  UpdateImage(image);

  //Update file name with time stamp
  char date[50];
  time_t t = time(NULL);
  struct tm *tm;
  int dotp = imgName.find_last_of(".");
  std::string fileName = imgName.substr(0,dotp);
  std::string ext = imgName.substr(dotp);
  std::string outFileName;
  
   
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
   //TODO: TEST your understanding: Try GL_LINEAR instead of GL_NEAREST on the 4x4 test image. What is happening?


   unsigned char* img_data = image->ToBytes();
   image->Write(imgName.c_str());
    //memset(img_data,0,4*img_w*img_h); //Load all zeros
   //Load the texture into memory
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
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
      

   //Event Loop (Loop forever processing each event as fast as possible)
   SDL_Event windowEvent;
   bool done = false;
   // bool gammaOn = false;   

   while (!done){
      while (SDL_PollEvent(&windowEvent)){  //Process input events (e.g., mouse & keyboard)
         if (windowEvent.type == SDL_QUIT) done = true;
         //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
         //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
            done = true; //Exit event loop
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
            fullscreen = !fullscreen;
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o) //If "o" is pressed
         {
            tm = localtime(&t);
            strftime(date, sizeof(date), "_%y%m%d_%H%M%S",tm);
            outFileName = fileName + string(date) + ext;
            image->Write(outFileName.c_str());
         }            
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_w) //If "w" is pressed
            {
               key_w_pressed();
               RerenderImage(image, img_data);
               }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_s) //If "s" is pressed
            {
               key_s_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_a) //If "a" is pressed
            {
               key_a_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_d) //If "d" is pressed
            {
               key_d_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_q) //If "o" is pressed
            {
               key_q_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_e)
            {
               key_e_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_j)
            {
               key_j_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_k)
            {
               key_k_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_l)
            {
               key_l_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_u)
            {
               key_u_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_i)
            {
               key_i_pressed();
               RerenderImage(image, img_data);
            }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o)
            {
               key_o_pressed();
               RerenderImage(image, img_data);
            }
         // if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT) //If "left arrow" is pressed
         //    {
         //       key_left_pressed();
         //       RerenderImage(image, img_data);
         //    }
         // if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT) //If "right arrow" is pressed
         //    {
         //       key_right_pressed();
         //       RerenderImage(image, img_data);
         //    }
         // if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP) //If "left arrow" is pressed
         //    {
         //       key_up_pressed();
         //       RerenderImage(image, img_data);
         //    }
         // if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN) //If "right arrow" is pressed
         //    {
         //       key_down_pressed();
         //       RerenderImage(image, img_data);
         //    }
         SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
         
      }     

      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
      
      // Clear the screen to grey
      glClearColor(0.6f, 0.6, 0.6f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLE_STRIP, 4, 4); //Draw the two triangles (4 vertices) making up the square
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Draw the two triangles (4 vertices) making up the square

      SDL_GL_SwapWindow(window); //Double buffering
   }

   delete [] img_data;
   delete image;
   delete camera;
   delete scene;
   glDeleteProgram(shaderProgram);
   glDeleteShader(fragmentShader);
   glDeleteShader(vertexShader);

   glDeleteBuffers(1, &vbo);

   glDeleteVertexArrays(1, &vao);
   
   //Clean Up
   SDL_GL_DeleteContext(context);
   SDL_Quit();
   // delete scene;
   return 0;
}
