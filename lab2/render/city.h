#ifndef CITY_H
#define CITY_H

#include "building.h"
#include <vector>
#include <glm/glm.hpp>

class City {
public:
    std::vector<GLuint> facades;

    City();
    ~City();

    bool initialize();
    void addBuilding(glm::vec3 position, glm::vec3 scale, int vFactor, GLuint facadeID);
    void render(const glm::mat4& vp, glm::vec3 lightPosition, glm::vec3 lightIntensity);
    void cleanup();

private:
    std::vector<Building> buildings;

    GLuint programID;
    GLuint textureID;
    GLuint mvpMatrixID;
    GLuint modelMatrixID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
};

#endif