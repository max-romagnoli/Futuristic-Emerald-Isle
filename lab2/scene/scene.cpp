#include <scene/scene.h>
#include <random>

Scene::~Scene() {
    cleanup();
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
            // Generate height factor
            int vFactor = dist(gen) + 1;
            float newHeight = buildingWidth * vFactor;

            // Generate width factor
            float wFactor = 1 + (rand() % 3) / 10.0f;
            float newWidth = buildingWidth * wFactor;

            // Pick texture
            int textureNum = rand() % 5;

            // Create and initialize a building
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

void Scene::initializeAxis() {
    axis.initialize();
}

void Scene::initializeSkybox(glm::vec3 position, glm::vec3 scale) {
    skybox.initialize(position, scale);
}

void Scene::render(const glm::mat4& vp) {
    skybox.render(vp);
    axis.render(vp);

    for (Building& b : buildings) {
        b.render(vp);
    }
}

void Scene::cleanup() {
    skybox.cleanup();
    axis.cleanup();
    for (Building& b : buildings) {
        b.cleanup();
    }
}
