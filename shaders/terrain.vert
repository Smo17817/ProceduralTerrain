#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Receive the Normals!

out float Height;
out vec3 Normal;
out vec3 FragPos; // The 3D position of the pixel in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Height = aPos.y;
    // Calculate the vertex position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Pass the correct Normal to the Fragment Shader
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}