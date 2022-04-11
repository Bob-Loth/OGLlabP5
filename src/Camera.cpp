#include "Camera.h"

using namespace glm;

void Camera::updateView(std::shared_ptr<MatrixStack> &View) {
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
}

void Camera::processWASDInput(glm::vec3& shooterPos) {
    if (isWASDPressed[0]) {
        eyePos -= movementSensitivity * vec3(w.x, 0.0f, w.z);
        shooterPos -= movementSensitivity * vec3(w.x, 0.0f, w.z);
        dEyePos = eyePos;
    }
    if (isWASDPressed[1]) {
        eyePos += movementSensitivity * vec3(u.x, 0.0f, u.z);
        shooterPos += movementSensitivity * vec3(u.x, 0.0f, u.z);
        dEyePos = eyePos;
    }
    if (isWASDPressed[2]) {
        eyePos += movementSensitivity * vec3(w.x, 0.0f, w.z);
        shooterPos += movementSensitivity * vec3(w.x, 0.0f, w.z);
        dEyePos = eyePos;
    }
    if (isWASDPressed[3]) {
        eyePos -= movementSensitivity * vec3(u.x, 0.0f, u.z);
        shooterPos -= movementSensitivity * vec3(u.x, 0.0f, u.z);
        dEyePos = eyePos;
    }
}

