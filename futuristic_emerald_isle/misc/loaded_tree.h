#ifndef LOADED_TREE_H
#define LOADED_TREE_H

#include <glm/glm.hpp>
#include <tiny_gltf.h>
#include <vector>
#include <glad/gl.h>

class LoadedTree {
public:
    LoadedTree();
    ~LoadedTree();

    void initialize(const ::std::string &filePath, const glm::vec3 & position, const glm::vec3 & scale, const std::string &texturePath);
    void render(const glm::mat4& vp);
    void cleanup();

private:
    glm::vec3 position;
    glm::vec3 scale;

    GLuint vao, vbo, ebo; // OpenGL buffers
    GLuint programID;
    GLuint textureID;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void loadModel(const std::string& filePath);
};

#endif