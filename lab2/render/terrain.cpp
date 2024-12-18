#include "terrain.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

#include "shader.h"
#include <utils/load_textures.h>

#include "Skybox.h"
#include "Skybox.h"

void Terrain::initialize(int width, int depth, float maxHeight) {
    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    for (int z = 0; z <= depth; ++z) {
        for (int x = 0; x <= width; ++x) {
            float worldX = x - halfWidth;
            float worldZ = z - halfDepth;

            float height = sin(worldX * 0.03f) * cos(worldZ * 0.03f) * maxHeight;

            vertices.emplace_back(glm::vec3(worldX, height, worldZ));
            uvs.emplace_back(glm::vec2(x / (float)width, z / (float)depth));
        }
    }

    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            GLuint topLeft = z * (width + 1) + x;
            GLuint topRight = topLeft + 1;
            GLuint bottomLeft = (z + 1) * (width + 1) + x;
            GLuint bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // OpenGL setup
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    textureID = LoadTextureTileBox("../lab2/assets/textures/test_terrain.jpg");

    programID = LoadShadersFromFile("../lab2/shaders/terrain.vert", "../lab2/shaders/terrain.frag");
}

void Terrain::render(const glm::mat4& vp) {
    glUseProgram(programID);

    glBindVertexArray(vertexArrayID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = vp * model;
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(0); // Positions
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1); // UVs
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Terrain::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(programID);
}

glm::vec3 Terrain::getCenterHill() {
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    float minDistance = std::numeric_limits<float>::max();
    glm::vec3 closestVertex;

    for (const auto& vertex : vertices) {
        float distance = glm::length(glm::vec2(vertex.x, vertex.z) - glm::vec2(center.x, center.z));

        if (distance < minDistance) {
            minDistance = distance;
            closestVertex = vertex;
        }
    }

    return closestVertex;
}

std::vector<glm::vec3> Terrain::getHighestPoints(int n) {
    std::vector<glm::vec3> sortedVertices = vertices;

    std::sort(sortedVertices.begin(), sortedVertices.end(),
              [](const glm::vec3& a, const glm::vec3& b) {
                  return a.y > b.y;
              });

    if (sortedVertices.size() > n) {
        sortedVertices.resize(n);
    }

    return sortedVertices;
}
