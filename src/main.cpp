/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

void resize_obj(std::vector<tinyobj::shape_t>& shapes) {
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float scaleX, scaleY, scaleZ;
	float shiftX, shiftY, shiftZ;
	float epsilon = 0.001f;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	//Go through all vertices to determine min and max of each dimension
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
			if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

			if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
			if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

			if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
			if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
		}
	}

	//From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent) {
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent) {
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent) {
		maxExtent = zExtent;
	}
	scaleX = 2.0f / maxExtent;
	shiftX = minX + (xExtent / 2.0f);
	scaleY = 2.0f / maxExtent;
	shiftY = minY + (yExtent / 2.0f);
	scaleZ = 2.0f / maxExtent;
	shiftZ = minZ + (zExtent) / 2.0f;

	//Go through all verticies shift and scale them
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
			assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
			assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
			assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
		}
	}
}


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program - use this one for Blinn-Phong
	std::shared_ptr<Program> prog;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	//our geometry
	shared_ptr<Shape> goal;
	shared_ptr<vector<Shape>> pool = make_shared<vector<Shape>>();
	vector<vec3> poolBBox;
	shared_ptr<Shape> sphere;
	shared_ptr<vector<Shape>> dummy = make_shared<vector<Shape>>();
	vector<vec3> dummyBBox;
	shared_ptr<Shape> ball;

	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;

	//the image to use as a texture (ground)
	shared_ptr<Texture> texture0;

	//global data (larger program should be encapsulated)
	vec3 gMin;
	float gRot = 0;
	float gTilt = 0;
	float gZoom = 0;
	float gCamH = 0;
	vec3 goalTrans = vec3(0, 1.1, -9.5);
	vec3 shooterTrans = vec3(0, .27, -7.3);
	//animation data
	float lightTrans = 0;
	float gTrans = -3;
	float sTheta = 0;
	float eTheta = 0;
	float hTheta = 0;
	shared_ptr<MatrixStack> rHandAnchor;
	float animSpeed = 2;
	int numThrows = 0;
	float goalieTime = 0;
	float shooterRot = 0;
	bool goalieColor = true;
	double glTime = 0;
	//where the ball starts from on the z axis
	float currX = 0.0f, currY = 0.0f, currZ = 0.0f;
	float z0 = 9.0;
	vec3 CameraPos;
	vec3 getCenterOfBBox(Shape s) {
		return vec3(
			(s.max.x + s.min.x) / 2,
			(s.max.y + s.min.y) / 2,
			(s.max.z + s.min.z) / 2
		);
	}

	vector<vec3> getMultiShapeBBox(shared_ptr<vector<Shape>> shapes) {
		vector<vec3> BBox;
		BBox.push_back(vec3(FLT_MAX, FLT_MAX, FLT_MAX));
		BBox.push_back(vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		
		for (size_t i = 0; i < shapes->size(); ++i) {
			//x
			if (shapes->at(i).min.x < BBox.at(0).x) BBox.at(0).x = shapes->at(i).min.x;
			if (shapes->at(i).max.x > BBox.at(1).x) BBox.at(1).x = shapes->at(i).min.x;
			//y
			if (shapes->at(i).min.y < BBox.at(0).y) BBox.at(0).y = shapes->at(i).min.y;
			if (shapes->at(i).max.y > BBox.at(1).y) BBox.at(1).y = shapes->at(i).max.y;
			//z
			if (shapes->at(i).min.z < BBox.at(0).z) BBox.at(0).z = shapes->at(i).min.z;
			if (shapes->at(i).max.z > BBox.at(1).z) BBox.at(1).z = shapes->at(i).max.z;
		}
		return BBox;
	}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		//update global camera rotate
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			gTilt -= 0.2f;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			gTilt += 0.2f;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			gRot -= 0.2f;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			gRot += 0.2f;
		}
		//update camera height
		if (key == GLFW_KEY_S && action == GLFW_PRESS){
			gCamH  += 0.25f;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS){
			gCamH  -= 0.25f;
		}
		//move light
		if (key == GLFW_KEY_Q && action == GLFW_PRESS){
			lightTrans += 1.f;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS){
			lightTrans -= 1.f;
		}
		//change color of goalie
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			goalieColor = false;
		}
		if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
			goalieColor = true;
		}
		//wireframe
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		//move camera back/forth
		if (key == GLFW_KEY_G && action == GLFW_PRESS) {
			gZoom = gZoom + 1;
		}
		if (key == GLFW_KEY_H && action == GLFW_PRESS) {
			gZoom = fmin(0,gZoom - 1);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
		
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		//allows for translucent water
		glEnable(GL_BLEND);
		//use alpha for source color, 1-alpha for destination color
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program that we will use for local shading
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightPos");
		prog->addUniform("viewPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");

		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("Texture0");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/water.jpg");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}

	void initGeom(const std::string& resourceDirectory)
	{
		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		

		// Initialize bunny mesh.
		vector<tinyobj::shape_t> TOshapesB;
 		vector<tinyobj::material_t> objMaterialsB;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/icoNoNormals.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			ball = make_shared<Shape>();
			ball->createShape(TOshapesB[0]);
			ball->computeNormals();
			ball->measure();
			ball->init(false);
		}

		vector<tinyobj::shape_t> TOshapes3;
		rc = tinyobj::LoadObj(TOshapes3, objMaterials, errStr, (resourceDirectory + "/dummy.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {

			for (size_t i = 0; i < TOshapes3.size(); ++i) {
				Shape s;
				s.createShape(TOshapes3[i]);
				dummy->push_back(s);
				dummy->at(i).measure();
				dummy->at(i).init(false);

			}
			dummyBBox = getMultiShapeBBox(dummy);
		}
		vector<tinyobj::shape_t> TOshapes5;
		rc = tinyobj::LoadObj(TOshapes5, objMaterials, errStr, (resourceDirectory + "/Hockey Gates model.obj").c_str());
		resize_obj(TOshapes5);
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			goal = make_shared<Shape>();
			
			goal->createShape(TOshapes5[0]);
			goal->measure();
			goal->init(false);
		}
		vector<tinyobj::shape_t> TOshapes6;
		rc = tinyobj::LoadObj(TOshapes6, objMaterials, errStr, (resourceDirectory + "/pool.obj").c_str());
		resize_obj(TOshapes6);
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			for (size_t i = 0; i < TOshapes6.size(); ++i) {
				Shape s;
				s.createShape(TOshapes6[i]);
				//s.computeNormals();
				s.reverseNormals();
				pool->push_back(s);
				pool->at(i).measure();
				pool->at(i).init(false);

			}
			
			poolBBox = getMultiShapeBBox(pool);
		}
		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 20;
		float g_groundY = 1.00;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }

      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
     	texture0->bind(curS->getUniform("Texture0"));
		//draw the ground plane 
		mat4 Trans = glm::translate(glm::mat4(1.0f), vec3(0,-0.15,0));
		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(0.15,1,0.6));
		mat4 ctm = Trans * ScaleS;
		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));

  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

     //helper function to pass material data to the GPU
	void SetMaterial(shared_ptr<Program> curS, int i) {

    	switch (i) {
    		case 0: //
    			glUniform3f(curS->getUniform("MatAmb"), 0.096f, 0.046f, 0.095f);
    			glUniform3f(curS->getUniform("MatDif"), 0.96f, 0.46f, 0.95f);
    			glUniform3f(curS->getUniform("MatSpec"), 0.45f, 0.23f, 0.45f);
    			glUniform1f(curS->getUniform("MatShine"), 120.0f);
    		break;
    		case 1: // 
    			glUniform3f(curS->getUniform("MatAmb"), 0.063f, 0.038f, 0.1f);
    			glUniform3f(curS->getUniform("MatDif"), 0.63f, 0.38f, 1.0f);
    			glUniform3f(curS->getUniform("MatSpec"), 0.3f, 0.2f, 0.5f);
    			glUniform1f(curS->getUniform("MatShine"), 4.0f);
    		break;
    		case 2: //
    			glUniform3f(curS->getUniform("MatAmb"), 0.004f, 0.05f, 0.09f);
    			glUniform3f(curS->getUniform("MatDif"), 0.04f, 0.5f, 0.9f);
    			glUniform3f(curS->getUniform("MatSpec"), 0.02f, 0.25f, 0.45f);
    			glUniform1f(curS->getUniform("MatShine"), 27.9f);
    		break;
			case 3:
				glUniform3f(prog->getUniform("MatAmb"), 0.09f, 0.09f, 0.09f);
				glUniform3f(prog->getUniform("MatDif"), 0.2f, 0.2f, 0.65f);
				glUniform3f(prog->getUniform("MatSpec"), 0.2f, 0.65f, 0.2f);
				glUniform1f(prog->getUniform("MatShine"), 120.0f);
			break;
			case 4:
				glUniform3f(prog->getUniform("MatAmb"), 0.095f, 0.095f, 0.095f);
				glUniform3f(prog->getUniform("MatDif"), 0.90f, 0.90f, 0.9f);
				glUniform3f(prog->getUniform("MatSpec"), 0.40f, 0.40f, 0.90f);
				glUniform1f(prog->getUniform("MatShine"), 16.0f);
			break;
			case 5:
				glUniform3f(prog->getUniform("MatAmb"), 0.095f, 0.095f, 0.095f);
				glUniform3f(prog->getUniform("MatDif"), 0.40f, 0.40f, 0.4f);
				glUniform3f(prog->getUniform("MatSpec"), 0.40f, 0.40f, 0.40f);
				glUniform1f(prog->getUniform("MatShine"), 16000.0f);
				break;
  		}
	}

	/* helper function to set model trasnforms */
  	void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}

	void shooterRender(std::shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		glUniform3f(prog->getUniform("MatAmb"), 0.065f, 0.020f, 0.020f);
		glUniform3f(prog->getUniform("MatDif"), 0.65f, 0.2f, 0.2f);
		glUniform3f(prog->getUniform("MatSpec"), 0.65f, 0.2f, 0.2f);
		glUniform1f(prog->getUniform("MatShine"), 200.0f);
		Model->loadIdentity();
		Model->translate(shooterTrans);
		Model->rotate(pi<float>() / 2, vec3(-1, 0, 0));
		Model->rotate(pi<float>() / 2, vec3(0, 0, 1));
		Model->scale(0.0050f);
		//draw the lower body
		setModel(prog, Model);
		for (size_t i = 0; i < 14; i++) {
			dummy->at(i).draw(prog);
		}
		//draw the upper body
			Model->pushMatrix();
				vec3 pivotBelly = getCenterOfBBox(dummy->at(13));
				Model->translate(pivotBelly);
				Model->rotate(0.5*shooterRot, vec3(0, 0, 1));
				Model->rotate(0.2*shooterRot, vec3(0, 1, 0));
				Model->translate(-pivotBelly);
				setModel(prog, Model);
				dummy->at(14).draw(prog);
				//draw the right arm
				shooterRightArmRender(Model);
				// draw the left arm
				shooterLeftArmRender(Model);
				//reverse-rotate the head and neck so that they stay aligned with hips
				Model->pushMatrix();
					vec3 pivotNeck = getCenterOfBBox(dummy->at(27));
					Model->translate(pivotNeck);
					Model->rotate(0.5*shooterRot, vec3(0, 0, -1));
					Model->rotate(0.2 * shooterRot, vec3(0, -1, 0));
					Model->translate(-pivotNeck);
					setModel(prog, Model);
					for (size_t i = 27; i < dummy->size(); i++) {
						dummy->at(i).draw(prog);
					}
				Model->popMatrix();
			Model->popMatrix();
		Model->popMatrix();
	}

	void shooterRightArmRender(std::shared_ptr<MatrixStack> Model) {
		int mirror = 1;
		int armIndex = 15;
		float shoulderRot = shooterRot;
		float elbowRot = cos(2 * pi<double>());
		Model->pushMatrix();
			vec3 pivotTorso = getCenterOfBBox(dummy->at(14));
			Model->translate(vec3(0, mirror * (1 * -0.5 + 5), 3 * -0.5));
			setModel(prog, Model);
			dummy->at(armIndex).draw(prog);
				Model->pushMatrix();
				vec3 rShoulder = getCenterOfBBox(dummy->at(armIndex));
				Model->translate(rShoulder); //center of shoulder
				//rotate upper arm towards goal just a small amount at the end of the throw. Hips, chest, and elbow does most of the work.
				Model->rotate((pi<float>() / 8) * shoulderRot + (pi<float>() / 8), vec3(mirror * 0, 0, 1)); 
				Model->translate(-rShoulder);
				setModel(prog, Model);
				dummy->at(armIndex + 1).draw(prog);
				dummy->at(armIndex + 2).draw(prog);

					Model->pushMatrix();
						vec3 rElbow = getCenterOfBBox(dummy->at(armIndex + 2));
						Model->translate(rElbow); //center of elbow
						Model->rotate((pi<float>() / 4), vec3(mirror * -1, 0, 0));
						Model->rotate((pi<float>() / 4) * elbowRot- (pi<float>() / 4), vec3(mirror * 0, 1, 0));
						Model->translate(-rElbow);
						setModel(prog, Model);
						setModel(prog, Model);
						dummy->at(armIndex + 3).draw(prog);
						dummy->at(armIndex + 4).draw(prog);
						Model->pushMatrix();
						vec3 rWrist = getCenterOfBBox(dummy->at(armIndex + 4));
						Model->translate(rWrist); //center of wrist
						Model->rotate(-0.5*pi<float>()/2*shooterRot +  pi<float>() / 2, vec3(0, -1, 0));
						
						Model->translate(-rWrist);
						Model->translate(getCenterOfBBox(dummy->at(armIndex + 5))); //move the ctm to the hand
						rHandAnchor = make_shared<MatrixStack>(*Model); //snapshot the ctm at this point
						Model->translate(-getCenterOfBBox(dummy->at(armIndex + 5)));
						setModel(prog, Model);
						dummy->at(armIndex + 5).draw(prog);
					Model->popMatrix();
				Model->popMatrix();
			Model->popMatrix();
		Model->popMatrix();
	}

	void ballRender(std::shared_ptr<MatrixStack> Model) {
		rHandAnchor->pushMatrix();
		glUniform3f(prog->getUniform("MatAmb"), 0.065f, 0.065f, 0.020f);
		glUniform3f(prog->getUniform("MatDif"), 0.65f, 0.65f, 0.2f);
		glUniform3f(prog->getUniform("MatSpec"), 0.65f, 0.65f, 0.2f);
		glUniform1f(prog->getUniform("MatShine"), 20.0f);
		rHandAnchor->scale(12);
		rHandAnchor->translate(vec3(0, 0, -1));
		setModel(prog, rHandAnchor);

		ball->draw(prog);
		rHandAnchor->popMatrix();
	}

	void shooterLeftArmRender(std::shared_ptr<MatrixStack> Model) {
		int mirror = -1;
		int armIndex = 21;

		Model->pushMatrix();
			vec3 pivotTorso = getCenterOfBBox(dummy->at(14));
			Model->translate(vec3(0, mirror * (1 * -0.5 + 5), 3 * -0.5));
			setModel(prog, Model);
			dummy->at(armIndex).draw(prog);
				Model->pushMatrix();
				vec3 rShoulder = getCenterOfBBox(dummy->at(armIndex));
				Model->translate(rShoulder); //center of shoulder
				Model->rotate((pi<float>() / 4) * -0.5 - (pi<float>() / 8), vec3(mirror * -1, 0, 0));
				Model->translate(-rShoulder);
				setModel(prog, Model);
				dummy->at(armIndex + 1).draw(prog);
				dummy->at(armIndex + 2).draw(prog);

					Model->pushMatrix();
						vec3 rElbow = getCenterOfBBox(dummy->at(armIndex + 2));
						Model->translate(rElbow); //center of shoulder
						Model->rotate((pi<float>() / 6) * -0.5 - (pi<float>() / 16), vec3(mirror * -1, 0, 0));
						Model->translate(-rElbow);
						setModel(prog, Model);
						setModel(prog, Model);
						dummy->at(armIndex + 3).draw(prog);
						dummy->at(armIndex + 4).draw(prog);
						Model->pushMatrix();
						vec3 rWrist = getCenterOfBBox(dummy->at(armIndex + 4));
						Model->translate(rWrist); //center of shoulder
						Model->rotate(pi<float>() / 2, vec3(0, -1, 0));
						Model->translate(-rWrist);
						
						
						setModel(prog, Model);
						
						dummy->at(armIndex + 5).draw(prog);
					Model->popMatrix();
				Model->popMatrix();
			Model->popMatrix();
		Model->popMatrix();
	}
	

	void goalieRender(std::shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		if (goalieColor) {
			glUniform3f(prog->getUniform("MatAmb"), 0.020f, 0.065f, 0.020f);
			glUniform3f(prog->getUniform("MatDif"), 0.2f, 0.65f, 0.2f);
			glUniform3f(prog->getUniform("MatSpec"), 0.2f, 0.65f, 0.2f);
			glUniform1f(prog->getUniform("MatShine"), 200.0f);
		}
		else {
			glUniform3f(prog->getUniform("MatAmb"), 0.085f, 0.085f, 0.020f);
			glUniform3f(prog->getUniform("MatDif"), 0.85f, 0.85f, 0.2f);
			glUniform3f(prog->getUniform("MatSpec"), 0.85f, 0.85f, 0.2f);
			glUniform1f(prog->getUniform("MatShine"), 200.0f);
		}
		Model->loadIdentity();
		//get the whole thing into position
		Model->translate(vec3(0, .27 + 0.04 * goalieTime, -9.3));
		Model->rotate(pi<float>() / 2, vec3(-1, 0, 0));
		Model->rotate(pi<float>() / 2, vec3(0, 0, -1));
		Model->scale(0.0050f);
		setModel(prog, Model);
		//draw torso and below
		for (size_t i = 0; i < 15; ++i) {
			dummy->at(i).draw(prog);
		}
		//draw neck and head with same transforms as lower body
		dummy->at(27).draw(prog);
		dummy->at(28).draw(prog);

		//right side
		goalieArmRender(Model, 15, 1);

		//left side
		goalieArmRender(Model, 21, -1);
		Model->popMatrix();
	}

	void goalieArmRender(std::shared_ptr<MatrixStack> Model, int armIndex, int mirror) {
		Model->pushMatrix();
		vec3 pivotTorso = getCenterOfBBox(dummy->at(14));
		Model->translate(vec3(0, mirror * (1 * goalieTime + 5), 3 * goalieTime));
		setModel(prog, Model);
		dummy->at(armIndex).draw(prog);
		Model->pushMatrix();
		vec3 rShoulder = getCenterOfBBox(dummy->at(armIndex));
		Model->translate(rShoulder); //center of shoulder
		Model->rotate((pi<float>() / 4) * goalieTime - (pi<float>() / 8), vec3(mirror * -1, 0, 0));
		Model->translate(-rShoulder);
		setModel(prog, Model);
		dummy->at(armIndex + 1).draw(prog);
		dummy->at(armIndex + 2).draw(prog);

		Model->pushMatrix();
		vec3 rElbow = getCenterOfBBox(dummy->at(armIndex + 2));
		Model->translate(rElbow); //center of shoulder
		Model->rotate((pi<float>() / 6) * goalieTime - (pi<float>() / 16), vec3(mirror * -1, 0, 0));
		Model->translate(-rElbow);
		setModel(prog, Model);
		setModel(prog, Model);
		dummy->at(armIndex + 3).draw(prog);
		dummy->at(armIndex + 4).draw(prog);
		Model->pushMatrix();
		vec3 rWrist = getCenterOfBBox(dummy->at(armIndex + 4));
		Model->translate(rWrist); //center of shoulder
		Model->rotate(pi<float>() / 2, vec3(0, -1, 0));
		Model->translate(-rWrist);
		setModel(prog, Model);
		dummy->at(armIndex + 5).draw(prog);
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
	}

	void goalRender(std::shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		SetMaterial(prog, 5);
		Model->loadIdentity();
		Model->translate(goalTrans);
		Model->scale(vec3(.72, .54, .6));
		setModel(prog, Model);
		goal->draw(prog);
		Model->popMatrix();
	}

	void poolRender(std::shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		SetMaterial(prog, 3);
		Model->loadIdentity();
		Model->translate(vec3(0, 2, 0));
		Model->rotate(pi<float>(), vec3(0, 1, 0));
		Model->scale(vec3(12, 12, 12));
		setModel(prog, Model);
		for (size_t i = 0; i < pool->size(); ++i) {
			if (i == 1) {
				continue; //don't draw the water
			}
			if (i <= 1) {
				SetMaterial(prog, 3);
			}
			else {
				SetMaterial(prog, 4);
			}
			pool->at(i).draw(prog);
		}
		Model->popMatrix();
	}

	void render() {
		glTime = glfwGetTime();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// View is global translation along negative z for now
		View->pushMatrix();
		View->loadIdentity();
		//for now the goal is centered on the origin, so cameraCenter is all 0, 
		//but if you wanted to rotate around an arbitrary object, that's how you would do it
		vec3 cameraCenter = shooterTrans;
		int cameraDistance = fmax(3,3 - gZoom);
		int cameraHeight = 1;
		CameraPos = vec3(
			//sin and cos to get camera moving on a circle cameraHeight units above x-z plane, with a radius of cameraDistance, centered on CameraCenter.
			
			cameraCenter.x + cameraDistance * (sin(gRot * pi<double>() / 4.0)),
			cameraCenter.y + cameraHeight * (gTilt * pi<double>() / 4.0) + cameraHeight,
			cameraCenter.z + cameraDistance * (cos(gRot * pi<double>() / 4.0)));
		
		//gives matrix of a view from arg1 to arg2
			glm::mat4 CameraMatrix = glm::lookAt(CameraPos, cameraCenter, vec3(0, 1, 0));
			View->multMatrix(CameraMatrix);

		// Draw the scene
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(prog->getUniform("viewPos"), cameraCenter.x, cameraCenter.y, cameraCenter.z);
		glUniform3f(prog->getUniform("lightPos"), -2.0f , 2.0f, 2.0f - lightTrans);

		
		
		//goal
		goalRender(Model);
		//goalie
		goalieRender(Model);
		//shooter
		shooterRender(Model);
		//ball
		ballRender(Model);
		//pool
		poolRender(Model);
		


		prog->unbind();

		

		//switch shaders to the texture mapping shader and draw the ground
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				
		drawGround(texProg);

		texProg->unbind();
		
		//animation update example
		sTheta = sin((float)glfwGetTime());
		eTheta = std::max(0.0f, (float)sin(glfwGetTime()));
		hTheta = std::max(0.0f, (float)cos(glfwGetTime()));
		//how fast the goalie does his animation, controlled by animSpeed.
		shooterRot = cos(pi<double>() * glTime);
		goalieTime = cos(2 * pi<double>() * glTime / animSpeed);
		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
