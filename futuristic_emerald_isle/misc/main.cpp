#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Futuristic Emerald Isle", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) return -1;

    glfwSetKeyCallback(window, key_callback);

    Renderer renderer;
    Camera camera(glm::vec3(0.0f, 50.0f, 200.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    Scene scene;

    renderer.initialize();
    scene.initialize();

    while (!glfwWindowShouldClose(window)) {
        camera.update();
        renderer.render(scene, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scene.cleanup();
    renderer.cleanup();
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    // Implement camera controls here
}