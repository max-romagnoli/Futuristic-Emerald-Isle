#ifndef INIT_GLFW_GLAD_H
#define INIT_GLFW_GLAD_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow* initializeOpenGL(const char* windowTitle, int width, int height);

#endif