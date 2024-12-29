#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;   // Camera position (eye)
    glm::vec3 lookAt;     // Target point the camera is looking at
    glm::vec3 up;         // Up vector
    float yaw = -90.0f;   // Horizontal rotation (in degrees), default facing -Z
    float pitch = 0.0f;
    float fov;            // Field of View
    float nearPlane;      // Near clipping plane
    float farPlane;       // Far clipping plane
    float aspectRatio;    // Aspect ratio (width/height)
    float speed = 3.0f;

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, lookAt, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }


    void setPosition(glm::vec3 pos) {
        glm::vec3 offset = pos - position;
        position = pos;
        lookAt += offset;
    }

    void setLookAt(glm::vec3 target) {
        lookAt = target;
    }

    void setSphericalCoords(float azimuth, float polar, float distance) {
        position.y = distance * cos(polar);
        position.x = distance * cos(azimuth);
        position.z = distance * sin(azimuth);
    }

    // Movement along the camera's forward vector
    void moveForward(float distance) {
        glm::vec3 forward = glm::normalize(lookAt - position);
        position += forward * distance;
        lookAt += forward * distance;
    }

    void moveBackward(float distance) {
        glm::vec3 forward = glm::normalize(lookAt - position);
        position -= forward * distance;
        lookAt -= forward * distance;
    }

    // Movement along the camera's right vector
    void moveRight(float distance) {
        glm::vec3 forward = glm::normalize(lookAt - position);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        position += right * distance;
        lookAt += right * distance;
    }

    void moveLeft(float distance) {
        glm::vec3 forward = glm::normalize(lookAt - position);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        position -= right * distance;
        lookAt -= right * distance;
    }

    // Movement along the up vector
    void moveUp(float distance) {
        position += up * distance;
        lookAt += up * distance;
    }

    void moveDown(float distance) {
        position -= up * distance;
        lookAt -= up * distance;
    }

    // Adjust the field of view
    void setFoV(float newFoV) {
        fov = glm::clamp(newFoV, 1.0f, 120.0f); // Clamp FoV between reasonable limits
    }

    [[nodiscard]] glm::vec3 getPosition() const {
        return position;
    }

};

#endif