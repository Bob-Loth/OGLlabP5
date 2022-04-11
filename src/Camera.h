#ifndef Camera_h
#define Camera_h

#include "WindowManager.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Texture.h"
#include "GLSL.h"

class Camera{
public:
    Camera() : eyePos(glm::vec3(0,1,0)) {}
    Camera(glm::vec3 initialPos) : eyePos(initialPos + glm::vec3(w.x, w.y + 1, w.z)){}
    
    void updateView(std::shared_ptr<MatrixStack> &View);
    void processWASDInput(glm::vec3& shooterPos);
    
    
    glm::vec3 dEyePos = glm::vec3(0, 2, 0);
    glm::vec3 w = glm::vec3(0, 0, 0);
    glm::vec3 eyePos;
    glm::vec3 u = glm::vec3(0, 0, 0);
    
    
    double mousePrevX, mousePrevY, deltaMouseX, deltaMouseY;
    double xSensitivity = 0.01;
    double ySensitivity = 0.01;
    float xRot = glm::radians(-90.0f), yRot = 0;
    bool isWASDPressed[4] = { false, false, false, false };
    int wState = GLFW_RELEASE, aState = GLFW_RELEASE, sState = GLFW_RELEASE, dState = GLFW_RELEASE;
    float movementSensitivity = 0.025;
    
    bool firstMouse = true;
};

#endif /* Camera_hpp */
