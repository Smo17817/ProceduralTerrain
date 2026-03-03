cmake_minimum_required(VERSION 3.10)
project(ProceduralTerrain)

set(CMAKE_CXX_STANDARD 17)

# Includi le cartelle per gli header
include_directories(include)

# Trova OpenGL e i pacchetti di sistema necessari
find_package(OpenGL REQUIRED)

# Assumiamo che tu abbia GLFW installato o compilato a parte
# find_package(glfw3 REQUIRED) # (Scommenta se usi un package manager come vcpkg o pacman)

# Aggiungi i file sorgente
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Terrain.cpp
    src/Terrain.h  
    src/PerlinNoiseGenerator.h   
    include/glad/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Linka le librerie
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)