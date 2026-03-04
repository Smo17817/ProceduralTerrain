#include "Terrain.h"
#include "PerlinNoiseGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib> // Per rand()
#include <ctime>   // Per time()

// Implementazione del costruttore
Terrain::Terrain(int w, int d, float s) : width(w), depth(d), scale(s) {
    generateMesh();
}

// Implementazione della generazione della mesh
// Implementazione della generazione della mesh
void Terrain::generateMesh() {
    PerlinNoiseGenerator terrainNoise(12345); // Seed per il rumore
    float baseFrequency = 0.02f;
    float amplitude = 60.0f;

    srand(static_cast<unsigned int>(time(0)));

    // 1. Calcoliamo l'offset per centrare gli alberi esattamente come il terreno!
    float offsetX = (width * scale) / 2.0f;
    float offsetZ = (depth * scale) / 2.0f;

    // Genera i vertici (X, Y, Z)
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            float xPos = x * scale;
            float zPos = z * scale;
            
            float yPos = terrainNoise.fractal(6, xPos * baseFrequency, zPos * baseFrequency) * amplitude;

            float hL = terrainNoise.fractal(6, (xPos - scale) * baseFrequency, zPos * baseFrequency) * amplitude;
            float hR = terrainNoise.fractal(6, (xPos + scale) * baseFrequency, zPos * baseFrequency) * amplitude;
            float hD = terrainNoise.fractal(6, xPos * baseFrequency, (zPos - scale) * baseFrequency) * amplitude;
            float hU = terrainNoise.fractal(6, xPos * baseFrequency, (zPos + scale) * baseFrequency) * amplitude;

            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * scale, hD - hU));

            vertices.push_back(xPos); vertices.push_back(yPos); vertices.push_back(zPos);
            vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z); 

            // --- POSIZIONAMENTO FORESTA ---
            // --- POSIZIONAMENTO FORESTA (DENSITÀ ESTREMA E NATURALE) ---
            if (yPos > 6.0f && yPos < 13.5f && normal.y > 0.65f) {
                
                // Scegliamo casualmente quanti alberi generare in questa singola "cella" (es. da 0 a 3)
                // Più alzi questo numero, più il PC esploderà di alberi!
                int treesInThisCell = rand() % 4; 

                for (int i = 0; i < treesInThisCell; i++) {
                    glm::mat4 model = glm::mat4(1.0f);
                    
                    // Creiamo un piccolo offset locale (spostiamo l'albero leggermente fuori dal vertice esatto)
                    // 'scale' è la distanza tra i vertici (di solito 1.5f nel tuo main)
                    float randomOffsetX = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * scale;
                    float randomOffsetZ = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * scale;

                    float finalX = (xPos - offsetX) + randomOffsetX;
                    float finalZ = (zPos - offsetZ) + randomOffsetZ;

                    // Posizioniamo l'albero
                    model = glm::translate(model, glm::vec3(finalX, yPos, finalZ));
                    
                    // Rotazione casuale
                    float randomRot = static_cast<float>(rand() % 360);
                    model = glm::rotate(model, glm::radians(randomRot), glm::vec3(0.0f, 1.0f, 0.0f));
                    
                    // Scala
                    float minScale = 0.008f;
                    float maxScale = 0.015f;
                    float randomScale = minScale + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxScale - minScale)));
                    model = glm::scale(model, glm::vec3(randomScale));

                    treeMatrices.push_back(model);
                }
            }
        }
    }

    // Genera gli indici
    for (int z = 0; z < depth - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = z * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft); indices.push_back(bottomLeft); indices.push_back(topRight);
            indices.push_back(topRight); indices.push_back(bottomLeft); indices.push_back(bottomRight);
        }
    }
}