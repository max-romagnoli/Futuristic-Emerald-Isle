#include "terrain.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

#include "shader.h"
#include <utils/load_textures.h>

#include "Skybox.h"
#include "Skybox.h"

void Terrain::initialize(int width, int depth, float maxHeight, float repeatFactor) {
    this->width = width;
    this->depth = depth;
    this->maxHeight = maxHeight;

    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    normals.resize((width + 1) * (depth + 1), glm::vec3(0.0f));

    for (int z = 0; z <= depth; ++z) {
        for (int x = 0; x <= width; ++x) {
            float worldX = x - halfWidth;
            float worldZ = z - halfDepth;

            float height = sin(worldX * 0.03f) * cos(worldZ * 0.03f) * maxHeight;

            vertices.emplace_back(glm::vec3(worldX, height, worldZ));

            uvs.emplace_back(glm::vec2(x / (float)width, z / (float)depth) * repeatFactor);
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

            glm::vec3 v0 = vertices[topLeft];
            glm::vec3 v1 = vertices[bottomLeft];
            glm::vec3 v2 = vertices[topRight];
            glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            glm::vec3 v3 = vertices[topRight];
            glm::vec3 v4 = vertices[bottomLeft];
            glm::vec3 v5 = vertices[bottomRight];
            glm::vec3 normal2 = glm::normalize(glm::cross(v4 - v3, v5 - v3));

            normals[topLeft] += normal1;
            normals[bottomLeft] += normal1 + normal2;
            normals[topRight] += normal1 + normal2;
            normals[bottomRight] += normal2;
        }
    }

    for (auto& normal : normals) {
        if (glm::length(normal) > 0.0f) {
            normal = glm::normalize(normal);
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

    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../lab2/shaders/terrain.vert", "../lab2/shaders/terrain.frag");

    // Uniform locations
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");

    // Texture
    textureID = LoadTextureTileBox("../lab2/assets/textures/grass.jpg");
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
}

void Terrain::render(const glm::mat4& vp, glm::vec3 lightPosition, glm::vec3 lightIntensity) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textureID);
    //glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);

    // MVP matrix
    glm::mat4 mvp = vp * model;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Lighting
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

    // Texture sampler
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Terrain::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &normalBufferID);
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


int Terrain::getWidth() const {
    return width;
}

int Terrain::getDepth() const {
    return depth;
}

float Terrain::getHeightAt(float x, float z) const {
    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    // Map world coordinates to grid coordinates
    float gridX = x + halfWidth;
    float gridZ = z + halfDepth;

    if (gridX < 0 || gridX >= width || gridZ < 0 || gridZ >= depth) {
        std::cerr << "Out-of-bounds coordinates: (" << x << ", " << z << ")" << std::endl;
        return 0.0f;
    }

    int x0 = static_cast<int>(std::floor(gridX));
    int z0 = static_cast<int>(std::floor(gridZ));
    int x1 = std::min(x0 + 1, width);
    int z1 = std::min(z0 + 1, depth);

    float h00 = vertices[z0 * (width + 1) + x0].y;
    float h10 = vertices[z0 * (width + 1) + x1].y;
    float h01 = vertices[z1 * (width + 1) + x0].y;
    float h11 = vertices[z1 * (width + 1) + x1].y;

    float tx = gridX - x0;
    float tz = gridZ - z0;

    float h0 = h00 * (1 - tx) + h10 * tx;
    float h1 = h01 * (1 - tx) + h11 * tx;

    return h0 * (1 - tz) + h1 * tz;
}
