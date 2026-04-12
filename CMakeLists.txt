cmake_minimum_required(VERSION 3.10)
project(ProceduralTerrain)

set(CMAKE_CXX_STANDARD 17)

# 1. Include the header directories (.h)
include_directories(include)

# 2. Tell CMake where to find the 'lib' folder containing libglfw3dll.a
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Find system OpenGL
find_package(OpenGL REQUIRED)

# Add source files (ATTENTION: glad.c is now required!)
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Terrain.cpp
    src/Terrain.h
    src/PerlinNoiseGenerator.h
    include/glad/glad.c 
)

add_executable(${PROJECT_NAME} ${SOURCES})

# 3. Link the libraries using "glfw3dll"
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw3)