#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Librerie matematiche GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
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

    // Configurazione Stato OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2. Carichiamo gli Shader
    Shader terrainShader("../shaders/terrain.vert", "../shaders/terrain.frag");
    Shader waterShader("../shaders/terrain.vert", "../shaders/water.frag");

    // 3. Generiamo il Terreno e i Buffer VAO/VBO/EBO
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

    // Attributo 0: Posizioni | Attributo 1: Normali
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 4. Preparazione Piano dell'Acqua
    float waterLevel = 1.5f; 
    float waterSize = 500.0f; 
    float waterVertices[] = {
        -waterSize, waterLevel, -waterSize,  0.0f, 1.0f, 0.0f,
        waterSize, waterLevel, -waterSize,  0.0f, 1.0f, 0.0f,
        waterSize, waterLevel,  waterSize,  0.0f, 1.0f, 0.0f,
        -waterSize, waterLevel,  waterSize,  0.0f, 1.0f, 0.0f
    };
    unsigned int waterIndices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int waterVAO, waterVBO, waterEBO;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glGenBuffers(1, &waterEBO);

    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(waterIndices), waterIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 5. RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        float time = (float)glfwGetTime();
        
        // --- LUCE E CIELO FISSI ---
        glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f)); // Luce che viene dall'alto
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f); // Luce bianca calda
        glm::vec3 skyColor = glm::vec3(0.4f, 0.7f, 1.0f);   // Azzurro costante

        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- MATRICI TELECAMERA ---
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 1000.0f);
        float camX = sin(time * 0.2f) * 150.0f;
        float camZ = cos(time * 0.2f) * 150.0f;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 80.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-(myTerrain.width * myTerrain.scale) / 2.0f, 0.0f, -(myTerrain.depth * myTerrain.scale) / 2.0f));

        // --- 1. DISEGNO TERRENO ---
        terrainShader.use();
        terrainShader.setMat4("projection", glm::value_ptr(projection));
        terrainShader.setMat4("view", glm::value_ptr(view));
        terrainShader.setMat4("model", glm::value_ptr(model));
        terrainShader.setVec3("lightDir", lightDirection);
        terrainShader.setVec3("lightColor", lightColor);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)myTerrain.indices.size(), GL_UNSIGNED_INT, 0);

        // --- 2. DISEGNO ACQUA ---
        waterShader.use();
        waterShader.setMat4("projection", glm::value_ptr(projection));
        waterShader.setMat4("view", glm::value_ptr(view));
        waterShader.setMat4("model", glm::value_ptr(model));
        waterShader.setVec3("lightDir", lightDirection);
        waterShader.setVec3("lightColor", lightColor);

        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 5. Pulizia
    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &waterVAO); glDeleteBuffers(1, &waterVBO); glDeleteBuffers(1, &waterEBO);
    glfwTerminate();
    
    return 0;
}