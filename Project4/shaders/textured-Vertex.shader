#version 150 core

in vec3 position;
//in vec3 inColor;

//const vec3 inColor = vec3(0.f,0.7f,0.f);
// const vec3 inLightPos = normalize(vec3(10,8,5));
in vec3 inNormal;
in vec2 inTexcoord;

out vec3 Color;
out vec3 vertNormal;
out vec3 pos;
out vec3 lightPos;
out vec2 texcoord;
out mat4 view1;
out mat4 model1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 inColor;

void main() {
   Color = inColor;
   gl_Position = proj * view * model * vec4(position,1.0);
   pos = (view * model * vec4(position,1.0)).xyz;
   // lightPos = (view * vec4(inLightPos,0.0)).xyz; //It's a vector!
   // lightPos = inLightPos;
   vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal,0.0);
   vertNormal = normalize(norm4.xyz);
   vec2 temp = vec2(model[0][0], model[2][2]);
   if (abs(model[0][0])<1.1) {temp = vec2(model[1][1],model[2][2]);}
   else if (abs(model[2][2])<1.1) {temp = vec2(model[0][0],model[1][1]);}
   texcoord = temp*inTexcoord;
   // texcoord = inTexcoord;
   view1 = view;
   model1 = model;
}