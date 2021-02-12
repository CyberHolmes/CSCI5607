//Project 1 - Moving Square
//This code assumes SDL2 and OpenGL are both properly installed on your system

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include "pga.h"
#include "geom_lib_2d.h"
using namespace std;


//Name of image texture
string textureName = "goldy.ppm";

//Screen size
int screen_width = 800;
int screen_height = 800;

//Globals to store the state of the square (position, width, and angle)
Point2D rect_pos = Point2D(0,0);
float rect_scale = 1;
float rect_angle = 0;

float vertices[] = {  //The function updateVertices() changes these values to match p1,p2,p3,p4
//  X     Y     R     G     B     U    V
  0.3f,  0.3f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
  0.3f, -0.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  -0.3f,  0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
  -0.3f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
// second object
  0.9f,  0.9f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
  0.9f, 0.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  0.3f,  0.9f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
  0.3f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
// 3rd object
  0.9f, -0.3f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
  0.9f, -0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  0.3f, -0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
  0.3f, -0.9f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
// 4th object
  -0.3f, -0.3f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
  -0.3f, -0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  -0.9f, -0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
  -0.9f, -0.9f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
// 5th object
  -0.3f, 0.9f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
  -0.3f, 0.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  -0.9f, 0.9f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
  -0.9f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
};

Point2D origin = Point2D(0,0);

//Watch winding...
// Point2D init_p1 = Point2D(vertices[21],vertices[22]);
// Point2D init_p2 = Point2D(vertices[7],vertices[8]);
// Point2D init_p3 = Point2D(vertices[0],vertices[1]);
// Point2D init_p4 = Point2D(vertices[14],vertices[15]);
// Point2D init_p3 = Point2D(vertices[21],vertices[22]);
// Point2D init_p4 = Point2D(vertices[7],vertices[8]);
// Point2D init_p1 = Point2D(vertices[0],vertices[1]);
// Point2D init_p2 = Point2D(vertices[14],vertices[15]);

Point2D init_p4 = Point2D(vertices[21],vertices[22]);
Point2D init_p3 = Point2D(vertices[7],vertices[8]);
Point2D init_p2 = Point2D(vertices[0],vertices[1]);
Point2D init_p1 = Point2D(vertices[14],vertices[15]);

Point2D p1 = init_p1, p2 = init_p2, p3 = init_p3, p4 = init_p4;

//Build lines along the edges or the rectange
//This is helpful for later geometric calculations
Line2D l1 = vee(p1,p2).normalized();
Line2D l2 = vee(p2,p3).normalized();
Line2D l3 = vee(p3,p4).normalized();
Line2D l4 = vee(p4,p1).normalized();
Line2D l5 = vee(p2,p4).normalized();


//Helper variables to track t
Point2D clicked_pos;
Point2D clicked_mouse;
float clicked_angle, clicked_size;

void mouseClicked(float mx, float my); //Called when mouse is pressed down
void mouseDragged(float mx, float my); //Called each time the mouse is moved during click
void updateVertices();

bool do_translate = false;
bool do_rotate = false;
bool do_scale = false;

//////////////////////////
///  Begin your code here
/////////////////////////

unsigned int numVertices = 4; //control number of vertices of the object
void animate(float dt);
Dir2D d_disp = Dir2D(0.1,0.1);
float g_size = 1.05;
float d_angle = 0.6;
bool animateEnable = false;
float dt = 0.015; //control how fast animation goes
bool showMulti = false;
bool brighten = false;

//Done: Read from ASCII (P3) PPM files
//Inputs are output variables for returning the image width and heigth
unsigned char* loadImage(int& img_w, int& img_h){

   //Open the texture image file
   ifstream ppmFile;
   ppmFile.open(textureName.c_str());
   if (!ppmFile){
      printf("ERROR: Texture file '%s' not found.\n",textureName.c_str());
      exit(1);
   }

   //Check that this is an ASCII PPM (first line is P3)
   string PPM_style;
   ppmFile >> PPM_style; //Read the first line of the header    
   if (PPM_style != "P3") {
      printf("ERROR: PPM Type number is %s. Not an ASCII (P3) PPM file!\n",PPM_style.c_str());
      exit(1);
   }

   //Read in the texture width and height
   ppmFile >> img_w >> img_h;
   unsigned char* img_data = new unsigned char[4*img_w*img_h];

   //Check that the 3rd line is 255 (ie., this is an 8 bit/pixel PPM)
   int maximum;
   ppmFile >> maximum;
   if (maximum != 255) {
      printf("ERROR: Maximum size is (%d) not 255.\n",maximum);
      exit(1);
   }

   //Done: This loop puts in fake data, replace with the actual pixels read from the file
   //      ... see project description for a hint on how to do this

   int red, green, blue, pixelNum=0;
   for (int i = 0; i < img_h; i++){
      for (int j = 0; j < img_w; j++){
         ppmFile>>red>>green>>blue;
         pixelNum++;
         img_data[i*img_w*4 + j*4] = red;  //Red
         img_data[i*img_w*4 + j*4 + 1] = green;  //Green
         img_data[i*img_w*4 + j*4 + 2] = blue;  //Blue
         img_data[i*img_w*4 + j*4 + 3] = 255;  //Alpha
      }
   }

   return img_data;
}

void mouseClicked(float m_x, float m_y){   
   printf("Clicked at %f, %f\n",m_x,m_y);

   clicked_mouse = Point2D(m_x,m_y);
   clicked_pos = rect_pos;
   clicked_angle = rect_angle;
   clicked_size = rect_scale;
   
   do_translate = false;
   do_rotate = false;
   do_scale = false;
   float d = 0.02; //in range radius   

   if (numVertices == 4)
   {
      if ((pointTriangleCornerDist(clicked_mouse,p1,p2,p3)<d) || dist(clicked_mouse,p4)<d)
      {
         do_scale=true;      
      }
      else if (pointSegmentDistance(clicked_mouse,p1,p2)<d || pointSegmentDistance(clicked_mouse,p2,p3)<d || pointSegmentDistance(clicked_mouse,p3,p4)<d || pointSegmentDistance(clicked_mouse,p4,p1)<d)
      {
         do_rotate = true;      
      }
      else if (pointInTriangle(clicked_mouse,p1,p2,p3) || pointInTriangle(clicked_mouse,p4,p1,p3))
      {
         do_translate=true;      
      } 
   }
   else if (numVertices == 3)
   {
      if (pointTriangleCornerDist(clicked_mouse,p3,p4,p2)<d)
      {
         do_scale=true;      
      }
      else if (pointSegmentDistance(clicked_mouse,p3,p4)<d || pointSegmentDistance(clicked_mouse,p4,p2)<d || pointSegmentDistance(clicked_mouse,p2,p3)<d)
      {
         do_rotate = true;      
      }
      else if (pointInTriangle(clicked_mouse,p2,p4,p3))
      {
         do_translate=true;      
      } 
   }
}   

void mouseDragged(float m_x, float m_y){
   Point2D cur_mouse = Point2D(m_x,m_y);      
   
   if (do_translate){
      Dir2D disp = cur_mouse-clicked_mouse;
      rect_pos = clicked_pos+disp;      
   }
   
   if (do_scale){
      //Compute the new size, g_size, based on the mouse positions
      float g_size = (cur_mouse-clicked_pos).magnitude()/(clicked_mouse-clicked_pos).magnitude();       
      rect_scale = clicked_size*g_size;      
   }
   
   if (do_rotate){
      //Compute the new angle, rect_angle, based on the mouse positions
      Line2D l1 = join(clicked_pos,clicked_mouse);
      Line2D l2 = join(clicked_pos,cur_mouse);
      float d_angle = angle(l1,l2);
      rect_angle = clicked_angle-sign(vee(cur_mouse,l1))*angle(l1,l2);      
   }

   //Assuming the angle (rect_angle), position (rect_pos), and scale (rect_scale) of the rectangle
   //  have all been set above, the following code should rotate, shift and scale the shape correctly.
   //It's still good to read through and make sure you understand how this works!

   Motor2D translate, rotate;

   Dir2D disp = rect_pos-origin;
   translate = Translator2D(disp);
   rotate = Rotator2D(rect_angle, rect_pos);

   Motor2D movement = rotate*translate;

   //Scale points
   p1 = init_p1.scale(rect_scale);
   p2 = init_p2.scale(rect_scale);
   p3 = init_p3.scale(rect_scale);
   p4 = init_p4.scale(rect_scale);

   //Use Motor to translate and rotate points
   p1 = transform(p1,movement);
   p2 = transform(p2,movement);
   p3 = transform(p3,movement);
   p4 = transform(p4,movement);

   //Update lines based on new points
   if (numVertices==4)
   {
      l1 = vee(p1,p2).normalized();
      l2 = vee(p2,p3).normalized();
      l3 = vee(p3,p4).normalized();
      l4 = vee(p4,p1).normalized();
   } else {
      //l1 = vee(p1,p2).normalized();
      l2 = vee(p2,p3).normalized();
      l3 = vee(p3,p4).normalized();
      //l4 = vee(p4,p1).normalized();
      l5 = vee(p2,p4).normalized();
   } 
   updateVertices();
}

void animate(float dt){

   srand(time(NULL));
   rect_pos.x += d_disp.x*dt;
   rect_pos.y += d_disp.y*dt;
   float cur_rect_size = rect_scale*0.3; // current rectangle size
   float b1 = rect_scale*0.3-1; // left or top boundary
   float b2 = -rect_scale*0.3+1; // right or bottom boundary
   if (rect_pos.x<b1)
   {
      rect_pos.x=b1;
      d_disp.x=-d_disp.x;
      d_angle = -d_angle;
   }      
   if (rect_pos.x>b2)
   {
      rect_pos.x=b2;
      d_disp.x=-d_disp.x;
      d_angle = -d_angle;
   }
   if (rect_pos.y<b1)
   {
      rect_pos.y=b1;
      d_disp.y=-d_disp.y;
      d_angle = -d_angle;
   } 
   if (rect_pos.y>b2)
   {
      rect_pos.y=b2;
      d_disp.y=-d_disp.y;
      d_angle = -d_angle;
   } 

   rect_scale *= 1-(1-g_size)*dt;
   if (rect_scale>1.5)
      g_size = 0.9;
   if (rect_scale<0.8)
      g_size = 1.1; 
 
   rect_angle -= d_angle*dt;      

   Motor2D translate, rotate;

   Dir2D disp = rect_pos-origin;
   translate = Translator2D(disp);
   rotate = Rotator2D(rect_angle, rect_pos);

   Motor2D movement = rotate*translate;

   //Scale points
   p1 = init_p1.scale(rect_scale);
   p2 = init_p2.scale(rect_scale);
   p3 = init_p3.scale(rect_scale);
   p4 = init_p4.scale(rect_scale);

   //Use Motor to translate and rotate points
   p1 = transform(p1,movement);
   p2 = transform(p2,movement);
   p3 = transform(p3,movement);
   p4 = transform(p4,movement);

   //Update lines based on new points
   if (numVertices==4)
   {
      l1 = vee(p1,p2).normalized();
      l2 = vee(p2,p3).normalized();
      l3 = vee(p3,p4).normalized();
      l4 = vee(p4,p1).normalized();
   } else {
      //l1 = vee(p1,p2).normalized();
      l2 = vee(p2,p3).normalized();
      l3 = vee(p3,p4).normalized();
      //l4 = vee(p4,p1).normalized();
      l5 = vee(p2,p4).normalized();
   }
   updateVertices();
}

//You shouldn't have to edit this, it updates the displayed verticies to match the computed p1, p2, p3, p4
void updateVertices(){ 
   vertices[0] = p3.x;  //Top right x
   vertices[1] = p3.y;  //Top right y
   
   vertices[7] = p2.x;  //Bottom right x
   vertices[8] = p2.y;  //Bottom right y
   
   vertices[14] = p4.x;  //Top left x
   vertices[15] = p4.y;  //Top left y
   
   vertices[21] =  p1.x;  //Bottom left x
   vertices[22] =  p1.y;  //Bottom left y
}

//Resest the square's position, orientation, and scale
void r_keyPressed(){
   cout << "The 'r' key was pressed" <<endl;
   p1 = init_p1;
   p2 = init_p2;
   p3 = init_p3;
   p4 = init_p4;
   updateVertices();
   rect_pos = Point2D(0,0);
   rect_scale = 1;
   rect_angle = 0.0;
   glDisable(GL_FRAMEBUFFER_SRGB);
   numVertices = 4;
   animateEnable = false;
}

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
   "   outColor = texture(tex0, texcoord).rgb;"
   "}";
    
bool fullscreen = false;

float mouse_dragging = false;
int main(int argc, char *argv[]){
   
   SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
   
   //Ask SDL to get a fairly recent version of OpenGL (3.2 or greater)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("Hailin's Project 1", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
	
	//The above window cannot be resized which makes some code slightly easier.
	//Below show how to make a full screen window or allow resizing
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen

   float aspect = screen_width/(float)screen_height; //aspect ratio (needs to be updated if the window is resized)
	
	updateVertices(); //set initial position of the square to match it's state
	
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


   int img_w, img_h;
   unsigned char* img_data = loadImage(img_w,img_h);
   printf("Loaded Image of size (%d,%d)\n",img_w,img_h);
   //memset(img_data,0,4*img_w*img_h); //Load all zeros
   //Load the texture into memory
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
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
   bool gammaOn = false;   

   while (!done){
      if (animateEnable)
         animate(dt);
      while (SDL_PollEvent(&windowEvent)){  //Process input events (e.g., mouse & keyboard)
         if (windowEvent.type == SDL_QUIT) done = true;
         //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
         //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
            done = true; //Exit event loop
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
            fullscreen = !fullscreen;
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_r) //If "r" is pressed
            r_keyPressed();
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_b) //If "b" is pressed
         {
            brighten = !brighten;
            if (brighten)
            {               
               for (int i=0; i<4*img_w*img_h; i++){
                  float tmp = (float)img_data[i]*1.5;
                  img_data[i] = (unsigned char)(tmp>255?255:tmp);
               }               
            } else
            {
               img_data = loadImage(img_w,img_h);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
            glGenerateMipmap(GL_TEXTURE_2D); 
         }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT) //If "left arrow" is pressed
            numVertices = 3;
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT) //If "right arrow" is pressed
            numVertices = 4;
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE) //If "space" is pressed
         {
            animateEnable = !animateEnable;
            if (animateEnable)
            {
               srand(time(NULL));
               d_disp = Dir2D((float)(rand()%100)/100,(float)(rand()%100)/100);
               d_angle = (float)(rand()%30+80)/100;               
            }
         }
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_m) //If "m" is pressed
            showMulti = !showMulti;
            
         SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
      }
      
      
      int mx, my;
      
      if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) { //Is the mouse down?
         if (mouse_dragging == false){
            mouseClicked(2*mx/(float)screen_width - 1, 1-2*my/(float)screen_height);
         } 
         else{
            mouseDragged(2*mx/(float)screen_width-1, 1-2*my/(float)screen_height);
         }
         mouse_dragging = true;
      } 
      else{
         mouse_dragging = false;
      }
      
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
      
      // Clear the screen to grey
      glClearColor(0.6f, 0.6, 0.6f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      if (showMulti)
      {
         glDrawArrays(GL_TRIANGLE_STRIP, 4, numVertices); //Draw the two triangles (4 vertices) making up the square
         glDrawArrays(GL_TRIANGLE_STRIP, 8, numVertices); //Draw the two triangles (4 vertices) making up the square
         glDrawArrays(GL_TRIANGLE_STRIP, 12, numVertices); //Draw the two triangles (4 vertices) making up the square
         glDrawArrays(GL_TRIANGLE_STRIP, 16, numVertices); //Draw the two triangles (4 vertices) making up the square
      }
      glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices); //Draw the two triangles (4 vertices) making up the square

      SDL_GL_SwapWindow(window); //Double buffering
   }

   delete [] img_data;
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
