#include <glad/glad.h>  // ATTENZIONE: glad deve sempre essere incluso PRIMA di glfw
#include <GLFW/glfw3.h>
#include <iostream>
#include "Terrain.h"

// Funzione per ridimensionare la viewport se l'utente rimpicciolisce/ingrandisce la finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Funzione per gestire l'input da tastiera
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 1. Inizializzazione di GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Creazione della finestra
    GLFWwindow* window = glfwCreateWindow(800, 600, "Generatore di Terreno Procedurale", NULL, NULL);
    if (window == NULL) {
        std::cout << "Errore nella creazione della finestra GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. Inizializzazione di GLAD (carica i puntatori alle funzioni di OpenGL)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Errore nell'inizializzazione di GLAD" << std::endl;
        return -1;
    }

    // 4. Generiamo i dati del terreno
    std::cout << "Generazione mesh in corso..." << std::endl;
    Terrain myTerrain(100, 100, 1.5f);
    std::cout << "Vertici: " << myTerrain.vertices.size() << " | Indici: " << myTerrain.indices.size() << std::endl;

    // 5. Configurazione dei Buffer OpenGL (VAO, VBO, EBO)
    // 
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Carichiamo i Vertici nel VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, myTerrain.vertices.size() * sizeof(float), myTerrain.vertices.data(), GL_STATIC_DRAW);

    // Carichiamo gli Indici nell'EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myTerrain.indices.size() * sizeof(unsigned int), myTerrain.indices.data(), GL_STATIC_DRAW);

    // Spieghiamo a OpenGL come leggere i dati (3 float per ogni vertice: X, Y, Z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Modalità Wireframe (disegna solo i contorni dei triangoli, perfetto per il debug del terreno)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 6. IL RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Rendering: Puliamo lo schermo con un colore di sfondo (blu scuro)
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- QUI DISEGNEREMO IL TERRENO ---
        // Per ora non lo disegniamo attivamente perché ci mancano gli Shader e la Telecamera!
        // Se lo disegnassimo ora, OpenGL non saprebbe come trasformare le coordinate 3D nello schermo 2D.
        
        // Scambia i buffer e interroga gli eventi (mouse, tastiera)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. Pulizia finale
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    
    return 0;
}