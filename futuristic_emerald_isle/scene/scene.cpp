#include <iostream>
#include <scene/scene.h>
#include <random>
#include <set>
#include <render/Forest.h>

Scene::~Scene() {
    cleanup();
}

void Scene::setupLighting() {
    const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
    const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
    const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
    lightIntensity = 5.0f * (14.0f * wave500 + 10.0f * wave600 + 10.0f * wave700);
    lightPosition = glm::vec3(1360, 710, -1370);

    //lightCube.initialize();
    //lightCube.position = lightPosition;
}

void Scene::initializeAxis() {
    axis.initialize();
}

void Scene::initializeSkybox(glm::vec3 position, glm::vec3 scale) {
    skybox.initialize(position, scale);
}

void Scene::initializeTerrain(int width, int depth, float maxHeight) {
    terrain.initialize(width, depth, maxHeight, 100.0f);
}

void Scene::initializeCityOnHill(const glm::vec3& hillPosition, int cityRows, int cityCols, float buildingWidth, float buildingSpacing) {
    City city;
    if (!city.initialize()) {
        std::cerr << "Failed to initialize city!" << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> weights = {50, 50, 30, 20, 10, 5, 5};
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    float spacingX = buildingWidth + buildingSpacing;
    float spacingZ = buildingWidth + buildingSpacing;

    float startX = hillPosition.x - (cityCols / 2) * spacingX;
    float startZ = hillPosition.z - (cityRows / 2) * spacingZ;

    for (int i = 0; i < cityRows; ++i) {
        for (int j = 0; j < cityCols; ++j) {

            int vFactor = dist(gen) + 1;
            float newHeight = buildingWidth * vFactor;

            //float wFactor = 1 + (rand() % 3) / 2.0f;
            //float newWidth = buildingWidth * wFactor;
            float newWidth = buildingWidth;

            int textureNum = rand() % 5;

            glm::vec3 position(
                startX + j * spacingX,
                hillPosition.y + newHeight - 4.0f,
                startZ + i * spacingZ
            );

            city.addBuilding(
                position,
                glm::vec3(buildingWidth, newHeight, buildingWidth),
                vFactor,
                city.facades.at(textureNum)
            );

            cities.push_back(city);
        }
    }
}

void Scene::initializeCitiesOnHills(int nCities) {
    std::vector<glm::vec3> highestPoints = terrain.getHighestPoints(nCities);
    for (const auto& point : highestPoints) {
        initializeCityOnHill(point, 4, 4, 2.0f, 4.0f);
    }
}

void Scene::initializeForest(const Terrain& terrain, int nTrees) {
    std::random_device rd;
    std::mt19937 gen(rd());

    float halfWidth = terrain.getWidth() / 2.0f;
    float halfDepth = terrain.getDepth() / 2.0f;

    std::uniform_real_distribution<float> xDist(-halfWidth, halfWidth);
    std::uniform_real_distribution<float> zDist(-halfDepth, halfDepth);
    std::uniform_real_distribution<float> scaleDist(1.0f, 1.7f);
    std::uniform_real_distribution<float> rotationDist(0.0f, 360.0f);

    std::vector<glm::vec3> positions;
    std::vector<float> rotations, scales;
    int generated = 0;
    while (generated < 25000) {
        float x = xDist(gen);
        float z = zDist(gen);
        float altitude = terrain.getHeightAt(x, z);

        if (altitude <= 10.0f) {
            positions.emplace_back(x, altitude, z);
            float scaleFactor = scaleDist(gen);
            scales.emplace_back(scaleFactor);
            float yRotation = rotationDist(gen);
            rotations.emplace_back(yRotation);
            generated++;
        }
    }

    forestLOD0.initialize(0, 0.0f, 50.0f);
    forestLOD0.setupLOD(0, positions, rotations, scales);

    forestLOD1.initialize(1, 50.0f, 100.0f);
    forestLOD1.setupLOD(1, positions, rotations, scales);

    forestLOD2.initialize(2, 100.0f, 1000.0f);
    forestLOD2.setupLOD(2, positions, rotations, scales);

}

void Scene::initializeCars(int nCars) {
    if (!cars.initialize("../futuristic_emerald_isle/assets/imported_models/flying_car/scene.gltf")) {
        std::cerr << "Failed to initialize cars!" << std::endl;
        return;
    }

    std::vector<glm::vec3> cityPositions;
    for (const City& city : cities) {
        if (!city.buildings.empty()) {
            cityPositions.push_back(city.buildings[0].position);
        }
    }

    cars.generateCars(cityPositions, nCars);
}

void Scene::initializeBirds(int nBirds) {
    if (!birds.initialize("../futuristic_emerald_isle/assets/imported_models/lowpoly_seagull/scene.gltf")) {
        std::cerr << "Failed to initialize birds!" << std::endl;
        return;
    }
    float altitudeThreshold = 30.0f;
    birds.generateBirds(terrain, nBirds, altitudeThreshold);
}

void Scene::adjustLighting(float threshold, float darkFactor, const glm::vec3& cameraPosition) {
    glm::vec3 baseLightIntensity = 5.0f * (14.0f * glm::vec3(0.0f, 255.0f, 146.0f) +
                                           10.0f * glm::vec3(255.0f, 190.0f, 0.0f) +
                                           10.0f * glm::vec3(205.0f, 0.0f, 0.0f));

    if (cameraPosition.y < threshold) {
        float factor = glm::max(darkFactor, cameraPosition.y / threshold);
        lightIntensity = baseLightIntensity * factor;
    } else {
        lightIntensity = baseLightIntensity;
    }
}

void Scene::render(const glm::mat4& vp, const glm::vec3& cameraPosition, double deltaTime) {
    adjustLighting(20.0f, 0.01f, cameraPosition);

    // DEBUG AXIS
    // axis.render(vp);
    skybox.render(vp);
    terrain.render(vp, lightPosition, lightIntensity);
    cars.render(vp, cameraPosition, 200.0f, lightPosition, lightIntensity, deltaTime);
    birds.render(vp, cameraPosition, 300.0f, lightPosition, lightIntensity, deltaTime);
    forestLOD0.render(vp, cameraPosition, lightPosition, lightIntensity);
    forestLOD1.render(vp, cameraPosition, lightPosition, lightIntensity);
    forestLOD2.render(vp, cameraPosition, lightPosition, lightIntensity);

    for (City& city : cities) {
        city.render(vp, cameraPosition, 800.0f, lightPosition, lightIntensity);
    }
}

void Scene::cleanup() {
    skybox.cleanup();
    axis.cleanup();
    terrain.cleanup();
    birds.cleanup();
    forestLOD0.cleanup();
    forestLOD1.cleanup();
    forestLOD2.cleanup();

    for (City& city : cities) {
        city.cleanup();
    }
    cities.clear();
}


/*** LEGACY ***/

void Scene::initializeBuildingsOnTerrain(Terrain& terrain) {
    placeBuildingsOnHills(terrain, 1, 1);
}

void Scene::placeBuildingsOnHills(Terrain& terrain, int numHills, int buildingsPerHill) {
    std::random_device rd;
    //std::mt19937 gen(rd());
    //std::uniform_int_distribution<> dist(0, terrain.vertices.size() - 1);
    std::mt19937 gen(rd());
    std::vector<int> weights = {50, 50, 30, 20, 10, 5, 5};
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    std::set<int> selectedHills;

    while (selectedHills.size() < numHills) {
        selectedHills.insert(dist(gen));
    }

    for (int hillIndex : selectedHills) {
        int vFactor = dist(gen) + 1;
        glm::vec3 hillPosition = terrain.vertices[hillIndex];
        float spacing = 10.0f;

        for (int i = 0; i < buildingsPerHill; ++i) {
            float offsetX = ((rand() % 100) / 50.0f - 1.0f) * spacing;
            float offsetZ = ((rand() % 100) / 50.0f - 1.0f) * spacing;

            glm::vec3 buildingPosition = hillPosition + glm::vec3(offsetX, 0, offsetZ);

            Building b;
            float buildingHeight = (rand() % 5 + 1) * 10.0f;
            b.initialize(
                buildingPosition,
                glm::vec3(10.0f, buildingHeight, 10.0f),
                vFactor,
                rand() % 5
            );
            buildings.push_back(b);
        }
    }
}

void Scene::initializeCity(int cityRows, int cityCols, float buildingWidth, float buildingSpacing) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> weights = {50, 50, 20, 10, 5};
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    float spacingX = buildingWidth + buildingSpacing;
    float spacingZ = buildingWidth + buildingSpacing;

    for (int i = -(cityRows / 2); i < (cityRows / 2); ++i) {
        for (int j = -(cityCols / 2); j < (cityCols / 2); ++j) {

            int vFactor = dist(gen) + 1;
            float newHeight = buildingWidth * vFactor;

            float wFactor = 1 + (rand() % 3) / 10.0f;
            float newWidth = buildingWidth * wFactor;

            int textureNum = rand() % 5;

            Building b;
            glm::vec3 position(j * spacingX, newHeight, i * spacingZ);
            b.initialize(
                position,
                glm::vec3(newWidth, newHeight, buildingWidth),
                vFactor,
                textureNum
            );
            buildings.push_back(b);
        }
    }
}