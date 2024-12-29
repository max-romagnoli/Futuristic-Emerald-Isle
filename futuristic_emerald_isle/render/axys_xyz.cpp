#include "axys_xyz.h"

#include <iostream>

#include "shader.h"

void AxisXYZ::initialize() {
    // Load shaders for the axis
    programID = LoadShadersFromFile("../futuristic_emerald_isle/shaders/axis.vert", "../futuristic_emerald_isle/shaders/axis.frag");
    if (programID == 0) {
        std::cerr << "Failed to load axis shaders." << std::endl;
    }

    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create a vertex buffer object to store the vertex data
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

    // Get a handle for our "MVP" uniform
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
}


void AxisXYZ::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Attach matrix to id-defined variable in vertex shader
    glm::mat4 mvp = cameraMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Draw the lines
    glDrawArrays(GL_LINES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void AxisXYZ::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
}