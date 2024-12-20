#ifndef GENERATED_TREE_H
#define GENERATED_TREE_H

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <vector>

class GeneratedTree {
public:
    GeneratedTree();
    ~GeneratedTree();

    void initialize(const glm::vec3& position, const glm::vec3& scale);
    void render(const glm::mat4& vp);
    void cleanup();

private:
    glm::vec3 position;
    glm::vec3 scale;

    GLuint vao, vbo, ebo;
    GLuint programID;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateTreeGeometry();
};

#endif