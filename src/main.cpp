#include <iostream>
#include "Terrain.h"

int main() {
    std::cout << "Avvio Generatore di Terreno Procedurale..." << std::endl;

    // Testiamo se il terreno si genera correttamente!
    Terrain myTerrain(100, 100, 1.5f);
    
    std::cout << "Vertici generati: " << myTerrain.vertices.size() << std::endl;
    std::cout << "Indici generati: " << myTerrain.indices.size() << std::endl;

    return 0;
}