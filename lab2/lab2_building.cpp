#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE 1
//#define TINYGLTF_NO_STB_IMAGE_WRITE 1
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>

#include <sstream>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include <render/axys_xyz.h>
#include <render/building.h>
#include <utils/init_glfw_glad.h>
#include <utils/camera.h>
#include <scene/scene.h>

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

static bool isMousePressed = false;
static double lastMouseX, lastMouseY;
static float sensitivity = 0.1f;

Camera mainCamera;
Camera lightCamera;
Camera* activeCamera = &mainCamera;

int main(void)
{
	// Initialise OpenGL
	GLFWwindow* window = initializeOpenGL("Toward a Futuristic Emerald Isle", 1024, 768);
	if (!window) return -1;

	// Keys
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

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
	Scene cityScene;
	cityScene.initializeAxis();
	cityScene.initializeTerrain(2000, 2000, 30.0f);
	cityScene.initializeCitiesOnHills(30);
	cityScene.initializeForest(cityScene.terrain);

	// Skybox
	glm::vec3 skyboxPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 skyboxScale(3000.0f, 3000.0f, 3000.0f); // Large enough to enclose the city
	cityScene.initializeSkybox(skyboxPosition, skyboxScale);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 viewMatrix = activeCamera->getViewMatrix();
		glm::mat4 projectionMatrix = activeCamera->getProjectionMatrix();
		glm::mat4 vp = projectionMatrix * viewMatrix;

		cityScene.render(vp);

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

/* TODO: could refactor and put some fields into the class*/
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!isMousePressed) return;

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	// Camera controls
	if (key == GLFW_KEY_W) activeCamera->moveForward(10.0f);
	if (key == GLFW_KEY_S) activeCamera->moveBackward(10.0f);
	if (key == GLFW_KEY_A) activeCamera->moveLeft(10.0f);
	if (key == GLFW_KEY_D) activeCamera->moveRight(10.0f);
	if (key == GLFW_KEY_Q) activeCamera->moveUp(10.0f);
	if (key == GLFW_KEY_E) activeCamera->moveDown(10.0f);

	// Adjust field of view
	if (key == GLFW_KEY_UP) activeCamera->setFoV(activeCamera->fov - 1.0f);
	if (key == GLFW_KEY_DOWN) activeCamera->setFoV(activeCamera->fov + 1.0f);

	// Switch between cameras
	if (key == GLFW_KEY_1) activeCamera = &mainCamera;
	if (key == GLFW_KEY_2) activeCamera = &lightCamera;

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

// Is called whenever a key is pressed/released via GLFW
/*void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		viewAzimuth = 0.f;
		viewPolar = 0.f;
		eye_center.y = viewDistance * cos(viewPolar);
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
		std::cout << "Reset." << std::endl;
	}

	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewPolar -= 0.1f;
		eye_center.y = viewDistance * cos(viewPolar);
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewPolar += 0.1f;
		eye_center.y = viewDistance * cos(viewPolar);
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewAzimuth -= 0.1f;
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewAzimuth += 0.1f;
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}*/