//
//  Lab1Application.hpp
//  P3Base
//
//  Created by Bob Loth on 4/10/22.
//

#ifndef Lab1Application_h
#define Lab1Application_h



#include <iostream>

#include "Application.h"


class Lab1Application : public Application
{
public:
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void updateBallPhysics();
    void drawBallPhysics(std::shared_ptr<MatrixStack> Model);
    void ballRender(std::shared_ptr<MatrixStack> Model);
    
    void checkCollisions();
    
    void shooterRender(std::shared_ptr<MatrixStack> Model);
    void shooterLegRender(std::shared_ptr<MatrixStack> Model, bool isRight);
    void shooterRightArmRender(std::shared_ptr<MatrixStack> Model);
    void shooterLeftArmRender(std::shared_ptr<MatrixStack> Model);
    
    void skyBoxRender(std::shared_ptr<MatrixStack> Model);
    
    void SetMaterial(std::shared_ptr<Program> curS, int i);
    void init(const std::string& resourceDirectory);
    void initGeom(const std::string& resourceDirectory);
    void initGround();
    void initTex(const std::string& resourceDirectory);
    void render(float frametime);
    
private:
    //general physics data
    typedef struct Physics {
        float FORCE_MULT = 0.0001f;
        glm::vec3 g = glm::vec3(0.0f, -1.98f, 0.0f);
        glm::vec3 buoyancy = -g;
    }Physics;
    Physics physics;
    
    //ball's position, and velocity constants
    typedef struct BallPhysics {
        glm::vec3 pos = glm::vec3(0); //hand pos
        glm::vec3 v;
        float rot;
        bool firstShotRender = false;
        bool isActive = false;
        bool lobbed = false;
        double timeSinceThrown = 0.0;
    }BallPhysics;
    BallPhysics ballPhysics;
    
    typedef struct ShooterAnim{
        float rot = 0;
        float kickSpeed = 0;
        std::shared_ptr<MatrixStack> rHandAnchor;
    }ShooterAnim;
    ShooterAnim shooterAnim;
};

    



#endif /* Lab1Application_h */
