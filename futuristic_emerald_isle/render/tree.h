// #ifndef TREE_H
// #define TREE_H
// #include <glm/glm.hpp>
// #include <tiny_gltf.h>
// #include <string>
//
// #include "glad/gl.h"
//
// class Tree {
// public:
//     Tree();
//     ~Tree();
//
//     bool initialize(const std::string &filePath);
//
//     bool loadModel(const std::string& filePath);
//     void setPosition(const glm::vec3& position);
//     void render(const glm::mat4& vp);
//     void cleanup();
//
// private:
//     tinygltf::Model model;
//     glm::vec3 position;
//     glm::mat4 modelMatrix;
//     std::vector<GLuint> bufferIDs;
//     std::vector<GLuint> textureIDs;
//     GLuint programID;
//
//     void updateModelMatrix();
//     void drawNode(const tinygltf::Node& node, const tinygltf::Model& model, const glm::mat4& parentMatrix);
// };
//
// #endif

#ifndef TREE_H
#define TREE_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "glad/gl.h"
#include "utils/utils.h"

class Tree {
public:
    Tree();
    ~Tree();

    int LOD;

    void setPosition(const glm::vec3& position);
    void setScale(const glm::vec3& scale);
    void setRotation(const glm::vec3& rotation);  // TODO:
    void render(const glm::mat4& vp);

    void cleanup();

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;

    void updateModelMatrix();

    GLuint programID;
    tinygltf::Model* model;
    std::vector<GLuint>* bufferIDs;
    std::vector<GLuint>* textureIDs;

    friend class Forest;
};

#endif