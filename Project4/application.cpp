//Multi-Object, Multi-Texture Example
//Stephen J. Guy, 2021

//This example demonstrates:
// Loading multiple models (a cube and a knot)
// Using multiple textures (wood and brick)
// Instancing (the teapot is drawn in two locations)
// Continuous keyboard input - arrows (moves knot up/down/left/right continuous when being held)
// Keyboard modifiers - shift (up/down arrows move knot in/out of screen when shift is pressed)
// Single key events - pressing 'c' changes color of a random teapot
// Mixing textures and colors for models
// Phong lighting
// Binding multiple textures to one shader

const char* INSTRUCTIONS = 
"***************\n"
"This is a treasure hunt game.\n"
"\n"
"Up/down (also w/s) - Moves player forward and backward.\n"
"left/right - Turn player view to left and right.\n"
"a/d - Moves player left and right.\n"
"shift key - accelorate moving turning speed.\n"
"Moving mouse with left button down - Change player's view angle.\n"
"l - Turn on/off the flash light.\n"
"o - Turn on/off directional light.\n"
"t - Toggles between bird's eye view and first person view.\n"
"space - Jump.\n"
"c - Changes first person's color.\n"
"***************\n"
;

//Mac OS build: g++ multiObjectTest.cpp -x c glad/glad.c -g -F/Library/Frameworks -framework SDL2 -framework OpenGL -o MultiObjTest
//Linux build:  g++ application.cpp -x c glad/glad.c -g -lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/ -o app

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#include <stack>
// #include <math.h>

#include "objects.h"
#include "camera.h"
#include "scene.h"

int screenWidth0 = 800; 
int screenHeight0 = 600; 
int screenWidth = screenWidth0;
int screenHeight = screenHeight0; 
float timePast = 0;
int numTextures = 0;
#define MAX_TEXTURES 100 // no more than 100 textures for now
#define MAX_MODELS 100 // no more than 100 textures for now
#define MAX_LEVEL 8
#define NUM_TEX_FILES 5
#define NUM_MODEL_FILES 9
GLuint textures[MAX_TEXTURES] = {0}; 
//0-wood, 1-brick, 2-brick1, 3-laminate1, 4-tile1
const char* texFiles[NUM_TEX_FILES] = {"textures/wood.bmp","textures/brick.bmp","textures/brick1.bmp","textures/laminate1.bmp","textures/tile1.bmp"};
//0-cube, 1-door, 2-girl, 3-treasure chest, 4-key, 5-sphere, 6-teapot, 7-knot
const char* modelFiles[NUM_MODEL_FILES] = {"models/cube.txt","models/Mydoor1.txt","models/Female_Alternative.txt"
	,"models/Chest_Closed.txt","models/Key4.txt","models/sphere.txt"
	,"models/MyKey.txt","models/Sword.txt","models/Skeleton.txt"
};
const char* sceneFiles[MAX_LEVEL] = {"maps/no_doors_l0.txt", "maps/no_doors_l1.txt", "maps/doors_l0.txt", "maps/doors_l2.txt",
		"maps/doors_l3.txt", "maps/zombie_l0.txt", "maps/zombie_l1.txt", "maps/zombie_l2.txt"};

//global vars
int curlevel = 0;
int score = 0;
Camera* camera = new Camera();
int numModels = 0;
int totalNumVerts = 0;
int modelStartVert[MAX_MODELS] = {0};
int modelNumVerts[MAX_MODELS] = {0};
bool useFlashLight = false; //0-disable, >0-use
bool enableDir = false; //0-disable, >0-use

bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
GLuint AddTexture(int& numTextures, const char* fileName);
float* CreateModel(int& numModels, int* modelStartVert, int* modelNumVerts, int& totalNumVerts, const char* fileName);
void renderObj(int shaderProgram, Obj* obj, float* lights, int nLights);
bool fullscreen = false;
void Win2PPM(int width, int height);

//srand(time(NULL));
float rand01(){
	return rand()/(float)RAND_MAX;
}

void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts);

int main(int argc, char *argv[]){
	
	SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

	//Read command line paramaters to get map file
	std::string curMap;
	if (argc < 2){
		curMap = sceneFiles[curlevel];
	} else {
		argv += 1, argc -= 1;
		curMap = argv[0];
		argv += 1, argc -= 1;
	}

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	auto fullWidth = DM.w;
	auto fullHeight = DM.h;
	printf("width=%d, height=%d\n",fullWidth,fullHeight);

	//Ask SDL to get a recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("Project 4: Treasure Hunt", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//Load OpenGL extentions with GLAD
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}
	//load models
	float* model[NUM_MODEL_FILES];
	for (int i=0; i<NUM_MODEL_FILES; i++){
		model[i] = CreateModel(numModels, modelStartVert, modelNumVerts, totalNumVerts, modelFiles[i]);		
	}
	float* modelData = new float[totalNumVerts*8];
	float* tmp = modelData;
	for (int i=0;i<NUM_MODEL_FILES;i++){
		copy(model[i],model[i]+modelNumVerts[i]*8,tmp);
		tmp +=modelNumVerts[i]*8;
	}
	tmp = NULL;
	for (int i=0; i<NUM_MODEL_FILES;i++) {model[i]=NULL;}
	//load textures
	for (int i=0;i<NUM_TEX_FILES;i++){
		textures[numTextures] = AddTexture(numTextures, texFiles[i]);
	}
	
	//Build a Vertex Array Object (VAO) to store mapping of shader attributse to VBO
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used
	
	int texturedShader = InitShader("./shaders/textured-Vertex.shader", "./shaders/textured-Fragment.shader");	
	
	//Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(texturedShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, isNormalized, stride, offset
	glEnableVertexAttribArray(posAttrib);
	
	//GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);
	
	GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);
	
	GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	GLint uniView = glGetUniformLocation(texturedShader, "view");
	GLint uniProj = glGetUniformLocation(texturedShader, "proj");

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one	
                       
	
	glEnable(GL_DEPTH_TEST);  

	printf("%s\n",INSTRUCTIONS);

	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;

	Scene* scene = new Scene();
	scene->ReadMap(curMap.c_str());
	camera->pos = scene->me->GetPos();
	float sceneW = float((scene->nc+1)*scene->cx);
	float sceneH = float((scene->nr+1)*scene->cz);
	float far0 = (sceneW>sceneH)?1.42*sceneW:1.42*sceneH;

	int nLights = scene->lights.size();
	// printf("nLights=%d\n",nLights);
	float* lights = new float[nLights*3];
	memset(lights, 0, sizeof(float)*nLights*3);
	int idx=0;
	for (int i=0;i<nLights;i++){
		lights[idx] = scene->lights[i].x; idx++;
		lights[idx] = scene->lights[i].y; idx++;
		lights[idx] = scene->lights[i].z; idx++;
	}
	
	bool quit = false;
	bool topView = false;	
	bool mouse_dragging = false;
	float p_time = SDL_GetTicks()/1000.f;
	bool upKeyPressed = false;
	bool dnKeyPressed = false;
	bool leftKeyPressed = false;
	bool rightKeyPressed = false;
	bool spaceKeyPressed = false;
	bool aKeyPressed = false;
	bool dKeyPressed = false;
	bool reset = false;
	bool shiftKeyPressed = false;
	// glm::vec3 pos0 = camera->pos;
	
	while (!quit){
		if (scene->win){			
			curlevel++;
			score += curlevel*10;
			if (curlevel == MAX_LEVEL){
				printf("****************************\n");
				printf("*  You have beat the game! *\n");
				printf("*  Final Score: %d         *\n",score);
				printf("****************************\n");
				quit = true;
			} else {
				printf("****************************\n");
				printf("Congratulations!\n");
				printf("Moving to level %d\n",curlevel);
				printf("****************************\n");
				curMap = sceneFiles[curlevel];
			}
			reset = true;
		}
		if (scene->gameOver) reset = true;
		timePast = SDL_GetTicks()/1000.f;
		float dt = timePast - p_time;
		p_time = timePast;
		// printf("camera.pos.y=%f\n",camera->pos.y);
		while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue					
			if (windowEvent.type == SDL_QUIT) quit = true;
			//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
			//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
				quit = true; //Exit event loop
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
				fullscreen = !fullscreen;
				if (fullscreen) {
					// screenWidth = fullWidth;
					// screenHeight = fullHeight;
					// SDL_SetWindowSize(window, screenWidth, screenHeight);
					SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); //Toggle fullscreen 
					
				} else {
					// screenWidth = screenWidth0;
					// screenHeight = screenHeight0;
					// SDL_SetWindowSize(window, screenWidth, screenHeight);
					SDL_SetWindowFullscreen(window, 0); //Toggle fullscreen 
				}
			}
			// if (windowEvent.type == SDL_KEYDOWN && (windowEvent.key.keysym.mod & KMOD_SHIFT)) shiftKeyPressed = true;
			// if (windowEvent.type == SDL_KEYUP && (windowEvent.key.keysym.mod & KMOD_SHIFT)) shiftKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && (windowEvent.key.keysym.sym == SDLK_RSHIFT)) shiftKeyPressed = true;
			if (windowEvent.type == SDL_KEYDOWN && (windowEvent.key.keysym.sym == SDLK_LSHIFT)) shiftKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && (windowEvent.key.keysym.sym == SDLK_RSHIFT)) shiftKeyPressed = false;
			if (windowEvent.type == SDL_KEYUP && (windowEvent.key.keysym.sym == SDLK_LSHIFT)) shiftKeyPressed = false;
			// if (windowEvent.type == SDL_KEYUP && (windowEvent.key.keysym.mod & KMOD_SHIFT)) shiftKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP) upKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP) upKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN) dnKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN) dnKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT) leftKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT) leftKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT) rightKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT) rightKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_w) upKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_w) upKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_s) dnKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_s) dnKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_a) aKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_a) aKeyPressed = false;
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_d) dKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_d) dKeyPressed = false;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_c){ //If "c" is pressed
				scene->me->SetColor(glm::vec3(rand01(),rand01(),rand01()));
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_l) useFlashLight = !useFlashLight;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_o) enableDir = !enableDir;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE) spaceKeyPressed = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_r) reset = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_t) topView = !topView;
			if (windowEvent.type == SDL_MOUSEWHEEL)//scroll up
			{
				camera->FoV -= 100.0 * dt * (float)windowEvent.wheel.y/(float)screenHeight;
				camera->FoV = (camera->FoV<0.5)?0.1:(camera->FoV>1.3)?1.3:camera->FoV;
				// printf("FoV=%f\n",camera->FoV);
			}
		}
		if (reset){
			delete scene;
			delete lights;
			scene = new Scene();
			scene->ReadMap(curMap.c_str());
			camera->pos = scene->me->GetPos();
			sceneW = float((scene->nc+1)*scene->cx);
			sceneH = float((scene->nr+1)*scene->cz);
			far0 = (sceneW>sceneH)?1.42*sceneW:1.42*sceneH;

			nLights = scene->lights.size();
			lights = new float[nLights*3];
			memset(lights, 0, sizeof(float)*nLights*3);
			idx=0;
			for (int i=0;i<nLights;i++){
				lights[idx] = scene->lights[i].x; idx++;
				lights[idx] = scene->lights[i].y; idx++;
				lights[idx] = scene->lights[i].z; idx++;
			}
			mouse_dragging = false;
			topView = false;
			reset = false;
			enableDir = false;
			useFlashLight = false;
		}
		if (camera->pos.y>=0.001) spaceKeyPressed = false;
		if (upKeyPressed) posFwd = (shiftKeyPressed)? 2 :1;
		if (dnKeyPressed) negFwd = (shiftKeyPressed)? -2 :-1;
		if (aKeyPressed) negRight = (shiftKeyPressed)? -2 :-1;
		if (dKeyPressed) posRight = (shiftKeyPressed)? 2 :1;
		if (leftKeyPressed) hPosTurn = (shiftKeyPressed)? 2 :1;
		if (rightKeyPressed) hNegTurn = (shiftKeyPressed)? -2 :-1;
		if (spaceKeyPressed) {upforce = (shiftKeyPressed)? 250 :180; fwdspeed = 2*(posFwd+negFwd); spaceKeyPressed = false;}
		// if (glm::distance(camera->pos,pos0)>0.01 && camera->pos.y<0.001){
		// printf("pos=%f,%f,%f\n", camera->pos.x, camera->pos.y, camera->pos.z);
		// }
		// pos0=camera->pos;
		int mx, my;    
		if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) { //Is the mouse down?
			if (mouse_dragging){
			// Compute new orientation
			camera->hAngle += 0.08f * dt * float(screenWidth/2 - mx );
			camera->vAngle  += 0.08f * dt * float( screenHeight/2 - my );
			camera->vAngle = fmin( 0.9, fmax( -0.9, camera->vAngle) );
			SDL_WarpMouseInWindow(window,screenWidth/2, screenHeight/2); //reset mouse position
		} 
		mouse_dragging = true;
		} else {mouse_dragging = false;}		
		// Clear the screen to default color
		glClearColor(.2f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(texturedShader);		
		// printf("timePast=%f\n",timePast);
		camera->Update(dt);
		
		if (scene->FirstPersonUpdate(camera->pos,dt)){
			scene->me->SetPos(camera->pos);
		} else {
			glm::vec3 curPos = scene->me->GetPos();
			curPos.y = 0; // in case of jumping
			scene->me->SetPos(curPos);
			camera->pos = scene->me->GetPos();
		}
		// camera->PrintState();

		scene->me->SetRotRad(camera->hAngle);
		scene->ZomUpdate(dt);

		float FoV, far;
		glm::mat4 view;
		if (topView){
			far = far0*2+5;		
			view = glm::lookAt(
				glm::vec3(sceneW/2.0, far0 * 2, sceneH/2.0),  //Cam Position
				glm::vec3(sceneW/2.0, 0.f, sceneH/2.0),  //Look at point
				glm::vec3(0.0f, 0.0f, 1.0f)
			); //Up
			FoV = atan2(sceneH,far-15);
		} else {
			view = glm::lookAt(
				camera->pos,
				camera->pos + camera->dir,
				camera->up
			);
			FoV = camera->FoV;
			far = far0;
		}

		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 proj = glm::perspective(FoV, screenWidth / (float) screenHeight, 0.1f, far); //FOV, aspect, near, far
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

		glBindVertexArray(vao);
		if (topView){ //do not show ceiling in top view
			// scene->me->show = false;
			scene->objs[1]->SetVisibility(false);
		}else{
			// scene->me->show = true;
			scene->objs[1]->SetVisibility(true);
		}

		for (auto const& obj : scene->objs) {
			if (obj->IsVisible()){
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textures[obj->GetTexID()]);
				glUniform1i(glGetUniformLocation(texturedShader, "tex"), 0);
				renderObj(texturedShader,obj,lights,nLights);
			}
		}
		for (auto const& zom : scene->zoms) {
			if (zom->IsVisible()){
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textures[zom->GetTexID()]);
				glUniform1i(glGetUniformLocation(texturedShader, "tex"), 0);
				renderObj(texturedShader,zom,lights,nLights);
			}
		}
		if (topView) renderObj(texturedShader,scene->me,lights,nLights);

		SDL_GL_SwapWindow(window); //Double buffering
		
	}
	
	//Clean Up
	glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);
	delete scene;
	delete camera;

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

void renderObj(int shaderProgram, Obj* obj, float* lights, int nLights){
	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
	glUniform3fv(uniColor, 1, glm::value_ptr(obj->GetColor()));
	
	GLint u_nLights = glGetUniformLocation(shaderProgram, "numLights");
	glUniform1i(u_nLights, nLights);
	// printf("nLights=%d\n",nLights);
	GLint u_lights = glGetUniformLocation(shaderProgram, "lights");
	glUniform3fv(u_lights,nLights, lights);

	GLint u_usespot = glGetUniformLocation(shaderProgram, "enable_SpotLight");
	if (useFlashLight){
		glUniform1i(u_usespot,1);
		GLint u_spotpos = glGetUniformLocation(shaderProgram, "spotpos");
		// printf("camera->dir.x=%f, camera->dir.z=%f\n",camera->dir.x,camera->dir.z);
		glUniform3f(u_spotpos,camera->pos.x,camera->pos.y-1,camera->pos.z);
		// printf("y=%f\n",camera->pos.y);
		GLint u_spotdir = glGetUniformLocation(shaderProgram, "spotdir");
		glUniform3f(u_spotdir,camera->dir.x,camera->dir.y - fabs(camera->dir.y)*0.2,camera->dir.z);
	} else {
		glUniform1i(u_usespot,0);
	}
	GLint u_enableDir = glGetUniformLocation(shaderProgram, "enable_dirLight");
	if (enableDir) glUniform1i(u_enableDir,1);
	else glUniform1i(u_enableDir,0);
      
  	GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");

	//Translate the model (matrix) left and back
	glm::mat4 model = glm::mat4(1); //Load intentity
	model = glm::translate(model,obj->GetPos());	
	model = glm::scale(model,obj->GetScale());
	model = glm::rotate(model,obj->GetRotRad(),obj->GetRotAxis());
	
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	//Set which texture to use (0 = wood texture ... bound to GL_TEXTURE0)
	glUniform1i(uniTexID, obj->GetTexID());

  //Draw an instance of the model (at the position & orientation specified by the model matrix above)
	glDrawArrays(GL_TRIANGLES, modelStartVert[obj->GetModelID()], modelNumVerts[obj->GetModelID()]); //(Primitive Type, Start Vertex, Num Verticies)
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders
	
	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	
	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	
	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);

	return program;
}

GLuint AddTexture(int& numTextures, const char* fileName){
	//// Allocate Texture 0 (Wood) ///////
	SDL_Surface* surface = SDL_LoadBMP(fileName);
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex;
	numTextures++;
	if (numTextures>MAX_TEXTURES-1){
		printf("Error: Number of Textures exceed preset limit of %d\n", MAX_TEXTURES);
		exit(1);
	} //throw error is number of textures exceed maximum
    glGenTextures(1, &tex);	
	GLuint temp = numTextures-1;
    glActiveTexture(GL_TEXTURE0+temp);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface);
	return tex;
}

float* CreateModel(int& numModels, int* modelStartVert, int* modelNumVerts, int& totalNumVerts, const char* fileName){
	if (numModels>MAX_MODELS-1){
		printf("Error: Number of Models exceed preset limit of %d\n", MAX_MODELS);
		exit(1);
	} //throw error is number of models exceed maximum
	ifstream modelFile;
	modelFile.open(fileName);
	int numLines = 0;
	modelFile >> numLines;
	float* model = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model[i];
	}
	// printf("Adding Model#%d, numlines = %d\n",numModels, numLines);
	modelStartVert[numModels] = totalNumVerts;
	modelNumVerts[numModels] = numLines/8;
	totalNumVerts += modelNumVerts[numModels];
	modelFile.close();
	numModels++;
	return model;
}