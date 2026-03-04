#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Librerie matematiche GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include "Terrain.h"
#include "Shader.h" 

// Callback per il ridimensionamento della finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Gestione input da tastiera
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 1. Inizializzazione GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Procedural World", NULL, NULL);
    if (window == NULL) {
        std::cout << "Errore creazione finestra" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Errore GLAD" << std::endl;
        return -1;
    }

    // Configurazione Pipeline
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_BLEND);      
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2. Shader
    Shader terrainShader("../shaders/terrain.vert", "../shaders/terrain.frag");
    Shader waterShader("../shaders/water.vert", "../shaders/water.frag");
    Shader cloudShader("../shaders/cloud.vert", "../shaders/cloud.frag");

    // 3. Terreno
    Terrain myTerrain(500, 500, 1.5f);
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, myTerrain.vertices.size() * sizeof(float), myTerrain.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myTerrain.indices.size() * sizeof(unsigned int), myTerrain.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Nuvole (semplice quad con shader di rumore)
    float cloudHeight = 0.0f;
    float cloudSize = 1200.0f;

    float cloudVertices[] = {
        -cloudSize, cloudHeight, -cloudSize,
        cloudSize, cloudHeight, -cloudSize,
        cloudSize, cloudHeight,  cloudSize,
        -cloudSize, cloudHeight,  cloudSize
    };

    unsigned int cloudIndices[] = {
        0,1,2,
        2,3,0
    };

    unsigned int cloudVAO, cloudVBO, cloudEBO;

    glGenVertexArrays(1, &cloudVAO);
    glGenBuffers(1, &cloudVBO);
    glGenBuffers(1, &cloudEBO);

    glBindVertexArray(cloudVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cloudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cloudIndices), cloudIndices, GL_STATIC_DRAW);

    // solo posizione (3 float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // 4. Griglia Acqua (Necessaria per le onde nel Vertex Shader)
    float waterLevel = 1.8f; 
    float waterSize = 600.0f; 
    int waterRes = 150; 
    float spacing = waterSize / (float)waterRes;

    std::vector<float> waterVertices;
    std::vector<unsigned int> waterIndices;

    for (int z = 0; z <= waterRes; z++) {
        for (int x = 0; x <= waterRes; x++) {
            float xPos = (x * spacing) - (waterSize / 2.0f);
            float zPos = (z * spacing) - (waterSize / 2.0f);
            waterVertices.push_back(xPos);
            waterVertices.push_back(waterLevel);
            waterVertices.push_back(zPos);
            waterVertices.push_back(0.0f); // Normale X
            waterVertices.push_back(1.0f); // Normale Y
            waterVertices.push_back(0.0f); // Normale Z
        }
    }

    for (int z = 0; z < waterRes; z++) {
        for (int x = 0; x < waterRes; x++) {
            int start = x + z * (waterRes + 1);
            waterIndices.push_back(start);
            waterIndices.push_back(start + 1);
            waterIndices.push_back(start + waterRes + 1);
            waterIndices.push_back(start + 1);
            waterIndices.push_back(start + waterRes + 2);
            waterIndices.push_back(start + waterRes + 1);
        }
    }

    unsigned int waterVAO, waterVBO, waterEBO;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glGenBuffers(1, &waterEBO);

    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, waterVertices.size() * sizeof(float), waterVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndices.size() * sizeof(unsigned int), waterIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 5. Loop di Rendering
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        float time = (float)glfwGetTime();
        
        // Ambiente fisso
        glm::vec3 skyColor = glm::vec3(0.4f, 0.7f, 1.0f);
        glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);

        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Telecamera
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 2000.0f);
        float camX = sin(time * 0.15f) * 350.0f;
        float camZ = cos(time * 0.15f) * 350.0f;
        glm::vec3 camPos = glm::vec3(camX, 100.0f, camZ);
        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f, 40.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Matrice Terreno
        glm::mat4 modelTerrain = glm::mat4(1.0f);
        modelTerrain = glm::translate(modelTerrain, glm::vec3(-(myTerrain.width * myTerrain.scale) / 2.0f, 0.0f, -(myTerrain.depth * myTerrain.scale) / 2.0f));

        // Terreno
        terrainShader.use();
        terrainShader.setMat4("projection", glm::value_ptr(projection));
        terrainShader.setMat4("view", glm::value_ptr(view));
        terrainShader.setMat4("model", glm::value_ptr(modelTerrain));

        terrainShader.setVec3("lightDir", lightDir);
        terrainShader.setVec3("lightColor", lightColor);

        terrainShader.setVec3("viewPos", camPos);
        terrainShader.setVec3("fogColor", skyColor);
        terrainShader.setFloat("fogDensity", 0.001f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)myTerrain.indices.size(), GL_UNSIGNED_INT, 0);

        // Nuvole
        cloudShader.use();
        cloudShader.setMat4("projection", glm::value_ptr(projection));
        cloudShader.setMat4("view", glm::value_ptr(view));
        cloudShader.setFloat("uTime", time);
        cloudShader.setVec3("skyColor", skyColor);
        cloudShader.setVec3("viewPos", camPos);

        glDepthMask(GL_FALSE); // Disabilita la scrittura della profondità per le trasparenze
        glBindVertexArray(cloudVAO);

        // Parametri per i Piani Multipli
        int numLayers = 8;           // Quanti strati sovrapporre
        float layerSpacing = 3.5f;   // Distanza in altezza tra uno strato e l'altro
        float startHeight = 110.0f;  // Altezza della base delle nuvole

        // Ciclo che disegna i piani dal basso verso l'alto
        for (int i = 0; i < numLayers; ++i) {
            // Calcola la percentuale di altezza (da 0.0 a 1.0)
            float layerFraction = (float)i / (float)(numLayers - 1); 
            float currentHeight = startHeight + (i * layerSpacing);

            // Sposta il piano all'altezza corrente
            glm::mat4 modelCloud = glm::mat4(1.0f);
            modelCloud = glm::translate(modelCloud, glm::vec3(camPos.x, currentHeight, camPos.z));
            cloudShader.setMat4("model", glm::value_ptr(modelCloud));

            // Invia i dati allo shader per ombreggiare e scolpire lo strato
            cloudShader.setFloat("uLayerFraction", layerFraction);
            cloudShader.setFloat("uLayerOffset", (float)i * 0.02f); // Leggero sfalsamento

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glDepthMask(GL_TRUE); // Riabilita la scrittura della profondità

        // Acqua
        waterShader.use();

        glm::mat4 modelWater = glm::mat4(1.0f);

        waterShader.setMat4("projection", glm::value_ptr(projection));
        waterShader.setMat4("view", glm::value_ptr(view));
        waterShader.setMat4("model", glm::value_ptr(modelWater));

        waterShader.setVec3("lightDir", lightDir);
        waterShader.setVec3("lightColor", lightColor);
        waterShader.setVec3("viewPos", camPos);
        waterShader.setFloat("uTime", time);
        waterShader.setFloat("waterLevel", waterLevel);

        waterShader.setVec3("viewPos", camPos);
        waterShader.setVec3("fogColor", skyColor);
        waterShader.setFloat("fogDensity", 0.001f);

        // trasparenza corretta
        glDepthMask(GL_FALSE);

        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)waterIndices.size(), GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &waterVAO); glDeleteBuffers(1, &waterVBO); glDeleteBuffers(1, &waterEBO);
    glDeleteVertexArrays(1, &cloudVAO); glDeleteBuffers(1, &cloudVBO); glDeleteBuffers(1, &cloudEBO);
    glfwTerminate();
    
    return 0;
}