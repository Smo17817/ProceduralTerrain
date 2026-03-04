#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// Le location da 2 a 5 sono occupate dalla mat4 dell'Instancing!
layout (location = 2) in mat4 aInstanceMatrix; 

out vec3 FragPos;
out vec3 Normal;
out float LocalY; // Ci serve per capire se stiamo colorando la base o la cima

uniform mat4 view;
uniform mat4 projection;

void main() {
    // Salviamo l'altezza prima che venga spostata nel mondo reale
    LocalY = aPos.y; 

    // La posizione 3D è calcolata usando la matrice specifica di questa istanza
    vec4 worldPos = aInstanceMatrix * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    // Ricalcoliamo la Normale in base a come abbiamo ruotato/scalato questo albero
    Normal = mat3(transpose(inverse(aInstanceMatrix))) * aNormal;
    
    gl_Position = projection * view * worldPos;
}