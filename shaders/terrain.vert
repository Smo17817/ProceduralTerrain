#version 330 core
layout (location = 0) in vec3 aPos;

out float Height; // Passiamo l'altezza al fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Height = aPos.y; // Catturiamo l'altezza
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}