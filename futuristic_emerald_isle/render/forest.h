#ifndef FOREST_H
#define FOREST_H

#include "tree.h"
#include <vector>

class Terrain;

class Forest {
public:
    Forest();
    ~Forest();

    bool initialize(int LOD, float minRenderRadius, float maxRenderRadius);
    void render(const glm::mat4 & vp, const glm::vec3 & cameraPosition, glm::vec3 lightPosition, glm::vec3 lightIntensity);
    void cleanup();

    void setupLOD(int LOD, const std::vector<glm::vec3>& positions, const std::vector<float>& rotations, const std::vector<float>& scales);
    void printCoords();

    int LOD;
    GLuint programID;
    const std::string LOD0_PATH = "../lab2/assets/imported_models/tree_lod0/LOD0.gltf";
    const std::string LOD1_PATH = "../lab2/assets/imported_models/tree_lod1/LOD1.gltf";
    const std::string LOD2_PATH = "../lab2/assets/imported_models/tree_lod2/LOD2.gltf";
    const std::string LOD0_SHADERS_PATH = "../lab2/shaders/tree_lod0/tree";
    const std::string LOD1_SHADERS_PATH = "../lab2/shaders/tree_lod1/tree";
    const std::string LOD2_SHADERS_PATH = "../lab2/shaders/tree_lod2/tree";
    float minRenderRadius, maxRenderRadius;

    tinygltf::Model model;
    std::vector<GLuint> bufferIDs;
    std::vector<GLuint> textureIDs;

private:
    std::vector<Tree> trees;
    GLuint instanceBuffer;
};

#endif