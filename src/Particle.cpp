//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start) :
	charge(1.0f),
	m(1.0f),
	d(0.0f),
	x(start.x, start.y, start.z),
	v(0.0f, 0.0f, 0.0f),
	lifespan(2.0f),
	tEnd(0.0f),
	scale(10.0f),
	color(.245f * (((rand() % 8) + 2) / 5),
		.245f * (((rand() % 8) + 2) / 5),
		.666f * (((rand() % 8) + 2) / 5),
		1.0f)
{
}

Particle::~Particle()
{
}

void Particle::load(vec3 start, vec3 ballV)
{
	// Random initialization
	rebirth(0.0f, start, ballV);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start, vec3 ballV)
{
	float vMult = 70.0f;
	charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;	
	m = 1.0f;
  	d = randFloat(0.0f, 0.02f);
	x = vec3(start.x, start.y, start.z);
	v.x = randFloat(0.2f, 0.6f) * ballV.x * vMult;
	v.y = randFloat(0.5f, 1.0f) * -ballV.y * vMult;
	v.z = randFloat(0.2f, 0.6f) * ballV.z * vMult;
	vec3 rotVec = vec3(ballV.x, -ballV.y, ballV.z);
	mat4 rotMat(1);
	rotMat = rotate(rotMat, radians(randFloat(-40.0f, 40.0f)), vec3(0, 1, 0));
	v = vec3(rotMat * vec4(v, 1.0f));
	lifespan = 1.0f;
	tEnd = t + lifespan;
	scale = randFloat(1.0f, 1.0f);
	float whiteVal = randFloat(0.20f, 0.80f);
   	color.r = whiteVal;
   	color.g = whiteVal;
   	color.b = randFloat(0.93f, 0.99f);
	color.a = randFloat(0.3f, 1.0f);
}

void Particle::update(float t, float h, const vec3 &g, const vec3 start)
{
	if(t > tEnd) {
		color.a = 0.0f;
	}

	//very simple update
	x += h * v;
	v += h * g * 10.0f;
	
}
