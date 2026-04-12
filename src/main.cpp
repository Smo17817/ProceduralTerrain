#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM math libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include "Terrain.h"
#include "Shader.h" 

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// Callback for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Keyboard input handling
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// A handy struct to save the data
struct MeshData {
    std::vector<float> vertices;    // (X, Y, Z, Nx, Ny, Nz)
    std::vector<unsigned int> indices;
};

// Function that reads the .obj file
MeshData loadTreeModel(const std::string& path) {
    MeshData mesh;
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) std::cout << "TinyObjReader Error: " << reader.Error() << "\n";
        exit(1);
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    // Extract all triangles from the OBJ file
    unsigned int currentIndex = 0;
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            for (size_t v = 0; v < 3; v++) { // Assume the OBJ is triangulated
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // 1. Position (X, Y, Z)
                mesh.vertices.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
                mesh.vertices.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
                mesh.vertices.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);

                // 2. Normal (Nx, Ny, Nz)
                if (idx.normal_index >= 0) {
                    mesh.vertices.push_back(attrib.normals[3 * size_t(idx.normal_index) + 0]);
                    mesh.vertices.push_back(attrib.normals[3 * size_t(idx.normal_index) + 1]);
                    mesh.vertices.push_back(attrib.normals[3 * size_t(idx.normal_index) + 2]);
                } else {
                    mesh.vertices.push_back(0.0f); mesh.vertices.push_back(1.0f); mesh.vertices.push_back(0.0f);
                }

                mesh.indices.push_back(currentIndex++);
            }
            index_offset += 3;
        }
    }
    return mesh;
}

// Function to generate a procedural sphere
void createSphere(unsigned int& vao, unsigned int& vbo, unsigned int& ebo, int& indexCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int stacks = 20;
    int slices = 20;
    float radius = 1.0f;
    const float PI = 3.14159265f;

    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        float V = i / (float)stacks;
        float phi = V * PI;
        for (int j = 0; j <= slices; ++j) {
            float U = j / (float)slices;
            float theta = U * (PI * 2.0f);
            float x = radius * cos(theta) * sin(phi);
            float y = radius * cos(phi);
            float z = radius * sin(theta) * sin(phi);
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        }
    }
    // Generate indices
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            indices.push_back(i * (slices + 1) + j);
            indices.push_back((i + 1) * (slices + 1) + j);
            indices.push_back(i * (slices + 1) + j + 1);
            indices.push_back(i * (slices + 1) + j + 1);
            indices.push_back((i + 1) * (slices + 1) + j);
            indices.push_back((i + 1) * (slices + 1) + j + 1);
        }
    }
    indexCount = indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

int main() {
    // 1. GLFW Initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Procedural World", NULL, NULL);
    if (window == NULL) {
        std::cout << "Window creation error" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD error" << std::endl;
        return -1;
    }

    // Pipeline Configuration
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_BLEND);      
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2. Shaders
    Shader terrainShader("../shaders/terrain.vert", "../shaders/terrain.frag");
    Shader waterShader("../shaders/water.vert", "../shaders/water.frag");
    Shader cloudShader("../shaders/cloud.vert", "../shaders/cloud.frag");
    Shader treeShader("../shaders/tree.vert", "../shaders/tree.frag");
    Shader sunShader("../shaders/sun.vert", "../shaders/sun.frag");

    // 3. Terrain
    Terrain myTerrain(500, 500, 1.5f);

    // Load the tree model
    MeshData treeMesh = loadTreeModel("../assets/models/TreeLow.obj");

    unsigned int treeVAO, treeVBO, treeEBO, instanceVBO;
    glGenVertexArrays(1, &treeVAO);
    glGenBuffers(1, &treeVBO);
    glGenBuffers(1, &treeEBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(treeVAO);

    // Vertices VBO
    glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
    glBufferData(GL_ARRAY_BUFFER, treeMesh.vertices.size() * sizeof(float), treeMesh.vertices.data(), GL_STATIC_DRAW);

    // Indices EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, treeMesh.indices.size() * sizeof(unsigned int), treeMesh.indices.data(), GL_STATIC_DRAW);

    // Attribute 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Attribute 1: Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- INSTANCE VBO (The matrices to position the trees) ---
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, myTerrain.treeMatrices.size() * sizeof(glm::mat4), myTerrain.treeMatrices.data(), GL_STATIC_DRAW);

    // A mat4 matrix occupies 4 attribute locations (from 2 to 5)
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    // Tell OpenGL that these attributes change PER INSTANCE (not per vertex)
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
    
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

    // Clouds (simple quad with noise shader)
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

    // position only (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // 4. Water Grid (Needed for waves in the Vertex Shader)
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
            waterVertices.push_back(0.0f); // Normal X
            waterVertices.push_back(1.0f); // Normal Y
            waterVertices.push_back(0.0f); // Normal Z
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

    // --- Sun Sphere Setup ---
    unsigned int sunVAO, sunVBO, sunEBO;
    int sunIndexCount;
    createSphere(sunVAO, sunVBO, sunEBO, sunIndexCount);

    // 5. Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        float time = (float)glfwGetTime();
        
        // --- DYNAMIC LIGHTING (DAY/NIGHT CYCLE) ---
        float daySpeed = 0.05f; // MUCH slower (was 0.3f)

        // Add a small offset (0.2f) so it starts a bit above the horizon (dawn)
        float dayTime = (time * daySpeed); 
        
        glm::vec3 sunDir = glm::normalize(glm::vec3(
            sin(dayTime) * 0.3f, // Slight lateral shift
            sin(dayTime),        // This is the sun's height (Y)
            -cos(dayTime)        // Negative Z axis: it rises exactly in front of the camera!
        ));

        // Invert for OpenGL (light points TOWARDS objects)
        glm::vec3 lightDir = -sunDir; 
        
        float sunHeight = sunDir.y; // Goes from 1.0 (noon) to -1.0 (midnight)

        // Night Color Palette MUCH brighter and midnight blue
        glm::vec3 skyDay   = glm::vec3(0.4f, 0.7f, 1.0f);
        glm::vec3 skyDawn  = glm::vec3(0.8f, 0.4f, 0.2f);
        glm::vec3 skyNight = glm::vec3(0.08f, 0.12f, 0.25f); // Raised and bluer

        glm::vec3 lightDay   = glm::vec3(1.0f, 1.0f, 0.9f);
        glm::vec3 lightDawn  = glm::vec3(1.0f, 0.6f, 0.3f);
        glm::vec3 lightNight = glm::vec3(0.35f, 0.40f, 0.55f); // Lightened considerably
        
        glm::vec3 skyColor, lightColor;

        // Mix colors based on the sun's height
        if (sunHeight > 0.2f) { // Full day
            float t = glm::smoothstep(0.2f, 0.5f, sunHeight);
            skyColor = mix(skyDawn, skyDay, t);
            lightColor = mix(lightDawn, lightDay, t);
        } else if (sunHeight > -0.2f) { // Sunset
            float t = glm::smoothstep(-0.2f, 0.2f, sunHeight);
            skyColor = mix(skyNight, skyDawn, t);
            lightColor = mix(lightNight, lightDawn, t);
        } else { // Night
            skyColor = skyNight;
            lightColor = lightNight;
        }

        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 2000.0f);
        float camX = sin(time * 0.1f) * 350.0f;
        float camZ = cos(time * 0.1f) * 350.0f;
        glm::vec3 camPos = glm::vec3(camX, 100.0f, camZ);
        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f, 40.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Terrain Matrix
        glm::mat4 modelTerrain = glm::mat4(1.0f);
        modelTerrain = glm::translate(modelTerrain, glm::vec3(-(myTerrain.width * myTerrain.scale) / 2.0f, 0.0f, -(myTerrain.depth * myTerrain.scale) / 2.0f));

        // Terrain
        terrainShader.use();
        terrainShader.setMat4("projection", glm::value_ptr(projection));
        terrainShader.setMat4("view", glm::value_ptr(view));
        terrainShader.setMat4("model", glm::value_ptr(modelTerrain));

        terrainShader.setVec3("lightDir", lightDir);
        terrainShader.setVec3("lightColor", lightColor);

        terrainShader.setVec3("viewPos", camPos);
        terrainShader.setVec3("fogColor", skyColor);
        terrainShader.setFloat("fogDensity", 0.001f);
        
        // ADD THIS LINE TO MAKE THE SHADOWS MOVE!
        terrainShader.setFloat("uTime", time); 

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)myTerrain.indices.size(), GL_UNSIGNED_INT, 0);

        // --- 1. DRAW SUN (Opaque, very far away) ---
        sunShader.use();
        sunShader.setMat4("projection", glm::value_ptr(projection));
        sunShader.setMat4("view", glm::value_ptr(view));
        
        // Position the sun very far away
        glm::vec3 sunPos = camPos + sunDir * 800.0f; 
        glm::mat4 modelSun = glm::mat4(1.0f);
        modelSun = glm::translate(modelSun, sunPos);
        modelSun = glm::scale(modelSun, glm::vec3(40.0f)); 
        sunShader.setMat4("model", glm::value_ptr(modelSun));
        
        glm::vec3 sunColorDay = glm::vec3(1.0f, 1.0f, 0.9f);
        glm::vec3 sunColorDawn = glm::vec3(1.0f, 0.5f, 0.0f); 
        glm::vec3 actualSunColor = mix(sunColorDawn, sunColorDay, glm::smoothstep(0.0f, 0.3f, sunHeight));
        
        sunShader.setVec3("sunColor", actualSunColor);

        glBindVertexArray(sunVAO);
        glDrawElements(GL_TRIANGLES, sunIndexCount, GL_UNSIGNED_INT, 0);

        // --- 2.5D Clouds ---
        cloudShader.use();
        cloudShader.setMat4("projection", glm::value_ptr(projection));
        cloudShader.setMat4("view", glm::value_ptr(view));
        cloudShader.setFloat("uTime", time);
        cloudShader.setVec3("skyColor", skyColor);
        cloudShader.setVec3("viewPos", camPos);
        cloudShader.setVec3("lightColor", lightColor);

        glDepthMask(GL_FALSE); // Disable depth writing for transparencies
        glBindVertexArray(cloudVAO);

        // Parameters for Multiple Planes
        int numLayers = 15;           // How many layers to overlap
        float layerSpacing = 1.8f;   // Height distance between one layer and another
        float startHeight = 110.0f;  // Height of the cloud base

        // Loop that draws the planes from bottom to top
        for (int i = 0; i < numLayers; ++i) {
            // Calculate the height percentage (from 0.0 to 1.0)
            float layerFraction = (float)i / (float)(numLayers - 1); 
            float currentHeight = startHeight + (i * layerSpacing);

            // Move the plane to the current height
            glm::mat4 modelCloud = glm::mat4(1.0f);
            modelCloud = glm::translate(modelCloud, glm::vec3(camPos.x, currentHeight, camPos.z));
            cloudShader.setMat4("model", glm::value_ptr(modelCloud));

            // Send data to the shader to shade and sculpt the layer
            cloudShader.setFloat("uLayerFraction", layerFraction);
            cloudShader.setFloat("uLayerOffset", (float)i * 0.02f); // Slight offset

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glDepthMask(GL_TRUE); // Re-enable depth writing

        // --- Trees ---
        treeShader.use();
        treeShader.setMat4("projection", glm::value_ptr(projection));
        treeShader.setMat4("view", glm::value_ptr(view));
        // Note: we don't pass "model" because every tree has its own matrix inside aInstanceMatrix!
        
        treeShader.setVec3("lightDir", lightDir);
        treeShader.setVec3("lightColor", lightColor);
        treeShader.setVec3("viewPos", camPos);
        treeShader.setVec3("fogColor", skyColor);
        treeShader.setFloat("fogDensity", 0.001f); // Same fog as the terrain

        glBindVertexArray(treeVAO);
        // HERE IS THE MAGIC: Draw the tree N times!
        glDrawElementsInstanced(GL_TRIANGLES, treeMesh.indices.size(), GL_UNSIGNED_INT, 0, myTerrain.treeMatrices.size());

        // Water
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

        // correct transparency
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