#include "loaded_tree.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>
#include <utils/load_textures.h>

LoadedTree::LoadedTree() : vao(0), vbo(0), ebo(0), programID(0) {}

LoadedTree::~LoadedTree() {
    cleanup();
}

void LoadedTree::initialize(const std::string& filePath, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath) {
    this->position = position;
    this->scale = scale;

    // Load shaders
    programID = LoadShadersFromFile("../lab2/shaders/tree.vert", "../lab2/shaders/tree.frag");

    // Load model
    loadModel(filePath);

    // Load texture
    textureID = LoadTextureTileBox(texturePath.c_str());
    if (!textureID) {
        std::cerr << "Failed to load texture for tree: " << texturePath << std::endl;
        return;
    }

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

    std::cout << "VAO: " << vao << ", VBO: " << vbo << std::endl;
}

void LoadedTree::loadModel(const std::string& filePath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool success = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    if (!warn.empty()) std::cerr << "Warn: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Err: " << err << std::endl;
    if (!success) throw std::runtime_error("Failed to load GLTF model");

    // Extract the first mesh and its data
    const auto& mesh = model.meshes[0];
    const auto& primitive = mesh.primitives[0];
    const auto& accessor = model.accessors[primitive.attributes.begin()->second];
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];

    vertices.assign(buffer.data.begin(), buffer.data.end());

    const auto& indexAccessor = model.accessors[primitive.indices];
    const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
    indices.assign(buffer.data.begin() + indexBufferView.byteOffset,
                   buffer.data.begin() + indexBufferView.byteOffset + indexBufferView.byteLength);

    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Indexes: " << indices.size() << std::endl;
}

void LoadedTree::render(const glm::mat4& vp) {
    glUseProgram(programID);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    glm::mat4 mvp = vp * model;
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &mvp[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void LoadedTree::cleanup() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
    glDeleteTextures(1, &textureID);
}