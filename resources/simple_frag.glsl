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
in vec3 EPos;

void main()
{
	//you will need to work with these for lighting
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 ambient = MatAmb * 1.0;
	vec3 H = normalize(normalize(EPos) + light);
	vec3 specular = MatSpec * pow(max(0, dot(fragNor,H)),MatShine);
	vec3 diffuse = 0.1 * MatDif * max(0,dot(fragNor,lightDir));
	color = vec4(ambient + diffuse + specular, 1.0);
}
