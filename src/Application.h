#ifndef Application_h
#define Application_h

#include <memory>


#include "WindowManager.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Texture.h"
#include "GLSL.h"
#include "Camera.h"


class Application : public EventCallbacks
{
public:
    WindowManager * windowManager = nullptr;
    
    
    // Our shader program - use this one for Blinn-Phong
    std::shared_ptr<Program> prog;
    //Our shader program for textures
    std::shared_ptr<Program> texProg;

    std::shared_ptr<Shape> sphere;
    std::shared_ptr<std::vector<Shape>> dummy = std::make_shared<std::vector<Shape>>();
    std::vector<glm::vec3> dummyBBox;
    std::shared_ptr<Shape> ball;
    std::shared_ptr<Shape> sky;
    //global data for ground plane - direct load constant defined CPU data to GPU (not obj)
    typedef struct Ground{
        GLuint buffObj;
        GLuint norBuffObj;
        GLuint texBuffObj;
        GLuint indexBuffObj;
        int giboLen;
    }Ground;
    Ground ground;
    
    
    //ground VAO
    GLuint GroundVertexArrayID;

    //the texture data, initialized in initTex
    std::vector<std::shared_ptr<Texture>> textures;
    
    float cumulativeFrametime = 0.0f;

    //camera data
    Camera camera;
    
    //global animation/misc data
    float lightTrans = 0;
    
    
    glm::vec3 getCenterOfBBox(Shape s) {
        return glm::vec3(
            (s.max.x + s.min.x) / 2,
            (s.max.y + s.min.y) / 2,
            (s.max.z + s.min.z) / 2
        );
    }
    // returns a vector of size 2. position 0 is the lowest-value point,
    // 1 is the highest, that describes the bounding box.
    std::vector<glm::vec3> getMultiShapeBBox(std::shared_ptr<std::vector<Shape>> shapes);
    
    //does nothing by default
    virtual void checkCollisions(){};

    virtual void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) = 0;
    virtual void mouseCallback(GLFWwindow *window, int button, int action, int mods) = 0;
    //mouse movement implemented in concrete class.
    //by default, does nothing
    virtual void scrollCallback(GLFWwindow* window, double deltaX, double deltaY){}
    virtual void resizeCallback(GLFWwindow *window, int width, int height){ glViewport(0, 0, width, height); }

    //setup functions, usually called once before render
    virtual void init(const std::string& resourceDirectory) = 0;
    virtual void initGeom(const std::string& resourceDirectory) = 0;
    virtual void initTex(const std::string& resourceDirectory) = 0;
    //directly pass quad for the ground to the GPU

    //code to draw the ground plane
    virtual void drawGround(std::shared_ptr<Program> curS);

    /* helper function to set model transforms */
    void SetModel(glm::vec3 trans, float rotY, float rotX, float sc, std::shared_ptr<Program> curS);
    void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M);
    void resize_obj(std::vector<tinyobj::shape_t>& shapes);
    
    //called every frame
    virtual void render(float frametime) = 0;
};
#endif /* Application_hpp */