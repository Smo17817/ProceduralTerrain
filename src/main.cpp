#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Librerie matematiche GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "Terrain.h"
#include "Shader.h" // La nostra nuova classe!

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
    // Evita che i triangoli lontani vengano disegnati sopra quelli vicini
    glEnable(GL_DEPTH_TEST);

    // 2. Carichiamo gli Shader
    // ATTENZIONE: Se esegui dalla cartella "build", i file shader si trovano nella cartella superiore
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

    // Modalità Wireframe (rimuovi il commento se vuoi vedere solo i contorni)
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 4. RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Ora puliamo sia il buffer del colore che quello della profondità
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f); // Un bel colore azzurro cielo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Attiviamo lo shader
        terrainShader.use();

        // Accendiamo il Sole!
        glm::vec3 lightDirection = glm::vec3(-0.5f, -1.0f, -0.5f); // Luce che arriva dall'alto e da destra
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f); // Luce solare leggermente calda
        
        glUniform3fv(glGetUniformLocation(terrainShader.ID, "lightDir"), 1, glm::value_ptr(lightDirection));
        glUniform3fv(glGetUniformLocation(terrainShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));

        // -- MATEMATICA DELLA TELECAMERA (MVP) --
        
        // A. Projection Matrix (Prospettiva, FOV, Aspect Ratio)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 1000.0f);
        
        // B. View Matrix (Posizione e direzione della telecamera)
        // Usiamo un po' di trigonometria per far ruotare la telecamera attorno al terreno col passare del tempo
        float time = glfwGetTime();
        float radius = 150.0f;
        float camX = sin(time * 0.5f) * radius;
        float camZ = cos(time * 0.5f) * radius;
        
        // Posizioniamo la telecamera in alto (Y=80) e la facciamo guardare verso il centro (0,0,0)
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 80.0f, camZ), 
                                    glm::vec3(0.0f, 0.0f, 0.0f), 
                                    glm::vec3(0.0f, 1.0f, 0.0f));

        // C. Model Matrix (Posizione del modello nel mondo)
        glm::mat4 model = glm::mat4(1.0f);
        // Il terreno parte da X=0 e Z=0. Lo trasliamo per centrarlo nell'origine (0,0,0)
        model = glm::translate(model, glm::vec3(-(myTerrain.width * myTerrain.scale) / 2.0f, 0.0f, -(myTerrain.depth * myTerrain.scale) / 2.0f));

        // Inviamo le matrici allo shader
        terrainShader.setMat4("projection", glm::value_ptr(projection));
        terrainShader.setMat4("view", glm::value_ptr(view));
        terrainShader.setMat4("model", glm::value_ptr(model));

        // -- DISEGNO DEL TERRENO --
        glBindVertexArray(VAO);
        // Parametri: Tipo di primitiva, numero di indici, tipo di dati degli indici, offset
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