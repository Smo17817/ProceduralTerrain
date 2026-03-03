#include <vector>
#include "FastNoiseLite.h"

// Generazione dei vertici
std::vector<float> vertices;
std::vector<unsigned int> indices;

int width = 100;  // Dimensione della griglia X
int depth = 100;  // Dimensione della griglia Z
float scale = 1.5f; // Distanza tra i vertici

FastNoiseLite noise;
noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
noise.SetFrequency(0.05f); // Controlla la "dolcezza" delle colline

// 1. Genera i vertici (X, Y, Z)
for (int z = 0; z < depth; ++z) {
    for (int x = 0; x < width; ++x) {
        float xPos = x * scale;
        float zPos = z * scale;
        // Calcola l'altezza Y usando il Perlin Noise
        float yPos = noise.GetNoise((float)xPos, (float)zPos) * 10.0f; // * 10 per amplificare l'altezza

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