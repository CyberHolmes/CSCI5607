#version 150 core

in vec3 Color;
in vec3 vertNormal;
in vec3 pos;
in vec3 lightPos;
in vec2 texcoord;
in mat4 view1;
in mat4 model1;

out vec4 outColor;

uniform sampler2D tex;

uniform int texID;
uniform int numLights;
uniform vec3 lights[20]; //no more than 20 lights are supported
uniform int enable_SpotLight;
uniform vec3 spotdir;
uniform vec3 spotpos;

const float a1 = 0.9; //spot light angle limits
const float a2 = 0.85;

const float ambient = .1;
void main() {
  vec3 color;
  if (texID == -1)
    color = Color;
  else
    color = texture(tex, texcoord).rgb* Color; ;

  vec3 normal = vertNormal;
  vec3 oColor = vec3(0,0,0);
  for (int i=0; i<numLights; i++){
    vec3 lpos = (view1 *  vec4(lights[i],1.0)).xyz;
    vec3 ldir = normalize(lpos - pos);
    float dist = distance(pos, lpos);
    float attenuation = 1.0/dist;//(dist*dist);
    vec3 diffuseC = color*max(dot(ldir,normal),0.0);
    vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,-ldir),0.0);
    if (dot(ldir,normal) <= 0.0) spec = 0;
    vec3 specC = color*pow(spec,4);
    oColor = oColor + (diffuseC+specC) * attenuation;
    // oColor = specC;
  }
  if (enable_SpotLight>0){
    vec3 spotpos1 = (view1 *  vec4(spotpos,1.0)).xyz;
    vec3 spotdir1 = normalize((view1 *  vec4(spotdir,0.0)).xyz);
    vec3 s_ldir = normalize(spotpos1 - pos);
    float angle = dot(-s_ldir,spotdir1);
    if (angle>a2){
      float term = (angle<a1)?(1.0-(a1 - angle)/(a1 - a2)) : 1.0;
      float dist = distance(pos, spotpos1);
      float attenuation = 4.0/dist;//(dist*dist);
      vec3 diffuseC = color*max(dot(-spotdir1,normal),0.0);
      // vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
      // vec3 reflectDir = reflect(viewDir,normal);
      // float spec = max(dot(reflectDir,-spotdir1),0.0);
      // if (dot(spotdir1,normal) <= 0.0) spec = 0;
      // vec3 specC = color*pow(spec,4);
      // oColor = oColor + (diffuseC+specC) * attenuation*term;
      oColor += diffuseC* attenuation*term;//+color*ambient;
      // oColor = specC;
      // oColor = vec3(attenuation,0,0);
    }//else{
  }
  oColor = oColor + color*ambient;
  outColor = vec4(oColor,1);
}