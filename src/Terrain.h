#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>

class Terrain {
public:
    // Buffer per OpenGL
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Parametri della griglia
    int width;
    int depth;
    float scale;

    // Costruttore con parametri di default
    Terrain(int w = 100, int d = 100, float s = 1.5f);

private:
    // Funzione interna per calcolare i dati
    void generateMesh();
};

#endif