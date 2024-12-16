#include "renderable_object.h"
#include "shader.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void RenderableObject::initialize(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Load shaders
    programID = LoadShadersFromFile(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if (programID == 0) {
        std::cerr << "Failed to load shaders: " << vertexShaderPath << " and " << fragmentShaderPath << std::endl;
        return;
    }

    // Get a handle for the "MVP" uniform
    mvpMatrixID = glGetUniformLocation(programID, "MVP");

    // Call subclass method to set up vertex, color, and other buffers
    setupBuffers();
}

void RenderableObject::render(const glm::mat4& vp) {
    if (programID == 0 || vertexArrayID == 0) return; // Ensure valid program and VAO

    glUseProgram(programID); // Use the correct shader program
    glBindVertexArray(vertexArrayID); // Bind VAO

    // Model matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    modelMatrix = glm::scale(modelMatrix, scale);

    // MVP matrix
    glm::mat4 mvp = vp * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Ensure all attributes (position, color, UV) are enabled
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // color
    glEnableVertexAttribArray(2); // UV

    // Texture
    if (textureID > 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(textureSamplerID, 0); // Texture unit 0
    }

    // Draw the object
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // Cleanup bindings
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
}

void RenderableObject::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(programID);
}