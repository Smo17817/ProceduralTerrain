#version 330 core
out vec4 FragColor;

in float Height; // Altezza ricevuta dal Vertex Shader

void main() {
    vec3 color;
    
    // Colora in base all'altezza (Y)
    if (Height < 2.0) {
        color = vec3(0.2, 0.5, 0.8); // Acqua
    } else if (Height < 3.0) {
        color = vec3(0.8, 0.7, 0.5); // Sabbia
    } else if (Height < 8.0) {
        color = vec3(0.3, 0.6, 0.3); // Erba
    } else if (Height < 12.0) {
        color = vec3(0.5, 0.5, 0.5); // Roccia
    } else {
        color = vec3(0.9, 0.9, 0.9); // Neve
    }

    FragColor = vec4(color, 1.0);
}