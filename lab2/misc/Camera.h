#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position, lookAt, up;
    glm::mat4 viewMatrix, projectionMatrix;

public:
    Camera(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& up);
    void update();
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;
};

#endif