#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 EPos;

void main()
{
	//vec3 normal = normalize(fragNor);
	//vec3 light = normalize(lightDir);
	//float dC = max(0, dot(normal, light));
	//color = vec4(MatAmb + dC*MatDif, 1.0);

	//ambient
	vec3 ambient = MatAmb * 1.0;
	//diffuse
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 diffuse = MatDif * max(0,dot(normal,light));
	//specular
	
	vec3 viewDir = normalize(EPos); //vector from fragment's position vector to the position of the camera
	vec3 H = normalize(lightDir + viewDir);
	vec3 specular = MatSpec * pow(max(dot(H,normal), 0.0), MatShine);
	
	color = vec4((ambient + diffuse + specular), 1.0);

}
