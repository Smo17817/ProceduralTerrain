# ProceduralTerrain
## [a.a. 2025/26] TDT4230 - Graphics&Visualizzation

A fully procedural, real-time 3D terrain generator built from scratch using Modern OpenGL (Core Profile 3.3) and C++. This project showcases advanced rendering techniques, procedural generation algorithms, and custom GLSL shaders to create a living, breathing environment complete with dynamic lighting, moving water, and instanced forests.

## ✨ Features
This project implements a variety of advanced graphics programming techniques across multiple custom shaders:
- 🌎 Procedural Terrain Generation
    - Generated using Fractal Brownian Motion (fBm) and Perlin Noise.
    - Procedural biome coloring based on elevation and normal slopes (Deep water, Sand, Grass, Rock, Snow).
    - Smooth transitions between biomes using smoothstep.
- 🌲 High-Performance Instanced Forest
    - Trees are populated procedurally based on terrain height and slope steepness.
    - Uses Instanced Rendering (glDrawElementsInstanced) to render thousands of trees in a single draw call.
    - Randomized per-instance scaling, rotation, and sub-grid offsets for a natural look.
- 🌊 Dynamic Water System
    - Multi-directional Gerstner Waves calculated in the Vertex Shader.
    - Advanced Fragment Shader logic featuring Fresnel effects, strong specular highlights, and depth-based water color tinting.
    - Dynamic procedural foam that gathers at wave crests.
- ☁️ 2.5D Volumetric Clouds & Shadows
    - Layered procedural noise planes (FBM) to simulate cloud depth and volume.
    - Clouds animate over time to simulate wind.
    - Clouds cast real-time, scrolling procedural shadows onto the terrain and biomes below.
- ☀️ Dynamic Day/Night Cycle
    - A procedural sphere acts as the sun, orbiting the scene.
    - Smooth interpolation of sky, light, and fog colors (Dawn -> Day -> Dusk -> Night).
    - Ambient and Diffuse lighting intensities scale naturally with the sun's position.
- 🎥 Cinematic Auto-Orbit Camera
    - The camera automatically orbits the terrain over time, providing a smooth, continuous showcase of the environment and lighting changes.
- 🌫️ Distance Fog
    - Exponential depth fog applied across all shaders (Terrain, Water, Trees) to blend the horizon and add atmospheric depth.

## 🛠️ Technical Stack & Dependencies
The project uses the following libraries:
- GLFW: Window creation and context handling.
- GLAD: OpenGL function pointer loading.
- GLM: OpenGL Mathematics library.
- TinyObjLoader: For loading the low-poly 3D tree model.

## 🚀 Build & Run Instructions
This project uses CMake as its build system.

Prerequisites
A C++17 compatible compiler (GCC, Clang, or MSVC).

CMake (version 3.10 or higher).

System-wide OpenGL drivers.

Compilation Steps (Linux / macOS / Windows with MinGW)
Clone the repository:

Bash
git clone https://github.com/YourUsername/ProceduralTerrain.git
cd ProceduralTerrain
Create a build directory and configure the project:

Bash
mkdir build
cd build
cmake ..
(If you are on Windows using MinGW, force the generator: cmake -G "MinGW Makefiles" ..)

Compile the project:

Bash
cmake --build .
Run the executable:

Bash
./ProceduralTerrain

### Project Structure
ProceduralTerrain/
├── assets/
│   └── models/
│       └── TreeLow.obj          # 3D Model for instancing
├── include/
│   ├── glad/                    # GLAD headers and .c file
│   ├── GLFW/                    # GLFW headers
│   ├── glm/                     # GLM math headers
│   └── tiny_obj_loader.h        # 3D model loader
├── lib/                         # Pre-compiled libraries (e.g., libglfw3dll.a)
├── shaders/
│   ├── terrain.vert / .frag     # Terrain shaders
│   ├── water.vert / .frag       # Water shaders
│   ├── tree.vert / .frag        # Instanced tree shaders
│   ├── cloud.vert / .frag       # 2.5D clouds shaders
│   └── sun.vert / .frag         # Sun sphere shaders
├── src/
│   ├── main.cpp                 # Application entry point & render loop
│   ├── Shader.cpp / .h          # Shader compilation and linking utility
│   ├── Terrain.cpp / .h         # Terrain mesh and forest generation
│   └── PerlinNoiseGenerator.h   # Custom Perlin/fBm noise implementation
└── CMakeLists.txt               # Build configuration

## 🎮 Controls
- ESC: Close the application.

Note: Camera movement and Day/Night progression are fully automated to showcase the environment.

