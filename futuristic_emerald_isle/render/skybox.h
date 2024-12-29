#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Skybox {
	glm::vec3 position;
	glm::vec3 scale;

	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;

	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID;

    void initialize(glm::vec3 position, glm::vec3 scale);
    void render(glm::mat4 cameraMatrix);
    void cleanup();

	GLfloat vertex_buffer_data[72] = {
		// Front face
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		// Back face
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		// Left face
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		// Right face
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,

		// Top face
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		// Bottom face
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
	};

	GLfloat color_buffer_data[72] = {
		// Front, red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Back, yellow
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		// Left, green
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// Right, cyan
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		// Top, blue
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Bottom, magenta
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
	};

	GLuint index_buffer_data[36] = {
		2, 1, 0,
		3, 2, 0,

		6, 5, 4,
		7, 6, 4,

		10, 9, 8,
		11, 10, 8,

		14, 13, 12,
		15, 14, 12,

		18, 17, 16,
		19, 18, 16,

		22, 21, 20,
		23, 22, 20
	};

	// TODO: Define UV buffer data
	GLfloat uv_buffer_data[48] = {
		// Back +Z
		1.0f, 2.0f / 3.0f,		// Top-right			U (horizontal), V (vertical)
		0.75f, 2.0f / 3.0f,		// Top-left				U, V
		0.75f, 1.0f / 3.0f,		// Bottom-left
		1.0f, 1.0f / 3.0f,		// Bottom-right

		// Front -Z
		0.5f, 2.0f / 3.0f,
		0.25f, 2.0f / 3.0f,
		0.25f, 1.0f / 3.0f,
		0.5f, 1.0f / 3.0f,

		// Left +X
		0.25f, 2.0f / 3.0f,
		0.0f, 2.0f / 3.0f,
		0.0f, 1.0f / 3.0f,
		0.25f, 1.0f / 3.0f,

		// Right -X
		0.75f, 2.0f / 3.0f,
		0.5f, 2.0f / 3.0f,
		0.5f, 1.0f / 3.0f,
		0.75f, 1.0f / 3.0f,

		// Top -Y
		0.25f, 0.0f,			// Bottom-left
		0.5f, 0.0f,				// Bottom-right
		0.5f, 1.0f / 3.0f,		// Top-right
		0.25f, 1.0f / 3.0f,		// Top-left

		// Bottom  +Y
		0.25f, 2.0f / 3.0f,
		0.5f, 2.0f / 3.0f,
		0.5f, 1.0f,
		0.25f, 1.0f,
	};
};

#endif