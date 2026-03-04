#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glm/glm.hpp>

class Terrain {
public:
    // Buffer per OpenGL
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Vettore delle matrici di istanza: contiene la "carta d'identità" 
    // (posizione, rotazione e scala) di ogni singolo albero
    std::vector<glm::mat4> treeMatrices; 

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