#include "city.h"
#include <iostream>
#include <sstream>
#include "shader.h"
#include <utils/load_textures.h>

City::City() : programID(0), textureID(0), mvpMatrixID(0), modelMatrixID(0), lightPositionID(0), lightIntensityID(0) {}

City::~City() {}

bool City::initialize() {
    programID = LoadShadersFromFile("../lab2/shaders/box.vert", "../lab2/shaders/box.frag");
    if (programID == 0) {
        std::cerr << "Failed to load shaders for city!" << std::endl;
        return false;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");

    for (const int i : {0, 1, 2, 3, 4, 5}) {
        std::stringstream texturePath;
        texturePath << "../lab2/assets/textures/facade" << i << ".jpg";
        facades.push_back(LoadTextureTileBox(texturePath.str().c_str()));
    }

    return true;
}

void City::addBuilding(glm::vec3 position, glm::vec3 scale, int vFactor, GLuint facadeID) {
    Building b;
    b.initialize(position, scale, vFactor, facadeID);
    buildings.push_back(b);
}

void City::render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius, glm::vec3 lightPosition, glm::vec3 lightIntensity) {
    for (Building& b : buildings) {
        float distanceToCamera = glm::distance(b.position, cameraPosition);
        if (distanceToCamera <= renderRadius) {
            b.render(vp, programID, textureID, mvpMatrixID, modelMatrixID, lightPositionID, lightIntensityID, lightPosition, lightIntensity);
        }
    }
}

void City::cleanup() {
    for (Building& b : buildings) {
        b.cleanup();
    }
    glDeleteProgram(programID);
    glDeleteTextures(1, &textureID);
}