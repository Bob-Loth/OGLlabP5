/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>
#include <chrono>

#include "Lab1Application.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_SWIZZLE
#include <glm/glm.hpp>


using namespace std;
using namespace glm;



int main(int argc, char *argv[])
{
    srand(time(0));
    // Where the resources are loaded from
    std::string resourceDir = "../resources";

    if (argc >= 2)
    {
        resourceDir = argv[1];
    }

    Application *application = new Lab1Application();

    // Your main will always include a similar set up to establish your window
    // and GL context, etc.

    WindowManager *windowManager = new WindowManager();
    windowManager->init(1920, 1080);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state

    application->init(resourceDir);
    application->initGeom(resourceDir);
    application->initTex(resourceDir);
    auto lastTime = chrono::high_resolution_clock::now();
    // Loop until the user closes the window.
    while (! glfwWindowShouldClose(windowManager->getHandle()))
    {
        // save current time for next frame
        auto nextLastTime = chrono::high_resolution_clock::now();

        // get time since last frame
        float deltaTime =
            chrono::duration_cast<std::chrono::microseconds>(
                chrono::high_resolution_clock::now() - lastTime)
            .count();
        // convert microseconds (weird) to seconds (less weird)
        deltaTime *= 0.000001;

        // reset lastTime so that we can calculate the deltaTime
        // on the next frame
        lastTime = nextLastTime;
        // Render scene.
        application->render(deltaTime);

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}
