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
    
    void ballPhysics();
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
    
};




#endif /* Lab1Application_h */
