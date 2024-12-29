#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
    void initialize();
    void render(const Scene& scene, const Camera& camera);
    void cleanup();
};

#endif