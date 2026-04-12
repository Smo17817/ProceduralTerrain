#version 330 core

// Position attribute from the buffer
layout(location = 0) in vec3 aPos;

// Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Output to the fragment shader
out vec3 WorldPos;

void main() {
    // 1. Calculate the position in world space
    vec4 world = model * vec4(aPos, 1.0);
    WorldPos = world.xyz;
    
    // 2. Transform the position to clip space for rendering
    gl_Position = projection * view * world;
}