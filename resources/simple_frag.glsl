#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform float MatShine;
uniform vec3 MatSpec;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 fragPos;

in vec3 viewFrag;
void main()
{
	//ambient
	vec3 ambient = MatAmb * 1.0;
	//diffuse
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 diffuse = MatDif * max(0,dot(normal,light));
	//specular
	
	vec3 viewDir = normalize(viewFrag - fragPos);
	vec3 H = normalize(lightDir + viewDir);
	vec3 specular = MatSpec * pow(max(dot(H,normal), 0.0), MatShine);;
	
	color = vec4(ambient + diffuse + specular, 1.0);
}
