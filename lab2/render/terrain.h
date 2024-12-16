#include <vector>
#include <glm/glm.hpp>

#include "glad/gl.h"

class Terrain {
public:
    GLuint vertexArrayID, vertexBufferID, indexBufferID, uvBufferID;
    GLuint textureID;
    GLuint programID;

    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec2> uvs;

    void initialize(int width, int depth, float maxHeight);
    void render(const glm::mat4& vp);
    void cleanup();
};
