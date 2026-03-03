#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Librerie matematiche GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "Terrain.h"
#include "Shader.h" 

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 1. Inizializzazione GLFW e Finestra
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Generatore di Terreno Procedurale", NULL, NULL);
    if (window == NULL) {
        std::cout << "Errore creazione finestra" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Errore inizializzazione GLAD" << std::endl;
        return -1;
    }

    // Abilitiamo il Depth Test (Z-Buffer) fondamentale per il 3D! 
    glEnable(GL_DEPTH_TEST);

    // 2. Carichiamo gli Shader
    Shader terrainShader("../shaders/terrain.vert", "../shaders/terrain.frag");

    // 3. Generiamo il Terreno e i Buffer
    Terrain myTerrain(100, 100, 1.5f);
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, myTerrain.vertices.size() * sizeof(float), myTerrain.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myTerrain.indices.size() * sizeof(unsigned int), myTerrain.indices.data(), GL_STATIC_DRAW);

    // Attributo 0: Posizioni (X, Y, Z) - Ora lo "stride" è 6 float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attributo 1: Normali (Nx, Ny, Nz) - Inizia dopo i primi 3 float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 4. RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // --- CALCOLO DEL TEMPO E DEL SOLE ---
        float time = glfwGetTime();
        float sunSpeed = 0.3f; 
        
        float sunX = cos(time * sunSpeed);
        float sunY = sin(time * sunSpeed);
        float sunZ = -0.5f; 
        
        glm::vec3 lightDirection = glm::normalize(glm::vec3(sunX, sunY, sunZ));

        // --- CALCOLO COLORI GIORNO/NOTTE ---
        glm::vec3 skyColor;
        glm::vec3 lightColor;

        if (sunY > 0.2f) { 
            // GIORNO PIENO
            skyColor = glm::vec3(0.4f, 0.7f, 1.0f); 
            lightColor = glm::vec3(1.0f, 1.0f, 0.9f); 
        } 
        else if (sunY > 0.0f) { 
            // TRAMONTO / ALBA
            float blend = sunY / 0.2f; 
            skyColor = glm::mix(glm::vec3(0.8f, 0.4f, 0.2f), glm::vec3(0.4f, 0.7f, 1.0f), blend); 
            lightColor = glm::mix(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(1.0f, 1.0f, 0.9f), blend); 
        } 
        else { 
            // NOTTE
            skyColor = glm::vec3(0.05f, 0.05f, 0.1f); 
            lightColor = glm::vec3(0.2f, 0.2f, 0.4f); 
            lightDirection = glm::normalize(glm::vec3(sunX, 0.0f, sunZ));
        }

        // Applichiamo il colore del cielo al background
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Attiviamo lo shader
        terrainShader.use();

        // --- ECCO LA CORREZIONE: USIAMO setVec3! ---
        terrainShader.setVec3("lightDir", -lightDirection); 
        terrainShader.setVec3("lightColor", lightColor);

        // -- MATEMATICA DELLA TELECAMERA (MVP) --
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 1000.0f);
        
        float camRadius = 150.0f;
        float camX = sin(time * 0.2f) * camRadius;
        float camZ = cos(time * 0.2f) * camRadius;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 80.0f, camZ), 
                                    glm::vec3(0.0f, 0.0f, 0.0f), 
                                    glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-(myTerrain.width * myTerrain.scale) / 2.0f, 0.0f, -(myTerrain.depth * myTerrain.scale) / 2.0f));

        terrainShader.setMat4("projection", glm::value_ptr(projection));
        terrainShader.setMat4("view", glm::value_ptr(view));
        terrainShader.setMat4("model", glm::value_ptr(model));

        // -- DISEGNO DEL TERRENO --
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, myTerrain.indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 5. Pulizia
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    
    return 0;
}