#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

uniform vec3 lightPos;
uniform vec4 splashPosition1;
uniform vec4 splashPosition2;
uniform vec4 splashPosition3;
uniform vec4 splashPosition4;
uniform vec4 splashPosition5;
uniform float time;

uniform int flip;

out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDir;
out vec3 EPos;

void main() {

  vec4 vPosition;
  float lifespan = 3.0f;
  float remaining1 = max(0.0f, lifespan - (time - splashPosition1.a));
  float remaining2 = max(0.0f, lifespan - (time - splashPosition2.a));
  float remaining3 = max(0.0f, lifespan - (time - splashPosition3.a));
  float remaining4 = max(0.0f, lifespan - (time - splashPosition4.a));
  float remaining5 = max(0.0f, lifespan - (time - splashPosition5.a));
  
  /* First model transforms */
  

  fragNor = flip * (M * vec4(vertNor, 0.0)).xyz;
  lightDir = vec3((vec4(lightPos - (M*vec4(vertPos,1.0)).xyz, 0.0)));
  

  float distance1 = distance(EPos, splashPosition1.xyz);
  float distance2 = distance(EPos, splashPosition2.xyz);
  float distance3 = distance(EPos, splashPosition3.xyz);
  float distance4 = distance(EPos, splashPosition4.xyz);
  float distance5 = distance(EPos, splashPosition5.xyz);

  float offsetY = (
    sin(distance1) * remaining1
  + sin(distance2) * remaining2 
  + sin(distance3) * remaining3
  + sin(distance4) * remaining4 
  + sin(distance5) * remaining5);

  gl_Position = P * V *M * vec4(vertPos.x, vertPos.y + offsetY, vertPos.z, 1.0);
  EPos = (M * vec4(vertPos.x,vertPos.y + offsetY, vertPos.z ,1.0)).xyz;
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
}
