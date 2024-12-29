#ifndef BIRD_H
#define BIRD_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "glad/gl.h"
#include "utils/utils.h"
#include <glm/gtc/quaternion.hpp>

class Bird {
public:
    Bird();
    ~Bird();

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    void setCircularPath(const glm::vec3& center, float radius, float speed);
    void setRotationTowards(const glm::vec3& targetPosition);
    void updateModelMatrix();

    void update(double deltaTime);
    void applyAnimation(const tinygltf::Animation& animation, double time);
    void render(const glm::mat4& vp);

    void cleanup();

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;

    glm::vec3 circularPathCenter;
    float circularPathRadius = 0.0f;
    float circularPathSpeed = 0.0f;
    float circularPathAngle = 0.0f;

    double currentAnimationTime;

    static GLuint programID;
    static tinygltf::Model model;
    static std::vector<GLuint> bufferIDs;
    static std::vector<GLuint> textureIDs;
};

#endif
