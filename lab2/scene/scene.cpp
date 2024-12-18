#include <scene/scene.h>
#include <random>
#include <set>

Scene::~Scene() {
    cleanup();
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

            Building b;
            b.initialize(
                position,
                glm::vec3(newWidth, newHeight, buildingWidth), // Size
                vFactor,
                textureNum
            );
            buildings.push_back(b);
        }
    }
}

void Scene::initializeCitiesOnHills(int nCities) {
    std::vector<glm::vec3> highestPoints = terrain.getHighestPoints(nCities);
    for (const auto& point : highestPoints) {
        initializeCityOnHill(point, 4, 4, 2.0f, 4.0f);
    }
}

void Scene::render(const glm::mat4& vp) {
    skybox.render(vp);
    axis.render(vp);
    terrain.render(vp);

    for (Building& b : buildings) {
        b.render(vp);
    }
}

void Scene::cleanup() {
    skybox.cleanup();
    axis.cleanup();
    terrain.cleanup();
    for (Building& b : buildings) {
        b.cleanup();
    }
}


/*** LEGACY ***/

void Scene::initializeBuildingsOnTerrain(Terrain& terrain) {
    // Place buildings on 20 hills, each with an array of 30 buildings
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

    // Randomly select unique hills
    while (selectedHills.size() < numHills) {
        selectedHills.insert(dist(gen));
    }

    for (int hillIndex : selectedHills) {
        int vFactor = dist(gen) + 1;
        glm::vec3 hillPosition = terrain.vertices[hillIndex];
        float spacing = 10.0f; // Spacing between buildings in an array

        for (int i = 0; i < buildingsPerHill; ++i) {
            // Slightly randomize building positions around the hill
            float offsetX = ((rand() % 100) / 50.0f - 1.0f) * spacing;
            float offsetZ = ((rand() % 100) / 50.0f - 1.0f) * spacing;

            glm::vec3 buildingPosition = hillPosition + glm::vec3(offsetX, 0, offsetZ);

            // Create and initialize a building
            Building b;
            float buildingHeight = (rand() % 5 + 1) * 10.0f; // Randomize height
            b.initialize(
                buildingPosition,
                glm::vec3(10.0f, buildingHeight, 10.0f), // Size
                vFactor,                                // Variant
                rand() % 5                              // Texture
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