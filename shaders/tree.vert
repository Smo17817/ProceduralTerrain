#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// Locations 2 to 5 are occupied by the Instancing mat4!
layout (location = 2) in mat4 aInstanceMatrix; 

out vec3 FragPos;
out vec3 Normal;
out float LocalY; // We need this to know if we are coloring the base or the top

uniform mat4 view;
uniform mat4 projection;

void main() {
    // Save the height before it gets moved into the real world
    LocalY = aPos.y; 

    // The 3D position is calculated using the specific matrix of this instance
    vec4 worldPos = aInstanceMatrix * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    // Recalculate the Normal based on how we rotated/scaled this tree
    Normal = mat3(transpose(inverse(aInstanceMatrix))) * aNormal;
    
    gl_Position = projection * view * worldPos;
}