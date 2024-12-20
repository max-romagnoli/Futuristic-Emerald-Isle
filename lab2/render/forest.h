#ifndef FOREST_H
#define FOREST_H

#include "tree.h"
#include <vector>

class Terrain;

class Forest {
public:
    Forest();
    ~Forest();

    bool initialize(const std::string& modelPath);
    void generateTrees(const Terrain& terrain, int count, float altitudeThreshold);
    void render(const glm::mat4& vp, const glm::vec3& cameraPosition, float renderRadius);

    void printCoords();

    void cleanup();

private:
    std::vector<Tree> trees;
};

#endif