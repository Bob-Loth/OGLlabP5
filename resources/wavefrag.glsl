#version 330 core
uniform sampler2D Texture0;

in vec2 vTexCoord;
in vec3 lightDir;
in vec3 fragNor;
in vec3 EPos;

out vec4 Outcolor;

uniform float alpha;

void main() {
  vec4 texColor0 = texture(Texture0, vTexCoord);
    //alpha
    vec3 ambient = texColor0.xyz;

    //diffuse
    vec3 normal = normalize(fragNor);
    vec3 light = normalize(lightDir);
    vec3 diffuse = texColor0.xyz * max(0,dot(normal,light));
    //specular
    vec3 viewDir = normalize(EPos);
    vec3 H = normalize(normalize(lightDir) + normalize(viewDir));
    vec3 specular = texColor0.xyz * pow(max(dot(H,normal),0.0),4);

  	//to set the out color as the texture color 
  	Outcolor = vec4(
    (0.1 * ambient) + 
    (1.0 * diffuse) + 
    (0.5 * specular),alpha);
  
  	//to set the outcolor as the texture coordinate (for debugging)
	//Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

