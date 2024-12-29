#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _USE_MATH_DEFINES

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iostream>
#include <utils/init_glfw_glad.h>
#include <utils/camera.h>
#include <scene/scene.h>

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

static bool isMousePressed = false;
static double lastMouseX, lastMouseY;
static float sensitivity = 0.1f;

Camera mainCamera;
Camera lightCamera;
Camera* activeCamera = &mainCamera;

Scene cityScene;

int windowWidth = 1024;
int windowHeight = 768;

int main(void)
{
	// Initialise OpenGL
	GLFWwindow* window = initializeOpenGL("Toward a Futuristic Emerald Isle", windowWidth, windowHeight);
	if (!window) return -1;

	// Keys
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	// Cameras
	mainCamera.position = glm::vec3(0.0f, 300.0f, 500.0f);
	mainCamera.lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	mainCamera.up = glm::vec3(0.0f, 1.0f, 0.0f);
	mainCamera.fov = 45.0f;
	mainCamera.nearPlane = 0.1f;
	mainCamera.farPlane = 40000.0f;
	mainCamera.aspectRatio = 4.0f / 3.0f;

	lightCamera.position = glm::vec3(-275.0f, 500.0f, -275.0f);
	lightCamera.lookAt = glm::vec3(-275.0f, 0.0f, -275.0f);
	lightCamera.up = glm::vec3(0.0f, 1.0f, 0.0f);
	lightCamera.fov = 120.0f;
	lightCamera.nearPlane = 48.0f;
	lightCamera.farPlane = 600.0f;
	lightCamera.aspectRatio = 1.0f;

	/*** SWITCH CAMERA ***/
	activeCamera = &mainCamera;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Scene setup
	cityScene.setupLighting();
	cityScene.initializeAxis();
	cityScene.initializeTerrain(4000, 4000, 30.0f);
	cityScene.initializeCitiesOnHills(100);
	cityScene.initializeForest(cityScene.terrain, 3000);
	cityScene.initializeCars(200);
	cityScene.initializeBirds(400);

	// Animations
	double lastTime = glfwGetTime();

	// FPS
	int frames = 0;
	double fTime = 0.0;

	// Skybox
	glm::vec3 skyboxPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 skyboxScale(3000.0f, 3000.0f, 3000.0f);
	cityScene.initializeSkybox(skyboxPosition, skyboxScale);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 viewMatrix = activeCamera->getViewMatrix();
		glm::mat4 projectionMatrix = activeCamera->getProjectionMatrix();
		glm::mat4 vp = projectionMatrix * viewMatrix;

		glm::vec3 cameraPosition = activeCamera->getPosition();

		// Delta time
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// FPS
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = frames / fTime;
			frames = 0;
			fTime = 0.0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Futuristic Emerald Isle | FPS: " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		cityScene.render(vp, cameraPosition, deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (!glfwWindowShouldClose(window));

	// Clean up
	cityScene.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			isMousePressed = true;
			glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
		} else if (action == GLFW_RELEASE) {
			isMousePressed = false;
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (xpos < 0 || xpos >= windowWidth || ypos < 0 || ypos > windowHeight)
		return;

	if (isMousePressed) {
		double deltaX = xpos - lastMouseX;
		double deltaY = ypos - lastMouseY;
		lastMouseX = xpos;
		lastMouseY = ypos;

		glm::vec3 viewDirection = glm::normalize(activeCamera->lookAt - activeCamera->position);

		glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians((float)deltaX * sensitivity), activeCamera->up);
		viewDirection = glm::vec3(yaw * glm::vec4(viewDirection, 0.0f));

		glm::vec3 rightDirection = glm::normalize(glm::cross(viewDirection, activeCamera->up));
		glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians((float)deltaY * sensitivity), rightDirection);
		viewDirection = glm::vec3(pitch * glm::vec4(viewDirection, 0.0f));

		activeCamera->setLookAt(activeCamera->position + viewDirection);
	}
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	if (activeCamera) {
		activeCamera->speed += static_cast<float>(yOffset);
		activeCamera->speed = glm::clamp(activeCamera->speed, 0.2f, 5.0f);
	}
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	float lightMovementSpeed = 10.0f;

	// Camera controls
	if (key == GLFW_KEY_W) activeCamera->moveForward(activeCamera->speed);
	if (key == GLFW_KEY_S) activeCamera->moveBackward(activeCamera->speed);
	if (key == GLFW_KEY_A) activeCamera->moveLeft(activeCamera->speed);
	if (key == GLFW_KEY_D) activeCamera->moveRight(activeCamera->speed);
	if (key == GLFW_KEY_Q) activeCamera->moveUp(activeCamera->speed);
	if (key == GLFW_KEY_E) activeCamera->moveDown(activeCamera->speed);

	// Adjust field of view
	if (key == GLFW_KEY_UP) activeCamera->setFoV(activeCamera->fov - 1.0f);
	if (key == GLFW_KEY_DOWN) activeCamera->setFoV(activeCamera->fov + 1.0f);

	// Move the light
	if (key == GLFW_KEY_I) cityScene.lightPosition.z -= lightMovementSpeed; // Forward
	if (key == GLFW_KEY_K) cityScene.lightPosition.z += lightMovementSpeed; // Backward
	if (key == GLFW_KEY_J) cityScene.lightPosition.x -= lightMovementSpeed; // Left
	if (key == GLFW_KEY_L) cityScene.lightPosition.x += lightMovementSpeed; // Right
	if (key == GLFW_KEY_U) cityScene.lightPosition.y += lightMovementSpeed; // Up
	if (key == GLFW_KEY_O) cityScene.lightPosition.y -= lightMovementSpeed; // Down

	// Reset camera
	if (key == GLFW_KEY_R) {
		mainCamera.setPosition(glm::vec3(0.0f, 300.0f, 500.0f));
		mainCamera.lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		lightCamera.setPosition(glm::vec3(-275.0f, 500.0f, -275.0f));
		lightCamera.lookAt = glm::vec3(-275.0f, 0.0f, -275.0f);
		activeCamera = &mainCamera;
	}

	// Exit application
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);
}