#include "Forest.h"
#include <random>
#include <iostream>
#include "terrain.h"
#include "shader.h"

Forest::Forest() {}

Forest::~Forest() {}

bool Forest::initialize(const std::string& modelPath) {
    Tree::programID = LoadShadersFromFile("../lab2/shaders/tree.vert", "../lab2/shaders/tree.frag");
    if (Tree::programID == 0) {
        std::cerr << "Failed to load tree shaders!" << std::endl;
        return false;
    }

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&Tree::model, &err, &warn, modelPath);
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

    for (const auto& bufferView : Tree::model.bufferViews) {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(bufferView.target, bufferID);
        glBufferData(bufferView.target,
                     bufferView.byteLength,
                     &Tree::model.buffers[bufferView.buffer].data[bufferView.byteOffset],
                     GL_STATIC_DRAW);
        Tree::bufferIDs.push_back(bufferID);
    }

    for (const auto& texture : Tree::model.textures) {
        const auto& image = Tree::model.images[texture.source];

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D);

        Tree::textureIDs.push_back(textureID);
    }

    return true;
}

void Forest::generateTrees(const Terrain& terrain, int count, float altitudeThreshold) {
    std::random_device rd;
    std::mt19937 gen(rd());

    float halfWidth = terrain.getWidth() / 2.0f;
    float halfDepth = terrain.getDepth() / 2.0f;

    std::uniform_real_distribution<float> xDist(-halfWidth, halfWidth);
    std::uniform_real_distribution<float> zDist(-halfDepth, halfDepth);

    int generated = 0;
    while (generated < count) {
        float x = xDist(gen);
        float z = zDist(gen);
        float altitude = terrain.getHeightAt(x, z);

        if (altitude <= altitudeThreshold) {
            Tree tree;
            tree.setPosition(glm::vec3(x, altitude, z));
            trees.push_back(tree);
            generated++;
        }
    }
}

void Forest::render(const glm::mat4& vp) {
    for (auto& tree : trees) {
        tree.render(vp);
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

    for (auto& bufferID : Tree::bufferIDs) {
        glDeleteBuffers(1, &bufferID);
    }
    Tree::bufferIDs.clear();

    for (auto& textureID : Tree::textureIDs) {
        glDeleteTextures(1, &textureID);
    }
    Tree::textureIDs.clear();

    if (Tree::programID != 0) {
        glDeleteProgram(Tree::programID);
        Tree::programID = 0;
    }

    std::cout << "Forest resources cleaned up." << std::endl;
}