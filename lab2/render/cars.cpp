#include "cars.h"
#include <random>
#include <iostream>
#include "shader.h"
#include <utils/utils.h>

Cars::Cars() {}
Cars::~Cars() {}

bool Cars::initialize(const std::string& modelPath) {
    Car::programID = LoadShadersFromFile("../lab2/shaders/car.vert", "../lab2/shaders/car.frag");
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

            // Log for debugging
            std::cout << "Normal Map Loaded for Material: " << material.name
                      << " Texture ID: " << normalTextureID << std::endl;
        }
    }

    return true;
}

// TODO:
void Cars::generateCars() {
    Car car;
    car.setPosition(glm::vec3(0.0f, 50.0f, 0.0f));
    car.setScale(glm::vec3(0.005f, 0.005f, 0.005f));
    car.setRotation(glm::vec3(90.0f, 180.0f, 0.0f));
    car.setAnimation(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(100.0f, 50.0f, 100.0f), 30.0f);
    cars.push_back(car);
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