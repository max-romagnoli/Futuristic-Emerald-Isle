#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <render/building.h>
#include "render/axys_xyz.h"
#include "render/birds.h"
#include "render/cars.h"
#include "render/city.h"
#include "render/skybox.h"
#include "render/terrain.h"
#include "render/forest.h"
#include "utils/light_cube.cpp"

class Scene {

public:
    LightCube lightCube;
    glm::vec3 lightIntensity;
    glm::vec3 lightPosition;

    std::vector<Building> buildings;
    std::vector<City> cities;
    AxisXYZ axis;
    Skybox skybox;
    Terrain terrain;
    Cars cars;
    Birds birds;
    Forest forestLOD0, forestLOD1, forestLOD2;

    ~Scene();

    void setupLighting();
    void adjustLighting(float threshold, float darkFactor, const glm::vec3& cameraPosition);

    void initializeCity(int cityRows, int cityCols, float buildingWidth, float buildingSpacing);
    void initializeAxis();
    void initializeSkybox(glm::vec3 position, glm::vec3 scale);
    void initializeTerrain(int width, int depth, float maxHeight);
    void initializeCityOnHill(const glm::vec3& hillPosition, int cityRows, int cityCols, float buildingWidth, float buildingSpacing);
    void initializeCitiesOnHills(int nCities);
    void initializeForest(const Terrain &terrain, int nTrees);
    void initializeCars(int nCars);
    void initializeBirds(int nBirds);

    void render(const glm::mat4& vp, const glm::vec3& cameraPosition, double deltaTime);
    void cleanup();

    /*** LEGACY METHODS ***/
    void initializeBuildingsOnTerrain(Terrain &terrain);
    void placeBuildingsOnHills(Terrain &terrain, int numHills, int buildingsPerHill);
};

#endif