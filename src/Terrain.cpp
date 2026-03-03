#include "Terrain.h"
#include "PerlinNoiseGenerator.h"

// Implementazione del costruttore
Terrain::Terrain(int w, int d, float s) : width(w), depth(d), scale(s) {
    generateMesh();
}

// Implementazione della generazione della mesh
void Terrain::generateMesh() {
    PerlinNoiseGenerator terrainNoise(12345); // Seed per il rumore
    float baseFrequency = 0.02f;
    float amplitude = 15.0f;

    // 1. Genera i vertici (X, Y, Z)
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            float xPos = x * scale;
            float zPos = z * scale;
            
            // Usiamo l'fBm con 6 ottave
            float noiseValue = terrainNoise.fractal(6, xPos * baseFrequency, zPos * baseFrequency);
            float yPos = noiseValue * amplitude;

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }

    // 2. Genera gli indici per comporre i triangoli (EBO)
    for (int z = 0; z < depth - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = z * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            // Primo triangolo
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Secondo triangolo
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}