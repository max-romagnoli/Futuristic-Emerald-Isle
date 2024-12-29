#include "bird.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

GLuint Bird::programID = 0;
tinygltf::Model Bird::model;
std::vector<GLuint> Bird::bufferIDs;
std::vector<GLuint> Bird::textureIDs;

Bird::Bird() :
    position(glm::vec3(0.0f, 50.0f, 0.0f)),
    rotation(glm::vec3(0.0f)),
    scale(glm::vec3(5.0f)),
    modelMatrix(1.0f),
    currentAnimationTime(0.0f),
    circularPathCenter(glm::vec3(0.0f)),
    circularPathRadius(10.0f),
    circularPathSpeed(1.0f),
    circularPathAngle(0.0f) {
        updateModelMatrix();
}

Bird::~Bird() {}

void Bird::setPosition(const glm::vec3& position) {
    this->position = position;
    updateModelMatrix();
}

void Bird::setRotation(const glm::vec3& rotation) {
    this->rotation = rotation;
    updateModelMatrix();
}

void Bird::setScale(const glm::vec3& scale) {
    this->scale = scale;
    updateModelMatrix();
}

void Bird::setRotationTowards(const glm::vec3& targetPosition) {
    glm::vec3 direction = glm::normalize(targetPosition - position);
    float yaw = glm::degrees(atan2(direction.x, direction.z));
    rotation.z = yaw + 180.0f;
    updateModelMatrix();
}

void Bird::setCircularPath(const glm::vec3& center, float radius, float speed) {
    circularPathCenter = center;
    circularPathRadius = radius;
    circularPathSpeed = speed;
    circularPathAngle = 0.0f; // Start at the initial angle
}

void Bird::update(double deltaTime) {
    currentAnimationTime += deltaTime;

    circularPathAngle += circularPathSpeed * deltaTime;
    if (circularPathAngle > 360.0f) {
        circularPathAngle -= 360.0f;
    }

    float posX = circularPathCenter.x + circularPathRadius * cos(glm::radians(circularPathAngle));
    float posZ = circularPathCenter.z + circularPathRadius * sin(glm::radians(circularPathAngle));
    float posY = circularPathCenter.y;
    setPosition(glm::vec3(posX, posY, posZ));

    const tinygltf::Animation& animation = model.animations[0];
    float maxTime = model.accessors[animation.samplers[0].input].maxValues[0];
    if (currentAnimationTime > maxTime) {
        currentAnimationTime = fmod(currentAnimationTime, maxTime);
    }
    applyAnimation(animation, currentAnimationTime);

    float nextAngle = circularPathAngle + circularPathSpeed * deltaTime * 10.0f;
    if (nextAngle > 360.0f) { nextAngle -= 360.0f; }
    float nextPosX = circularPathCenter.x + circularPathRadius * cos(glm::radians(nextAngle));
    float nextPosZ = circularPathCenter.z + circularPathRadius * sin(glm::radians(nextAngle));
    setRotationTowards(glm::vec3(nextPosX, posY, nextPosZ));
}

void Bird::applyAnimation(const tinygltf::Animation& animation, double time) {
    for (const auto& channel : animation.channels) {
        const auto& sampler = animation.samplers[channel.sampler];
        const auto& inputAccessor = model.accessors[sampler.input];
        const auto& outputAccessor = model.accessors[sampler.output];

        const auto* inputBuffer = &model.buffers[model.bufferViews[inputAccessor.bufferView].buffer];
        const float* times = reinterpret_cast<const float*>(
            inputBuffer->data.data() + model.bufferViews[inputAccessor.bufferView].byteOffset
        );

        int prevIdx = 0, nextIdx = 0;
        for (size_t i = 0; i < inputAccessor.count - 1; ++i) {
            if (time >= times[i] && time < times[i + 1]) {
                prevIdx = i;
                nextIdx = i + 1;
                break;
            }
        }

        float t = (time - times[prevIdx]) / (times[nextIdx] - times[prevIdx]);

        const auto* outputBuffer = &model.buffers[model.bufferViews[outputAccessor.bufferView].buffer];
        const float* outputData = reinterpret_cast<const float*>(
            outputBuffer->data.data() + model.bufferViews[outputAccessor.bufferView].byteOffset
        );

        if (channel.target_path == "translation") {
            glm::vec3 prevTranslation(
                outputData[prevIdx * 3 + 0], // x
                outputData[prevIdx * 3 + 1], // y
                outputData[prevIdx * 3 + 2]  // z
            );
            glm::vec3 nextTranslation(
                outputData[nextIdx * 3 + 0], // x
                outputData[nextIdx * 3 + 1], // y
                outputData[nextIdx * 3 + 2]  // z
            );
            glm::vec3 interpolatedTranslation = glm::mix(prevTranslation, nextTranslation, t); // Linear interpolation
            position = interpolatedTranslation;
        } else if (channel.target_path == "rotation") {
            glm::quat prevRot(
                outputData[prevIdx * 4 + 3], // w
                outputData[prevIdx * 4 + 0], // x
                outputData[prevIdx * 4 + 1], // y
                outputData[prevIdx * 4 + 2]  // z
            );
            glm::quat nextRot(
                outputData[nextIdx * 4 + 3], // w
                outputData[nextIdx * 4 + 0], // x
                outputData[nextIdx * 4 + 1], // y
                outputData[nextIdx * 4 + 2]  // z
            );
            glm::quat interpolatedRot = glm::slerp(prevRot, nextRot, t);

            rotation = glm::degrees(glm::eulerAngles(interpolatedRot));
        } else if (channel.target_path == "scale") {
            // Scale is stored as VEC3 (x, y, z)
            glm::vec3 prevScale(
                outputData[prevIdx * 3 + 0], // x
                outputData[prevIdx * 3 + 1], // y
                outputData[prevIdx * 3 + 2]  // z
            );
            glm::vec3 nextScale(
                outputData[nextIdx * 3 + 0], // x
                outputData[nextIdx * 3 + 1], // y
                outputData[nextIdx * 3 + 2]  // z
            );
            glm::vec3 interpolatedScale = glm::mix(prevScale, nextScale, t);
            scale = interpolatedScale;
        }
    }
    updateModelMatrix();
}

void Bird::updateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Bird::render(const glm::mat4& vp) {
    glUseProgram(programID);

    glm::mat4 mvp = vp * modelMatrix;

    GLuint mvpLocation = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

    for (const auto& node : model.nodes) {
        glm::mat4 nodeMatrix = mvp;

        if (node.mesh >= 0) {
            const auto& mesh = model.meshes[node.mesh];

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
                    if (material.normalTexture.index >= 0) {
                        const auto& textureInfo = material.normalTexture;

                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, textureIDs[textureInfo.index]);

                        GLuint normalMapLocation = glGetUniformLocation(programID, "normalMapSampler");
                        glUniform1i(normalMapLocation, 1);
                    }
                }

                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                    const int accessorIdx = primitive.attributes.at("POSITION");
                    const auto& accessor = model.accessors[accessorIdx];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[accessor.bufferView]);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, bufferView.byteStride,
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

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const int accessorIdx = primitive.attributes.at("NORMAL");
                    const auto& accessor = model.accessors[accessorIdx];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[accessor.bufferView]);
                    glVertexAttribPointer(2,
                                          accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          bufferView.byteStride,
                                          reinterpret_cast<void*>(accessor.byteOffset));
                    glEnableVertexAttribArray(2);
                }

                if (primitive.indices >= 0) {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[accessor.bufferView]);
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

void Bird::cleanup() {
}