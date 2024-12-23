#ifndef CAR_H
#define CAR_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "glad/gl.h"
#include "utils/utils.h"

class Car {
public:
    Car();
    ~Car();

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setRotationTowards(const glm::vec3& targetPosition);
    void setScale(const glm::vec3& scale);

    void setAnimation(const glm::vec3& start, const glm::vec3& end, float duration);
    void update(double deltaTime);

    void render(const glm::mat4& vp);
    void cleanup();

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;

    glm::vec3 startPosition;
    glm::vec3 endPosition;
    double animationDuration;
    double elapsedTime;

    void updateModelMatrix();

    static GLuint programID;
    static tinygltf::Model model;
    static std::vector<GLuint> bufferIDs;
    static std::vector<GLuint> textureIDs;

    friend class Cars;
};

#endif