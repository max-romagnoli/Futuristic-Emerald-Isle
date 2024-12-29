#include "Skybox.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <utils/load_textures.h>

void Skybox::initialize(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../futuristic_emerald_isle/shaders/skybox.vert", "../futuristic_emerald_isle/shaders/skybox.frag");
    if (programID == 0)
    {
        std::cerr << "Failed to load shaders." << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");

    textureID = LoadTextureTileBox("../futuristic_emerald_isle/assets/skyboxes/sky.png");

    textureSamplerID  = glGetUniformLocation(programID,"textureSampler");
}

void Skybox::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glm::mat4 modelMatrix = glm::mat4();
    modelMatrix = glm::scale(modelMatrix, scale);

    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glDrawElements(
        GL_TRIANGLES,
        36,
        GL_UNSIGNED_INT,
        (void*)0
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Skybox::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(programID);
}