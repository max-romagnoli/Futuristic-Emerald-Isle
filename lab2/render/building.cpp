#include "building.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "shader.h"
#include <utils/load_textures.h>


void Building::initialize(glm::vec3 position, glm::vec3 scale, int vFactor, int textureNum) {
	// Define scale of the building geometry
	this->position = position;
	this->scale = scale;
	this->vFactor = vFactor;
	this->textureNum = textureNum;

	// Create a vertex array object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create a vertex buffer object to store the vertex data
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	// Create a vertex buffer object to store the color data
       // TODO:
	glGenBuffers(1, &colorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	// TODO: Scale UV coordinates to range greater than 1
	for (int i = 0; i < 24; ++i)
		uv_buffer_data[2*i+1] *= vFactor;			// Modifies the V coordinates (2*i+1) of each UV pair, scaling it 5x

	// TODO: Create a vertex buffer object to store the UV data
	glGenBuffers(1, &uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

	// Create an index buffer object to store the index data that defines triangle faces
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	programID = LoadShadersFromFile("../lab2/shaders/box.vert", "../lab2/shaders/box.frag");
	if (programID == 0)
	{
		std::cerr << "Failed to load shaders." << std::endl;
	}

	// Get a handle for our "MVP" uniform
	mvpMatrixID = glGetUniformLocation(programID, "MVP");

	// TODO: Load a texture
	std::stringstream texturePath;
	texturePath << "../lab2/assets/textures/facade" << textureNum << ".jpg";
	textureID = LoadTextureTileBox(texturePath.str().c_str());

	// TODO: Get a handle to texture sampler
	textureSamplerID  = glGetUniformLocation(programID,"textureSampler"); // links textureSample in shader to textureSamplerID in the code

}


void Building::render(glm::mat4 cameraMatrix) {
	glUseProgram(programID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	// TODO: Model transform
	// -----------------------
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);
	// -----------------------

	// Set model-view-projection matrix
	glm::mat4 mvp = cameraMatrix * modelMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

	// TODO: Enable UV buffer and texture sampler
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glVertexAttribPointer(2,
		2,						// number of components per vertex (U, V)
		GL_FLOAT,			    // type for each component
		GL_FALSE,
		0,
		0);

	// Set textureSampler to use texture unit 0
	glActiveTexture(GL_TEXTURE0);					// activate texture unit 0
	glBindTexture(GL_TEXTURE_2D, textureID);		// binds texture to unit
	glUniform1i(textureSamplerID, 0);			// set sample to use texture unit

	// Draw the box
	glDrawElements(
		GL_TRIANGLES,      // mode
		36,    			   // number of indices
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
}


void Building::cleanup() {
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &colorBufferID);
	glDeleteBuffers(1, &indexBufferID);
	glDeleteVertexArrays(1, &vertexArrayID);
	//glDeleteBuffers(1, &uvBufferID);
	//glDeleteTextures(1, &textureID);
	glDeleteProgram(programID);
}