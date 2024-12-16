#ifndef AXIS_XYZ_H
#define AXIS_XYZ_H

#include <glad/gl.h>
#include <glm/glm.hpp>

struct AxisXYZ {
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint colorBufferID;
    GLuint mvpMatrixID;
    GLuint programID;

    void initialize();
    void render(glm::mat4 cameraMatrix);
    void cleanup();

    GLfloat vertex_buffer_data[18] = {
        // X axis
        0.0, 0.0f, 0.0f,
        100.0f, 0.0f, 0.0f,

        // Y axis
        0.0f, 0.0f, 0.0f,
        0.0f, 100.0f, 0.0f,

        // Z axis
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 100.0f
    };

    GLfloat color_buffer_data[18] = {
        // X, red
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // Y, green
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // Z, blue
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
};

#endif