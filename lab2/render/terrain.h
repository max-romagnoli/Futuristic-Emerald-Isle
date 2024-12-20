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

    void initialize(int width, int depth, float maxHeight, float repeatFactor);
    void render(const glm::mat4& vp);
    void cleanup();

    glm::vec3 getCenterHill();
    std::vector<glm::vec3> getHighestPoints(int n);
    int getWidth() const;
    int getDepth() const;
    float getHeightAt(float x, float z) const;

private:
    int width;
    int depth;
    float maxHeight;
};