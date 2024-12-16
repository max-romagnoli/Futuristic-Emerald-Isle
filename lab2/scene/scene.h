#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include <render/building.h>
#include "render/axys_xyz.h"
#include "render/skybox.h"
#include "render/terrain.h"

class Scene {

public:
    std::vector<Building> buildings;
    AxisXYZ axis;
    Skybox skybox;
    Terrain terrain;
    ~Scene();

    void initializeCity(int cityRows, int cityCols, float buildingWidth, float buildingSpacing);
    void initializeAxis();
    void initializeSkybox(glm::vec3 position, glm::vec3 scale);
    void initializeTerrain(int width, int depth, float maxHeight);
    void render(const glm::mat4& vp);
    void cleanup();
};

#endif