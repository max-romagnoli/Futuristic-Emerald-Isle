#include "Scene.h"
#include "Skybox.h"
#include "Building.h"

void Scene::initialize() {
    // Initialize buildings using procedural generation
    for (int i = -5; i <= 5; ++i) {
        for (int j = -5; j <= 5; ++j) {
            Building building(glm::vec3(i * 20.0f, 0, j * 20.0f), glm::vec3(10.0f, 50.0f, 10.0f));
            building.initialize();
            buildings.push_back(building);
        }
    }

    // Load skybox
    //skyboxVAO = createSkyboxVAO();
    //skyboxTexture = loadSkyboxTextures("resources/textures/skybox/");
}

void Scene::draw(const glm::mat4& vp) {
    // Draw skybox
    drawSkybox(skyboxVAO, skyboxTexture, vp);

    // Draw buildings
    for (const Building& building : buildings) {
        building.draw(vp);
    }
}

void Scene::cleanup() {
    for (Building& building : buildings) {
        building.cleanup();
    }
    glDeleteTextures(1, &skyboxTexture);
}