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

uniform vec3 splashForce1;
uniform vec3 splashForce2;
uniform vec3 splashForce3;
uniform vec3 splashForce4;
uniform vec3 splashForce5;


uniform vec3 ballV;
uniform float time;

uniform int flip;

out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDir;
out vec3 EPos;

#define pi 3.1415926535897932384626433832795

void main() {

  vec4 vPosition;
  float lifespan = 4.0f;
  float remaining1 = max(0.0f, lifespan - (time - splashPosition1.a));
  float remaining2 = max(0.0f, lifespan - (time - splashPosition2.a));
  float remaining3 = max(0.0f, lifespan - (time - splashPosition3.a));
  float remaining4 = max(0.0f, lifespan - (time - splashPosition4.a));
  float remaining5 = max(0.0f, lifespan - (time - splashPosition5.a));
  
  /* First model transforms */
  

  fragNor = flip * (M * vec4(vertNor, 0.0)).xyz;
  lightDir = vec3((vec4(lightPos - (M*vec4(vertPos,1.0)).xyz, 0.0)));
  
  EPos = (M * vec4(vertPos.x,vertPos.y, vertPos.z ,1.0)).xyz;
  float distance1 = distance(EPos, splashPosition1.xyz);
  float distance2 = distance(EPos, splashPosition2.xyz);
  float distance3 = distance(EPos, splashPosition3.xyz);
  float distance4 = distance(EPos, splashPosition4.xyz);
  float distance5 = distance(EPos, splashPosition5.xyz);

  float propagationAdjust1 = 0.7/distance1 * sin(pi * remaining1/lifespan);
  float propagationAdjust2 = 0.7/distance2 * sin(pi * remaining2/lifespan);
  float propagationAdjust3 = 0.7/distance3 * sin(pi * remaining3/lifespan);
  float propagationAdjust4 = 0.7/distance4 * sin(pi * remaining4/lifespan);
  float propagationAdjust5 = 0.7/distance5 * sin(pi * remaining5/lifespan);
  
  float wavelength = 4.0f;
  float offsetY1 = length(splashForce1) * sin(wavelength * distance1 * remaining1) * propagationAdjust1;
  float offsetY2 = length(splashForce2) * sin(wavelength * distance2 * remaining2) * propagationAdjust2;
  float offsetY3 = length(splashForce3) * sin(wavelength * distance3 * remaining3) * propagationAdjust3;
  float offsetY4 = length(splashForce4) * sin(wavelength * distance4 * remaining4) * propagationAdjust4;
  float offsetY5 = length(splashForce5) * sin(wavelength * distance5 * remaining5) * propagationAdjust5;
  
  gl_Position = P * V *M * vec4(vertPos.x, vertPos.y - (offsetY1 + offsetY2 + offsetY3 + offsetY4 + offsetY5), vertPos.z, 1.0);
  EPos = (M * vec4(vertPos.x,vertPos.y - (offsetY1 + offsetY2 + offsetY3 + offsetY4 + offsetY5), vertPos.z ,1.0)).xyz;
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
}
