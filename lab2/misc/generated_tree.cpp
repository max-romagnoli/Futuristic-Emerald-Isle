#include "generated_tree.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <render/shader.h>

GeneratedTree::GeneratedTree() : vao(0), vbo(0), ebo(0), programID(0) {}

GeneratedTree::~GeneratedTree() {
    cleanup();
}

void GeneratedTree::initialize(const glm::vec3& position, const glm::vec3& scale) {
    this->position = position;
    this->scale = scale;

    // Load shaders
    programID = LoadShadersFromFile("../lab2/shaders/tree.vert", "../lab2/shaders/tree.frag");

    // Generate tree geometry
    generateTreeGeometry();

    // OpenGL setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // UV
}

void GeneratedTree::generateTreeGeometry() {
    const int slices = 16;

    // Generate trunk (cylinder)
    for (int i = 0; i <= slices; ++i) {
        float angle = i * 2.0f * M_PI / slices;
        float x = cos(angle);
        float z = sin(angle);

        vertices.push_back(x); vertices.push_back(0.0f); vertices.push_back(z); // Base
        vertices.push_back(x); vertices.push_back(1.0f); vertices.push_back(z); // Top
    }

    // Generate indices for trunk
    for (int i = 0; i < slices; ++i) {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
        indices.push_back((i * 2 + 2) % (2 * slices));

        indices.push_back(i * 2 + 1);
        indices.push_back((i * 2 + 3) % (2 * slices));
        indices.push_back((i * 2 + 2) % (2 * slices));
    }

    // Add a sphere for the canopy (simplified as a single vertex for now)
    vertices.push_back(0.0f); vertices.push_back(1.5f); vertices.push_back(0.0f);
}

void GeneratedTree::render(const glm::mat4& vp) {
    glUseProgram(programID);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    glm::mat4 mvp = vp * model;
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void GeneratedTree::cleanup() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}