#ifndef CARS_H
#define CARS_H

#include <string>
#include "car.h"
#include <vector>

// TODO:
class Terrain;

class Cars {
public:
    Cars();
    ~Cars();

    bool initialize(const std::string& modelPath);
    void generateCars();
    void render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius, glm::vec3 lightPosition, glm::vec3 lightIntensity, double deltaTime);
    void cleanup();

private:
    std::vector<Car> cars;
};

#endif