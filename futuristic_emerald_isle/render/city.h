#ifndef CITY_H
#define CITY_H

#include "building.h"
#include <vector>
#include <glm/glm.hpp>

class City {
public:
    std::vector<GLuint> facades;
    std::vector<Building> buildings;

    City();
    ~City();

    bool initialize();
    void addBuilding(glm::vec3 position, glm::vec3 scale, int vFactor, GLuint facadeID);
    void render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius, glm::vec3 lightPosition, glm::vec3 lightIntensity);
    void cleanup();

private:
    GLuint programID;
    GLuint textureID;
    GLuint mvpMatrixID;
    GLuint modelMatrixID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
};

#endif