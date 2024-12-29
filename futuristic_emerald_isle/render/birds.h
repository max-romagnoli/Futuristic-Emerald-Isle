#ifndef BIRDS_H
#define BIRDS_H

#include "bird.h"
#include <vector>

class Terrain;

class Birds {
public:
    Birds();
    ~Birds();

    bool initialize(const std::string& modelPath);
    void generateBirds(Terrain& terrain, int nBirds, float altitudeThreshold);
    void render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius, glm::vec3 lightPosition, glm::vec3 lightIntensity, double deltaTime);
    void cleanup();

private:
    std::vector<Bird> birds;
};

#endif