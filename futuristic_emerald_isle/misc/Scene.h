#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include "ModelLoader.h"

class Scene {
private:
    std::vector<Building> buildings;
    GLuint skyboxVAO, skyboxTexture;

public:
    void initialize();
    void draw(const glm::mat4& vp);
    void cleanup();
};

#endif