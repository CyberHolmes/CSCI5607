#version 150 core

in vec3 Color;
in vec3 vertNormal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

uniform int texID;

uniform vec3 lights[3];
uniform int numLights;

const float ambient = .3;
void main() {
  vec3 color;
  if (texID == -1)
    color = Color;
  else if (texID == 0)
    color = texture(tex0, texcoord).rgb;
  else if (texID == 1)
    color = texture(tex1, texcoord).rgb; 
  else if (texID == 2)
    color = texture(tex2, texcoord).rgb;
  else if (texID == 3)
    color = texture(tex3, texcoord).rgb;
  else if (texID == 4)
    color = texture(tex4, texcoord).rgb;  
  else{
    outColor = vec4(1,0,0,1);
    return; //This was an error, stop lighting!
  }
  vec3 oColor = vec3(0,0,0);
  for (int i=0; i<3; i++){
    vec3 lightDir = normalize(lights[i] - pos);
    float dist = distance(pos, lights[i]);
    float attenuation = 20.0/(dist*dist);
    vec3 diffuseC = color*max(dot(lightDir,normal),0.0);
    vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,-lightDir),0.0);
    if (dot(lightDir,normal) <= 0.0) spec = 0;
    vec3 specC = vec3(1.0,1.0,1.0)*pow(spec,4);
    oColor = oColor + diffuseC+specC;
  }

  oColor = oColor + color*ambient;
  outColor = vec4(oColor,1);
}