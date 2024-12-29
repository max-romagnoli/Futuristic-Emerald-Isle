#include <iostream>
#include <string>
#include "glad/gl.h"
#include "utils.h"

void CheckGLError(const std::string& functionName) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in " << functionName << ": " << err << std::endl;
    }
}