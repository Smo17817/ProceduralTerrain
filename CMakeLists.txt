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
    src/PerlinNoiseGenator.h
    include/glad/glad.c # Ricordati di scaricare glad.c dal sito web di GLAD!
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Linka le librerie
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)