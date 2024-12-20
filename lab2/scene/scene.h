#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include <render/building.h>
#include "render/axys_xyz.h"
#include "render/skybox.h"
#include "render/terrain.h"
#include "render/tree.h"
#include "render/forest.h"

class Scene {

public:
    std::vector<Building> buildings;
    AxisXYZ axis;
    Skybox skybox;
    Terrain terrain;
    //Tree tree;
    Forest forest;

    ~Scene();

    void initializeCity(int cityRows, int cityCols, float buildingWidth, float buildingSpacing);
    void initializeAxis();
    void initializeSkybox(glm::vec3 position, glm::vec3 scale);
    void initializeTerrain(int width, int depth, float maxHeight);
    void initializeCityOnHill(const glm::vec3& hillPosition, int cityRows, int cityCols, float buildingWidth, float buildingSpacing);
    void initializeCitiesOnHills(int nCities);
    void initializeForest(const Terrain &terrain);
    void initializeTree(const Terrain &terrain);

    void render(const glm::mat4& vp, const glm::vec3& cameraPosition);
    void cleanup();

    /*** LEGACY METHODS ***/
    void initializeBuildingsOnTerrain(Terrain &terrain);
    void placeBuildingsOnHills(Terrain &terrain, int numHills, int buildingsPerHill);
};

#endif