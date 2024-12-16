#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include <render/building.h>
#include "render/axys_xyz.h"
#include "render/skybox.h"

class Scene {

public:
    std::vector<Building> buildings;
    AxisXYZ axis;
    Skybox skybox; // Add Skybox
    ~Scene();

    void initializeCity(int cityRows, int cityCols, float buildingWidth, float buildingSpacing);
    void initializeAxis();
    void initializeSkybox(glm::vec3 position, glm::vec3 scale);
    void render(const glm::mat4& vp);
    void cleanup();
};

#endif