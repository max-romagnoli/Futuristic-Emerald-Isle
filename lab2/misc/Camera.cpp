#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& up)
    : position(position), lookAt(lookAt), up(up) {
    projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
}

void Camera::update() {
    viewMatrix = glm::lookAt(position, lookAt, up);
}

const glm::mat4& Camera::getViewMatrix() const {
    return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const {
    return projectionMatrix;
}