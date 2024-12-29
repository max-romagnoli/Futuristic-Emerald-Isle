#include "tree.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "shader.h"

Tree::Tree() : position(0.0f), modelMatrix(1.0f) {}

Tree::~Tree() {}

void Tree::setPosition(const glm::vec3& position) {
    this->position = position;
    updateModelMatrix();
}

void Tree::setScale(const glm::vec3& scale) {
    this->scale = scale;
    updateModelMatrix();
}

void Tree::setRotation(const glm::vec3& rotation) {
    this->rotation = rotation;
    updateModelMatrix();
}

void Tree::updateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Tree::render(const glm::mat4& vp) {

    glUseProgram(programID);

    glm::mat4 mvp = vp * modelMatrix;

    GLuint mvpLocation = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

    for (const auto& node : model->nodes) {
        glm::mat4 nodeMatrix = mvp;

        if (node.mesh >= 0) {
            const auto& mesh = model->meshes[node.mesh];

            for (const auto& primitive : mesh.primitives) {
                if (primitive.material >= 0) {
                    const auto& material = model->materials[primitive.material];
                    if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                        const auto& textureInfo = material.pbrMetallicRoughness.baseColorTexture;

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, (*textureIDs)[textureInfo.index]);

                        GLuint textureLocation = glGetUniformLocation(programID, "textureSampler");
                        glUniform1i(textureLocation, 0);
                    }
                    if (material.normalTexture.index >= 0) {
                        const auto& textureInfo = material.normalTexture;

                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, (*textureIDs)[textureInfo.index]);

                        GLuint normalMapLocation = glGetUniformLocation(programID, "normalMapSampler");
                        glUniform1i(normalMapLocation, 1);
                    }
                }

                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                    const int accessorIdx = primitive.attributes.at("POSITION");
                    const auto& accessor = model->accessors[accessorIdx];
                    const auto& bufferView = model->bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ARRAY_BUFFER, (*bufferIDs)[accessor.bufferView]);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, bufferView.byteStride,
                                          reinterpret_cast<void*>(accessor.byteOffset));
                    glEnableVertexAttribArray(0);
                }

                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                    const int accessorIdx = primitive.attributes.at("TEXCOORD_0");
                    const auto& accessor = model->accessors[accessorIdx];
                    const auto& bufferView = model->bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ARRAY_BUFFER, (*bufferIDs)[accessor.bufferView]);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, bufferView.byteStride,
                                          reinterpret_cast<void*>(accessor.byteOffset));
                    glEnableVertexAttribArray(1);
                }

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const int accessorIdx = primitive.attributes.at("NORMAL");
                    const auto& accessor = model->accessors[accessorIdx];
                    const auto& bufferView = model->bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ARRAY_BUFFER, (*bufferIDs)[accessor.bufferView]);
                    glVertexAttribPointer(2,
                                          accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          bufferView.byteStride,
                                          reinterpret_cast<void*>(accessor.byteOffset));
                    glEnableVertexAttribArray(2);
                }

                if (primitive.indices >= 0) {
                    const auto& accessor = model->accessors[primitive.indices];
                    const auto& bufferView = model->bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*bufferIDs)[accessor.bufferView]);
                    glDrawElements(GL_TRIANGLES, accessor.count, accessor.componentType,
                                   reinterpret_cast<void*>(accessor.byteOffset));
                }
            }

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }
    }

}


void Tree::cleanup() {
}