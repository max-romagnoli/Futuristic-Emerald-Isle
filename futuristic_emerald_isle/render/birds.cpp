#include "birds.h"
#include "shader.h"
#include <iostream>
#include <bits/random.h>
#include "terrain.h"


Birds::Birds() {}
Birds::~Birds() {}

bool Birds::initialize(const std::string& modelPath) {
    Bird::programID = LoadShadersFromFile("../futuristic_emerald_isle/shaders/bird.vert", "../futuristic_emerald_isle/shaders/bird.frag");
    if (Bird::programID == 0) {
        std::cerr << "Failed to load bird shaders!" << std::endl;
        return false;
    }

    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool res = loader.LoadASCIIFromFile(&Bird::model, &err, &warn, modelPath);
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res) {
        std::cout << "Failed to load glTF: " << modelPath << std::endl;
        return false;
    }

    std::cout << "Loaded glTF: " << modelPath << std::endl;

    for (const auto& bufferView : Bird::model.bufferViews) {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(bufferView.target, bufferID);
        glBufferData(bufferView.target,
                     bufferView.byteLength,
                     &Bird::model.buffers[bufferView.buffer].data[bufferView.byteOffset],
                     GL_STATIC_DRAW);
        Bird::bufferIDs.push_back(bufferID);
    }

    for (const auto& texture : Bird::model.textures) {
        const auto& image = Bird::model.images[texture.source];

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D);

        Bird::textureIDs.push_back(textureID);
    }

    for (const auto& material : Bird::model.materials) {
        if (material.normalTexture.index >= 0) {
            const auto& textureInfo = material.normalTexture;
            GLuint normalTextureID = Bird::textureIDs[textureInfo.index];

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalTextureID);

            // Log for debugging
            std::cout << "Normal Map Loaded for Material: " << material.name
                      << " Texture ID: " << normalTextureID << std::endl;
        }
    }

    return true;
}

void Birds::generateBirds(Terrain& terrain, int nBirds, float altitudeThreshold) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> radiusDist(40.0f, 70.0f);
    std::uniform_real_distribution<float> speedDist(50.0f, 150.0f);
    std::uniform_int_distribution<int> flockSizeDist(1, 5);

    std::vector<glm::vec3> hilltops = terrain.getHighestPoints(nBirds);

    for (const auto& hilltop : hilltops) {
        int flockSize = flockSizeDist(gen);
        float radius = radiusDist(gen);

        for (int i = 0; i < flockSize; ++i) {
            Bird bird;
            bird.setPosition(hilltop);
            bird.setScale(glm::vec3(0.3f));

            float speed = speedDist(gen);
            radius = radius + i;

            glm::vec3 center = hilltop;
            //center.y -= 10.0f;
            bird.setCircularPath(center, radius, speed);

            birds.push_back(bird);
        }
    }
}

void Birds::render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius, glm::vec3 lightPosition,
        glm::vec3 lightIntensity, double deltaTime)  {
    for (auto& bird : birds) {
        bird.update(deltaTime);
        float distanceToCamera = glm::distance(bird.position, cameraPosition);

        if (distanceToCamera <= renderRadius) {
            glUseProgram(Bird::programID);
            glUniform3fv(glGetUniformLocation(Bird::programID, "lightPosition"), 1, &lightPosition[0]);
            glUniform3fv(glGetUniformLocation(Bird::programID, "lightIntensity"), 1, &lightIntensity[0]);
            bird.render(vp);
        }
    }
}

void Birds::cleanup() {
    for (auto& bird : birds) {
        bird.cleanup();
    }
    birds.clear();

    for (auto& bufferID : Bird::bufferIDs) {
        glDeleteBuffers(1, &bufferID);
    }
    Bird::bufferIDs.clear();

    for (auto& textureID : Bird::textureIDs) {
        glDeleteTextures(1, &textureID);
    }
    Bird::textureIDs.clear();

    if (Bird::programID != 0) {
        glDeleteProgram(Bird::programID);
        Bird::programID = 0;
    }

    std::cout << "Birds resources cleaned up." << std::endl;
}