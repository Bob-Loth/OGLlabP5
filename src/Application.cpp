#include "Application.h"


using namespace glm;
using namespace std;

glm::vec3 Application::getCenterOfBBox(Shape s) {
    return glm::vec3(
        (s.max.x + s.min.x) / 2,
        (s.max.y + s.min.y) / 2,
        (s.max.z + s.min.z) / 2
    );
}

std::vector<glm::vec3> Application::getMultiShapeBBox(std::shared_ptr<std::vector<Shape>> shapes) {
    std::vector<glm::vec3> BBox;
    BBox.push_back(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX));
    BBox.push_back(glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

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

void Application::resize_obj(std::vector<tinyobj::shape_t>& shapes) {
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
    float maxExtent = 0.0, xExtent, yExtent, zExtent;
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

void Application::SetModel(glm::vec3 trans, float rotY, float rotX, float sc, std::shared_ptr<Program> curS) {
    mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
    mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
    mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
    mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
    mat4 ctm = Trans*RotX*RotY*ScaleS;
    glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
}

void Application::setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
}

void Application::drawGround(std::shared_ptr<Program> curS) {
    curS->bind();
    glBindVertexArray(GroundVertexArrayID);
    glUniform1f(curS->getUniform("alpha"), 0.6f);
    glUniform1i(curS->getUniform("flip"), 1);
    textures[0]->bind(curS->getUniform("Texture0"));
    //draw the ground plane
    mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(0.15,1,0.6));
    mat4 ctm = ScaleS;
    glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, ground.buffObj);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, ground.norBuffObj);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, ground.texBuffObj);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ground.indexBuffObj);
    glDrawElements(GL_TRIANGLES, ground.giboLen, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    curS->unbind();
}
