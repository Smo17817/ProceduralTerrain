#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Riceviamo le Normali!

out float Height;
out vec3 Normal;
out vec3 FragPos; // La posizione 3D del pixel nel mondo

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Height = aPos.y;
    // Calcoliamo la posizione del vertice nel mondo
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Passiamo la Normale corretta al Fragment Shader
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}