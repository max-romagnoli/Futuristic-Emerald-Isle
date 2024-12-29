#include "Forest.h"
#include <random>
#include <iostream>
#include "terrain.h"
#include "shader.h"
#include <utils/utils.h>

Forest::Forest() {}

Forest::~Forest() {}

bool Forest::initialize(int LOD, float minRenderRadius, float maxRenderRadius) {
    this -> LOD = LOD;
    this -> minRenderRadius = minRenderRadius;
    this -> maxRenderRadius = maxRenderRadius;

    std::string modelPath;
    std::string shadersPath;

    if (LOD == 0) {
        modelPath = LOD0_PATH;
        shadersPath = LOD0_SHADERS_PATH;
    } else if (LOD == 1) {
        modelPath = LOD1_PATH;
        shadersPath = LOD1_SHADERS_PATH;
    } else if (LOD == 2) {
        modelPath = LOD2_PATH;
        shadersPath = LOD2_SHADERS_PATH;
    }

    std::string vertexShaderPath = shadersPath + ".vert";
    std::string fragmentShaderPath = shadersPath + ".frag";

    GLuint programID = LoadShadersFromFile(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if (programID == 0) {
        std::cerr << "Failed to load tree shaders!" << std::endl;
        return false;
    }

    this->programID = programID;

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&this->model, &err, &warn, modelPath);
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

    for (const auto& bufferView : this->model.bufferViews) {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(bufferView.target, bufferID);
        glBufferData(bufferView.target,
                     bufferView.byteLength,
                     &this->model.buffers[bufferView.buffer].data[bufferView.byteOffset],
                     GL_STATIC_DRAW);
        this->bufferIDs.push_back(bufferID);
    }

    for (const auto& texture : this->model.textures) {
        const auto& image = this->model.images[texture.source];

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D);

        this->textureIDs.push_back(textureID);
    }

    for (const auto& material : this->model.materials) {
        if (material.normalTexture.index >= 0) {
            const auto& textureInfo = material.normalTexture;
            GLuint normalTextureID = this->textureIDs[textureInfo.index];

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalTextureID);

            // Log for debugging
            std::cout << "Normal Map Loaded for Material: " << material.name
                      << " Texture ID: " << normalTextureID << std::endl;
        }
    }

    return true;
}

void Forest::setupLOD(int LOD, const std::vector<glm::vec3>& positions, const std::vector<float>& rotations, const std::vector<float>& scales) {
    if (positions.size() == rotations.size() && positions.size() == scales.size()) {
        for (int i = 0; i < positions.size(); i++) {
            Tree tree;
            tree.LOD = LOD;
            tree.programID = programID;
            tree.model = &model;
            tree.bufferIDs = &bufferIDs;
            tree.textureIDs = &textureIDs;
            tree.setPosition(positions[i]);
            //tree.setRotation(glm::vec3(0, rotations[i], 0));
            tree.setScale(glm::vec3(scales[i], scales[i], scales[i]));
            trees.push_back(tree);
        }
    }
}

void Forest::render(const glm::mat4& vp, const glm::vec3& cameraPosition, glm::vec3 lightPosition, glm::vec3 lightIntensity) {
    for (auto& tree : trees) {
        float distanceToCamera = glm::distance(tree.position, cameraPosition);

        if (distanceToCamera >= minRenderRadius && distanceToCamera <= maxRenderRadius) {
            glUseProgram(this->programID);
            glUniform3fv(glGetUniformLocation(this->programID, "lightPosition"), 1, &lightPosition[0]);
            glUniform3fv(glGetUniformLocation(this->programID, "lightIntensity"), 1, &lightIntensity[0]);
            tree.render(vp);
        }
    }
}

void Forest::printCoords() {
    for (auto& tree : trees) {
        std::cout << tree.position.x << ", " << tree.position.y << std::endl;
    }
}

void Forest::cleanup() {
    for (auto& tree : trees) {
        tree.cleanup();
    }
    trees.clear();

    for (auto& bufferID : bufferIDs) {
        glDeleteBuffers(1, &bufferID);
    }
    bufferIDs.clear();

    for (auto& textureID : textureIDs) {
        glDeleteTextures(1, &textureID);
    }
    textureIDs.clear();

    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }

    std::cout << "Forest resources cleaned up." << std::endl;
}