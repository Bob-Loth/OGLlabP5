//
//  Lab1Application.cpp
//  P3Base
//
//  Created by Bob Loth on 4/10/22.
//

#include "Lab1Application.h"

using namespace std;
using namespace glm;

void Lab1Application::checkCollisions(){
    //TODO make a sensible collision algorithm with the required added entities.
}

void Lab1Application::render(float frametime) {
    
    processWASDInput();
    checkCollisions();
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width/(float)height;

    // Create the matrix stacks
    auto Projection = make_shared<MatrixStack>();
    auto View = make_shared<MatrixStack>();
    auto Model = make_shared<MatrixStack>();

    // Apply perspective projection.
    Projection->pushMatrix();
    Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

    // View is behind shooter
    View->pushMatrix();
    
    View->loadIdentity();
    vec3 target = normalize(vec3(
        cos(xRot) * cos(yRot), //x
        sin(yRot), //y
        sin(xRot) * cos(yRot))); //z
    vec3 up = vec3(0, 1, 0);
    mat4 lookAt = glm::lookAt(
        eyePos - target, //eyepos
        eyePos + target,
        up); //up
    View->multMatrix(lookAt);

    // Draw the scene
    prog->bind();
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniform3f(prog->getUniform("lightPos"), -2.0f , 2.0f, 2.0f - lightTrans);
    
    //shooter
    shooterRender(Model);
    prog->unbind();

    //switch shaders to the texture mapping shader and draw the ground
    texProg->bind();
    glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniform3f(texProg->getUniform("lightPos"), -2.0f, 2.0f, 2.0f - lightTrans);
    glUniform1f(texProg->getUniform("alpha"), 1.0f);
    textures[2]->bind(texProg->getUniform("Texture0"));
    glUniform1i(texProg->getUniform("flip"), 1);
    if (ballPhysics.isActive) {
        updateBallPhysics();
        drawBallPhysics(Model);
        if (ballPhysics.timeSinceThrown + 10.0 < cumulativeFrametime) {
            ballPhysics.isActive = false;
        }
    }
    else {
        ballRender(Model);
    }
    texProg->unbind();
    
    
    skyBoxRender(Model);
    drawGround(texProg);

    
    //how fast the shooter does his animation
    cumulativeFrametime += frametime;

    shooterAnim.rot = cos(pi<double>() * cumulativeFrametime);
    shooterAnim.kickSpeed = cos(2 * pi<double>() * cumulativeFrametime);
    // Pop matrix stacks.
    Projection->popMatrix();
    View->popMatrix();

}

void Lab1Application::mouseCallback(GLFWwindow *window, int button, int action, int mods){
    double posX, posY;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        ballPhysics.lobbed = false;
        
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        ballPhysics.lobbed = true;
    }

    if (action == GLFW_PRESS) {
        ballPhysics.pos = vec3(shooterAnim.rHandAnchor->topMatrix()[3][0], shooterAnim.rHandAnchor->topMatrix()[3][1], shooterAnim.rHandAnchor->topMatrix()[3][2]);

        ballPhysics.firstShotRender = true;
        ballPhysics.isActive = true;
        if (ballPhysics.lobbed) {
            ballPhysics.v = physics.FORCE_MULT * 300 * -vec3(w.x, w.y - 1, w.z);
        }
        else {
            ballPhysics.v = physics.FORCE_MULT * 500 * -vec3(w.x, w.y - 0.5, w.z);
        }
        ballPhysics.timeSinceThrown = cumulativeFrametime;
    }
}

void Lab1Application::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        isWASDPressed[0] = true;
        //eyePos -= movementSensitivity * w;
    }
    
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        isWASDPressed[1] = true;
        //eyePos += movementSensitivity * u;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        isWASDPressed[2] = true;
        //eyePos += movementSensitivity * w;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        isWASDPressed[3] = true;
        //eyePos -= movementSensitivity * u;
    }

    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        isWASDPressed[0] = false;
    }

    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        isWASDPressed[1] = false;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        isWASDPressed[2] = false;
    }

    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        isWASDPressed[3] = false;
    }

    //move light
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        lightTrans += 1.f;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        lightTrans -= 1.f;
    }
    //wireframe
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        movementSensitivity /= 2;
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        movementSensitivity *= 2;
    }
}

void Lab1Application::updateBallPhysics() {
    //throw the ball with high velocity, at a lower angle.
    ballPhysics.v += physics.FORCE_MULT * physics.g;
    float depth = -ballPhysics.pos.y; //depth is positive if below 0 height.
    if (depth > 0.0) {
        ballPhysics.v += physics.FORCE_MULT * (physics.buoyancy * (1.0f + 0.6f * depth));
        ballPhysics.v.x = 0.975 * ballPhysics.v.x;
        if (ballPhysics.pos.y - (shooterTrans.y + 0.6f) < 0.05 && ballPhysics.v.y < 0) {
            
            if (length(vec3(ballPhysics.v.x, 0.0f, ballPhysics.v.z)) > 0.04) {
                ballPhysics.v.y = -ballPhysics.v.y;
                ballPhysics.v.x *= 0.925;
                ballPhysics.v.z *= 0.925;
                ballPhysics.pos.y += 0.005;
            }
            else {
                ballPhysics.v.y = 0.975 * ballPhysics.v.y;
            }
        }
        ballPhysics.v.z = 0.975 * ballPhysics.v.z;
    }
    ballPhysics.pos += ballPhysics.v;
}

void Lab1Application::drawBallPhysics(std::shared_ptr<MatrixStack> Model) {
    texProg->bind();
    
    textures[1]->bind(texProg->getUniform("Texture0"));
    glUniform1i(texProg->getUniform("flip"), 1);

    Model->pushMatrix();
    
    if (ballPhysics.firstShotRender) {
        ballPhysics.pos = vec3(shooterAnim.rHandAnchor->topMatrix()[3][0], shooterAnim.rHandAnchor->topMatrix()[3][1], shooterAnim.rHandAnchor->topMatrix()[3][2]);
        ballPhysics.firstShotRender = false;
    }
    Model->translate(ballPhysics.pos);
    Model->scale(0.08f);
    ballPhysics.rot += 2*length(ballPhysics.v);
    Model->rotate(ballPhysics.rot, cross(vec3(0, 1, 0), ballPhysics.v));
    setModel(texProg, Model);
    ball->draw(texProg);
    
    texProg->unbind();
}

void Lab1Application::shooterLegRender(std::shared_ptr<MatrixStack> Model, bool isRight) {
    int offset = 0;
    int flip = 1;
    if (!isRight) {
        offset = 6;
        flip = -flip;
    }

    Model->pushMatrix();
        vec3 pivotRPelvis = getCenterOfBBox(dummy->at(5 + offset));
        Model->translate(pivotRPelvis);
        Model->rotate(flip * 0.5 * shooterAnim.kickSpeed, vec3(0, 1, 0));
        Model->translate(-pivotRPelvis);
        setModel(prog, Model);
        dummy->at(4 + offset).draw(prog);
        dummy->at(5 + offset).draw(prog);
        Model->pushMatrix();
            vec3 pivotRKnee = getCenterOfBBox(dummy->at(3 + offset));
            Model->translate(pivotRKnee);
            Model->rotate(flip * 0.25 * shooterAnim.kickSpeed + pi<float>() / 8, vec3(0, 1, 0));
            Model->translate(-pivotRKnee);
            setModel(prog, Model);
            dummy->at(2 + offset).draw(prog);
            dummy->at(3 + offset).draw(prog);
            Model->pushMatrix();
                vec3 pivotRAnkle = getCenterOfBBox(dummy->at(1 + offset));
                Model->translate(pivotRAnkle);
                Model->rotate(pi<float>() / 3, vec3(0, 1, 0));
                Model->translate(-pivotRAnkle);
                setModel(prog, Model);
                dummy->at(0 + offset).draw(prog);
                dummy->at(1 + offset).draw(prog);
            Model->popMatrix();
        Model->popMatrix();
    Model->popMatrix();
}
void Lab1Application::shooterRender(std::shared_ptr<MatrixStack> Model) {
    Model->pushMatrix();
    glUniform3f(prog->getUniform("MatAmb"), 0.065f, 0.020f, 0.020f);
    glUniform3f(prog->getUniform("MatDif"), 0.65f, 0.2f, 0.2f);
    glUniform3f(prog->getUniform("MatSpec"), 0.65f, 0.2f, 0.2f);
    glUniform1f(prog->getUniform("MatShine"), 200.0f);
    Model->loadIdentity();
    Model->translate(shooterTrans);
    Model->rotate(pi<float>() / 2, vec3(-1, 0, 0));
    Model->rotate(-xRot, vec3(0, 0, 1));
    Model->scale(0.0050f);
    //draw the lower body
    setModel(prog, Model);
    //draw hips and belly
    for (size_t i = 12; i < 14; i++) {
        dummy->at(i).draw(prog);
    }
    //draw right leg
    shooterLegRender(Model, true);
    
    //draw left leg
    shooterLegRender(Model, false);
    
    //draw the upper body
        Model->pushMatrix();
            vec3 pivotBelly = getCenterOfBBox(dummy->at(13));
            Model->translate(pivotBelly);
            Model->rotate(0.5*shooterAnim.rot, vec3(0, 0, 1));
            Model->rotate(0.2*shooterAnim.rot, vec3(0, 1, 0));
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
                Model->rotate(0.5*shooterAnim.rot, vec3(0, 0, -1));
                Model->rotate(0.2 * shooterAnim.rot, vec3(0, -1, 0));
                Model->translate(-pivotNeck);
                setModel(prog, Model);
                for (size_t i = 27; i < dummy->size(); i++) {
                    dummy->at(i).draw(prog);
                }
            Model->popMatrix();
        Model->popMatrix();
    Model->popMatrix();
}

void Lab1Application::shooterRightArmRender(std::shared_ptr<MatrixStack> Model) {
    int mirror = 1;
    int armIndex = 15;
    float shoulderRot = shooterAnim.rot;
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
                    Model->rotate(-0.5*pi<float>()/2*shooterAnim.rot +  pi<float>() / 2, vec3(0, -1, 0));
                    
                    Model->translate(-rWrist);
                    Model->translate(getCenterOfBBox(dummy->at(armIndex + 5))); //move the ctm to the hand
                    shooterAnim.rHandAnchor = make_shared<MatrixStack>(*Model); //snapshot the ctm at this point
                    Model->translate(-getCenterOfBBox(dummy->at(armIndex + 5)));
                    setModel(prog, Model);
                    dummy->at(armIndex + 5).draw(prog);
                Model->popMatrix();
            Model->popMatrix();
        Model->popMatrix();
    Model->popMatrix();
}

void Lab1Application::ballRender(std::shared_ptr<MatrixStack> Model) {
    texProg->bind();
    textures[1]->bind(texProg->getUniform("Texture0"));
    glUniform1i(texProg->getUniform("flip"), 1);
    shooterAnim.rHandAnchor->pushMatrix();
    shooterAnim.rHandAnchor->scale(12);
    shooterAnim.rHandAnchor->translate(vec3(0, 0, -1));
    setModel(texProg, shooterAnim.rHandAnchor);
    if (ballPhysics.firstHandRender) {
        shooterAnim.handPos = vec3(shooterAnim.rHandAnchor->topMatrix()[3][0], shooterAnim.rHandAnchor->topMatrix()[3][1], shooterAnim.rHandAnchor->topMatrix()[3][2]);
        ballPhysics.firstHandRender = false;
    }
    
    
    ball->draw(texProg);
    shooterAnim.rHandAnchor->popMatrix();
    texProg->unbind();
}

void Lab1Application::skyBoxRender(std::shared_ptr<MatrixStack> Model) {
    texProg->bind();
    glUniform1i(texProg->getUniform("flip"), -1);
    glUniform3f(texProg->getUniform("lightPos"), -2.0f, 2.0f, 2.0f - lightTrans);
    textures[2]->bind(texProg->getUniform("Texture0"));
    Model->loadIdentity();
    Model->scale(40.0f);
    setModel(texProg, Model);
    sky->draw(texProg);
}

void Lab1Application::shooterLeftArmRender(std::shared_ptr<MatrixStack> Model) {
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

void Lab1Application::init(const std::string& resourceDirectory){
    GLSL::checkVersion();
    glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //allows for translucent water
    glEnable(GL_BLEND);
    //use alpha for source color, 1-alpha for destination color
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Set background color.
    glClearColor(.72f, .84f, 1.06f, 1.0f);
    // Enable z-buffer test.
    CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
    CHECKED_GL_CALL(glEnable(GL_BLEND));
    CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECKED_GL_CALL(glPointSize(14.0f));


    // Initialize the GLSL program that we will use for local shading
    prog = make_shared<Program>();
    prog->setVerbose(false);
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
    
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");

    // Initialize the GLSL program that we will use for texture mapping
    texProg = make_shared<Program>();
    texProg->setVerbose(false);
    texProg->setShaderNames(resourceDirectory + "/tex_vert_old.glsl", resourceDirectory + "/tex_frag0_old.glsl");
    texProg->init();
    texProg->addUniform("P");
    texProg->addUniform("V");
    texProg->addUniform("M");
    texProg->addUniform("Texture0");
    texProg->addUniform("alpha");
    texProg->addUniform("lightPos");
    texProg->addUniform("flip");
    texProg->addAttribute("vertPos");
    texProg->addAttribute("vertNor");
    texProg->addAttribute("vertTex");
}

void Lab1Application::initGeom(const std::string& resourceDirectory){
    //EXAMPLE set up to read one shape from one obj file - convert to read several
    // Initialize mesh
    // Load geometry
    // Some obj files contain material information.We'll ignore them for this assignment.
    string errStr;
    //load in the mesh and make the shape(s)

    // Initialize ball mesh.
    vector<tinyobj::shape_t> TOshapesBall;
    vector<tinyobj::material_t> objMaterials;
    //load in the mesh and make the shape(s)
    bool rc = tinyobj::LoadObj(TOshapesBall, objMaterials, errStr, (resourceDirectory + "/ballTex.obj").c_str(), (resourceDirectory + "untitled.mtl").c_str());
    if (!rc) {
        cerr << errStr << endl;
    } else {
        
        ball = make_shared<Shape>();
        ball->createShape(TOshapesBall[0]);

        ball->measure();
        ball->init(true);
    }

    vector<tinyobj::shape_t> TOshapesDummy;
    rc = tinyobj::LoadObj(TOshapesDummy, objMaterials, errStr, (resourceDirectory + "/dummy.obj").c_str());
    if (!rc) {
        cerr << errStr << endl;
    }
    else {

        for (size_t i = 0; i < TOshapesDummy.size(); ++i) {
            Shape s;
            s.createShape(TOshapesDummy[i]);
            dummy->push_back(s);
            dummy->at(i).measure();
            dummy->at(i).init(false);

        }
        dummyBBox = getMultiShapeBBox(dummy);
    }
    
    vector<tinyobj::shape_t> TOshapesSkybox;
    rc = tinyobj::LoadObj(TOshapesSkybox, objMaterials, errStr, (resourceDirectory + "/sphereTex.obj").c_str());
    resize_obj(TOshapesSkybox);
    if (!rc) {
        cerr << errStr << endl;
    }
    else {
        sky = make_shared<Shape>();
        sky->createShape(TOshapesSkybox[0]);
        sky->measure();
        sky->init(true);
    }
    //code to load in the ground plane (CPU defined data passed to GPU)
    initGround();
}

void Lab1Application::initGround() {

    float g_groundSize = 20;
    float g_groundY = 0.00;

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

    ground.giboLen = 6;
    glGenBuffers(1, &ground.buffObj);
    glBindBuffer(GL_ARRAY_BUFFER, ground.buffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

    glGenBuffers(1, &ground.norBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, ground.norBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

    glGenBuffers(1, &ground.texBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, ground.texBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

    glGenBuffers(1, &ground.indexBuffObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ground.indexBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}

void Lab1Application::initTex(const std::string& resourceDirectory) {
    //read in and load the texture
    
    textures.emplace_back(make_shared<Texture>());
    textures[0]->setFilename(resourceDirectory + "/water.jpg");
    textures[0]->init();
    textures[0]->setUnit(0);
    textures[0]->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    //water polo ball texture
    textures.emplace_back(make_shared<Texture>());
    textures[1]->setFilename(resourceDirectory + "/ballTex.png");
    textures[1]->initAlpha();
    textures[1]->setUnit(1);
    textures[1]->setWrapModes(GL_REPEAT, GL_REPEAT);

    //skybox texture
    textures.emplace_back(make_shared<Texture>());
    textures[2]->setFilename(resourceDirectory + "/cartoonSky.png");
    textures[2]->init();
    textures[2]->setUnit(2);
    textures[2]->setWrapModes(GL_REPEAT, GL_REPEAT);
}

void Lab1Application::SetMaterial(std::shared_ptr<Program> curS, int i) {

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
