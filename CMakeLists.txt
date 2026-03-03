cmake_minimum_required(VERSION 3.10)
project(ProceduralTerrain)

set(CMAKE_CXX_STANDARD 17)

# 1. Includi le cartelle per gli header (.h)
include_directories(include)

# 2. Diciamo a CMake dove trovare la cartella 'lib' con dentro libglfw3dll.a
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Trova OpenGL di sistema
find_package(OpenGL REQUIRED)

# Aggiungi i file sorgente (ATTENZIONE: glad.c ora è obbligatorio!)
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Terrain.cpp
    src/Terrain.h
    src/PerlinNoiseGenerator.h
    include/glad/glad.c 
)

add_executable(${PROJECT_NAME} ${SOURCES})

# 3. Linka le librerie usando "glfw3dll"
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw3)