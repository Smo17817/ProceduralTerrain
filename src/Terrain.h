#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glm/glm.hpp>

class Terrain {
public:
    // OpenGL Buffers
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Vector of instance matrices: contains the "ID card" 
    // (position, rotation, and scale) of every single tree
    std::vector<glm::mat4> treeMatrices; 

    // Grid parameters
    int width;
    int depth;
    float scale;

    // Constructor with default parameters
    Terrain(int w = 100, int d = 100, float s = 1.5f);

private:
    // Internal function to calculate data
    void generateMesh();
};

#endif