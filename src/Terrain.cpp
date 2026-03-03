#include "Terrain.h"
#include "PerlinNoiseGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Implementazione del costruttore
Terrain::Terrain(int w, int d, float s) : width(w), depth(d), scale(s) {
    generateMesh();
}

// Implementazione della generazione della mesh
void Terrain::generateMesh() {
    PerlinNoiseGenerator terrainNoise(12345); // Seed per il rumore
    float baseFrequency = 0.02f;
    float amplitude = 60.0f;

    // 1. Genera i vertici (X, Y, Z)
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            float xPos = x * scale;
            float zPos = z * scale;
            
            // Altezza del punto attuale
            float yPos = terrainNoise.fractal(6, xPos * baseFrequency, zPos * baseFrequency) * amplitude;

            // Calcolo delle Normali (Guardiamo i vicini!)
            float hL = terrainNoise.fractal(6, (xPos - scale) * baseFrequency, zPos * baseFrequency) * amplitude; // Sinistra
            float hR = terrainNoise.fractal(6, (xPos + scale) * baseFrequency, zPos * baseFrequency) * amplitude; // Destra
            float hD = terrainNoise.fractal(6, xPos * baseFrequency, (zPos - scale) * baseFrequency) * amplitude; // Giù
            float hU = terrainNoise.fractal(6, xPos * baseFrequency, (zPos + scale) * baseFrequency) * amplitude; // Su

            // Creiamo il vettore Normale e lo normalizziamo (lunghezza 1)
            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * scale, hD - hU));

            // Ora inseriamo 6 valori per ogni vertice, non più solo 3!
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(normal.x); // <-- NUOVO
            vertices.push_back(normal.y); // <-- NUOVO
            vertices.push_back(normal.z); // <-- NUOVO
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