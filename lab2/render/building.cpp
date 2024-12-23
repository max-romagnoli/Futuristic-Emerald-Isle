#include "building.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <utils/load_textures.h>

Building::Building() {}

Building::~Building() {}

void Building::initialize(const glm::vec3& position, const glm::vec3& scale, int vFactor, GLuint facadeID) {
    this->position = position;
    this->scale = scale;
    this->vFactor = vFactor;
    this->facadeID = facadeID;

    populateNormals();

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	for (int i = 0; i < 24; ++i)
		uv_buffer_data[2*i+1] *= vFactor;

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);
}

void Building::render(const glm::mat4& vp, GLuint programID, GLuint textureID, GLuint mvpMatrixID,
		GLuint modelMatrixID, GLuint lightPositionID, GLuint lightIntensityID, glm::vec3 lightPosition, glm::vec3 lightIntensity) {

	glUseProgram(programID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	// Model matrix
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

	// MVP matrix
	glm::mat4 mvp = vp * modelMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Lighting
	glUniform3fv(lightPositionID, 1, &lightPosition[0]);
	glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

	// Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, facadeID);
	glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void Building::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
}

std::string Building::populateNormals() {
	int offsets[] = {
		0,    // Front
		12,   // Back
		24,   // Left
		36,   // Right
		48,   // Top
		60    // Bottom
	};

	int verticesPerFace = 4;

	std::fill(std::begin(normal_buffer_data), std::end(normal_buffer_data), 0.0f);

	for (int i = 0; i < 6; ++i) {
		int vertexOffset = offsets[i];
		int normalOffset = vertexOffset;
		addFaceNormal(normal_buffer_data, normalOffset, vertex_buffer_data, vertexOffset);
	}

	std::stringstream addedNormals;
	for (int i = 0; i < 72; i += 3) {
		addedNormals << "Normal: ";
		addedNormals << normal_buffer_data[i] << ", ";
		addedNormals << normal_buffer_data[i + 1] << ", ";
		addedNormals << normal_buffer_data[i + 2] << "\n";
	}

	return addedNormals.str();
}

glm::vec3 Building::calcNormal(const GLfloat* faceVertices) {
	glm::vec3 v1(faceVertices[0], faceVertices[1], faceVertices[2]);
	glm::vec3 v2(faceVertices[3], faceVertices[4], faceVertices[5]);
	glm::vec3 v3(faceVertices[6], faceVertices[7], faceVertices[8]);

	glm::vec3 edge1 = v2-v1;
	glm::vec3 edge2 = v3-v1;
	glm::vec3 normal = -glm::normalize(glm::cross(edge2, edge1));

	return normal;
}

void Building::addFaceNormal(GLfloat* normalBuffer, int offset, const GLfloat* vertexBuffer, int vertexOffset) {
	glm::vec3 normal = calcNormal(vertexBuffer + vertexOffset);
	for (int i = 0; i < 6; ++i) {
		normalBuffer[offset + i * 3 + 0] = normal.x;
		normalBuffer[offset + i * 3 + 1] = normal.y;
		normalBuffer[offset + i * 3 + 2] = normal.z;
	}
}