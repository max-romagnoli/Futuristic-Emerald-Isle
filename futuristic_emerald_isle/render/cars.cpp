#include "cars.h"
#include <random>
#include <iostream>
#include "shader.h"
#include <utils/utils.h>

#include "city.h"

Cars::Cars() {}
Cars::~Cars() {}

bool Cars::initialize(const std::string& modelPath) {
    Car::programID = LoadShadersFromFile("../futuristic_emerald_isle/shaders/car.vert", "../futuristic_emerald_isle/shaders/car.frag");
    if (Car::programID == 0) {
        std::cerr << "Failed to load car shaders!" << std::endl;
        return false;
    }

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&Car::model, &err, &warn, modelPath);
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

    for (const auto& bufferView : Car::model.bufferViews) {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(bufferView.target, bufferID);
        glBufferData(bufferView.target,
                     bufferView.byteLength,
                     &Car::model.buffers[bufferView.buffer].data[bufferView.byteOffset],
                     GL_STATIC_DRAW);
        Car::bufferIDs.push_back(bufferID);
    }

    for (const auto& texture : Car::model.textures) {
        const auto& image = Car::model.images[texture.source];

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D);

        Car::textureIDs.push_back(textureID);
    }

    for (const auto& material : Car::model.materials) {
        if (material.normalTexture.index >= 0) {
            const auto& textureInfo = material.normalTexture;
            GLuint normalTextureID = Car::textureIDs[textureInfo.index];

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalTextureID);

            std::cout << "Normal Map Loaded for Material: " << material.name
                      << " Texture ID: " << normalTextureID << std::endl;
        }
    }

    return true;
}

void Cars::generateCars(const std::vector<glm::vec3>& cityPositions, int nCars) {
    if (cityPositions.size() < 2) {
        std::cerr << "Not enough cities to generate car paths!" << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, cityPositions.size() - 1);

    for (int i = 0; i < nCars; ++i) {
        int startIndex = dist(gen);
        int endIndex;
        do {
            endIndex = dist(gen);
        } while (endIndex == startIndex);

        glm::vec3 start = cityPositions[startIndex];
        glm::vec3 end = cityPositions[endIndex];

        start.y = 50.0f;
        end.y = 50.0f;

        Car car;
        car.setPosition(start);
        car.setScale(glm::vec3(0.005f, 0.005f, 0.005f));
        car.setRotationTowards(end);
        car.setAnimation(start, end, 10.0f);

        cars.push_back(car);
    }
}

void Cars::render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius, glm::vec3 lightPosition, glm::vec3 lightIntensity, double deltaTime) {
    for (auto& car : cars) {
        car.update(deltaTime);
        float distanceToCamera = glm::distance(car.position, cameraPosition);

        if (distanceToCamera <= renderRadius) {
            glUseProgram(Car::programID);
            glUniform3fv(glGetUniformLocation(Car::programID, "lightPosition"), 1, &lightPosition[0]);
            glUniform3fv(glGetUniformLocation(Car::programID, "lightIntensity"), 1, &lightIntensity[0]);
            car.render(vp);
        }
    }
}

void Cars::cleanup() {
    for (auto& car : cars) {
        car.cleanup();
    }
    cars.clear();

    for (auto& bufferID : Car::bufferIDs) {
        glDeleteBuffers(1, &bufferID);
    }
    Car::bufferIDs.clear();

    for (auto& textureID : Car::textureIDs) {
        glDeleteTextures(1, &textureID);
    }
    Car::textureIDs.clear();

    if (Car::programID != 0) {
        glDeleteProgram(Car::programID);
        Car::programID = 0;
    }

    std::cout << "Cars resources cleaned up." << std::endl;
}