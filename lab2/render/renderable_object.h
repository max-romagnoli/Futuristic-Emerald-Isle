#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include <glm/glm.hpp>
#include <string>

#include "glad/gl.h"

class RenderableObject {
protected:
    GLuint vertexArrayID = 0;
    GLuint vertexBufferID = 0;
    GLuint colorBufferID = 0;
    GLuint indexBufferID = 0;
    GLuint uvBufferID = 0;

    GLuint programID = 0;
    GLuint mvpMatrixID = 0;
    GLuint textureID = 0;
    GLuint textureSamplerID = 0;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    // Virtual method to be implemented by subclasses for setting up data
    virtual void setupBuffers() = 0;

public:
    virtual ~RenderableObject() = default;

    // Initializes the object (e.g., shaders, buffers)
    virtual void initialize(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    // Renders the object
    virtual void render(const glm::mat4& vp);

    // Cleans up GPU resources
    virtual void cleanup();
};

#endif