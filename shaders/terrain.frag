#version 330 core
out vec4 FragColor;

in float Height;

void main() {
    vec3 color;
    
    // Logica di base per il colore del bioma
    if (Height < -2.0) {
        color = vec3(0.1, 0.4, 0.8); // Acqua profonda (Blu)
    } else if (Height < 0.0) {
        color = vec3(0.2, 0.6, 0.9); // Acqua bassa (Azzurro)
    } else if (Height < 1.0) {
        color = vec3(0.9, 0.8, 0.5); // Sabbia (Giallo/Beige)
    } else if (Height < 6.0) {
        color = vec3(0.2, 0.7, 0.2); // Erba (Verde)
    } else if (Height < 8.5) {
        color = vec3(0.5, 0.4, 0.3); // Roccia (Marrone/Grigio)
    } else {
        color = vec3(0.95, 0.95, 0.95); // Neve (Bianco)
    }

    FragColor = vec4(color, 1.0);
}