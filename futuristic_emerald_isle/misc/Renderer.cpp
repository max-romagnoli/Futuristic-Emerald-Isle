#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"

void Renderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
}

void Renderer::render(const Scene& scene, const Camera& camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
    scene.draw(vp);
}

void Renderer::cleanup() {}