#include "Tree.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <tinygltf-2.9.3/stb_image.h>

#include "shader.h"

Tree::Tree() : position(0.0f) {}

Tree::~Tree() {}

bool Tree::initialize(const std::string& filePath) {
    // TODO: should load shaders only once before all trees
    programID = LoadShadersFromFile("../lab2/shaders/tree.vert", "../lab2/shaders/tree.frag");
    if (programID == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
    }

    if (!loadModel(filePath)) {
        std::cerr << "Failed to load tree model" << std::endl;
        return false;
    }
    return true;
}

bool Tree::loadModel(const std::string& filePath) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res) {
        std::cout << "Failed to load glTF: " << filePath << std::endl;
        return false;
    }

    std::cout << "Loaded glTF: " << filePath << std::endl;

    for (const auto& bufferView : model.bufferViews) {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(bufferView.target, bufferID);
        glBufferData(bufferView.target,
                     bufferView.byteLength,
                     &model.buffers[bufferView.buffer].data[bufferView.byteOffset],
                     GL_STATIC_DRAW);
        bufferIDs.push_back(bufferID);
    }

    for (const auto& texture : model.textures) {
        const auto& image = model.images[texture.source];

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Upload image data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D);

        textureIDs.push_back(textureID);
    }

    return true;
}

void Tree::setPosition(const glm::vec3& position) {
    this->position = position;
    updateModelMatrix();
}

void Tree::updateModelMatrix() {
    modelMatrix = glm::translate(glm::mat4(1.0f), position);
}

void Tree::render(const glm::mat4& vp) {
    glUseProgram(programID);

    glm::mat4 mvp = vp * modelMatrix;

    for (const auto& node : model.nodes) {
        drawNode(node, model, mvp);
    }
}

void Tree::drawNode(const tinygltf::Node& node, const tinygltf::Model& model, const glm::mat4& parentMatrix) {
    glm::mat4 nodeMatrix = parentMatrix;

    if (node.mesh >= 0) {
        const auto& mesh = model.meshes[node.mesh];

        GLuint mvpLocation = glGetUniformLocation(programID, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &nodeMatrix[0][0]);

        for (const auto& primitive : mesh.primitives) {
            if (primitive.material >= 0) {
                const auto& material = model.materials[primitive.material];
                if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                    const auto& textureInfo = material.pbrMetallicRoughness.baseColorTexture;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, textureIDs[textureInfo.index]);

                    GLuint textureLocation = glGetUniformLocation(programID, "textureSampler");
                    glUniform1i(textureLocation, 0);
                }
            }

            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const int accessorIdx = primitive.attributes.at("POSITION");
                const auto& accessor = model.accessors[accessorIdx];
                const auto& bufferView = model.bufferViews[accessor.bufferView];

                glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[accessor.bufferView]);
                glVertexAttribPointer(0,
                                      accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      bufferView.byteStride,
                                      reinterpret_cast<void*>(accessor.byteOffset));
                glEnableVertexAttribArray(0);
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const int accessorIdx = primitive.attributes.at("TEXCOORD_0");
                const auto& accessor = model.accessors[accessorIdx];
                const auto& bufferView = model.bufferViews[accessor.bufferView];

                glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[accessor.bufferView]);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, bufferView.byteStride,
                                      reinterpret_cast<void*>(accessor.byteOffset));
                glEnableVertexAttribArray(1);
            }

            if (primitive.indices >= 0) {
                const auto& accessor = model.accessors[primitive.indices];
                const auto& bufferView = model.bufferViews[accessor.bufferView];

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[accessor.bufferView]);
                glDrawElements(GL_TRIANGLES,
                               accessor.count,
                               accessor.componentType,
                               reinterpret_cast<void*>(accessor.byteOffset));
            }
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    for (const auto& childIndex : node.children) {
        drawNode(model.nodes[childIndex], model, nodeMatrix);
    }
}

void Tree::cleanup() {
    for (auto bufferID : bufferIDs) {
        glDeleteBuffers(1, &bufferID);
    }
    bufferIDs.clear();

    for (auto textureID : textureIDs) {
        glDeleteBuffers(1, &textureID);
    }
    textureIDs.clear();
}