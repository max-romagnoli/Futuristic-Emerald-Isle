#ifndef TREE_H
#define TREE_H
#include <glm/glm.hpp>
#include <tiny_gltf.h>
#include <string>

#include "glad/gl.h"

class Tree {
public:
    Tree();
    ~Tree();

    bool initialize(const std::string &filePath);

    bool loadModel(const std::string& filePath);
    void setPosition(const glm::vec3& position);
    void render(const glm::mat4& vp);
    void cleanup();

private:
    tinygltf::Model model;
    glm::vec3 position;
    glm::mat4 modelMatrix;
    std::vector<GLuint> bufferIDs;
    std::vector<GLuint> textureIDs;
    GLuint programID;

    void updateModelMatrix();
    void drawNode(const tinygltf::Node& node, const tinygltf::Model& model, const glm::mat4& parentMatrix);
};

#endif