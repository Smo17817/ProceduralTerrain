#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out float WaveHeight; // Passiamo l'altezza dell'onda per la spuma

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

void main() {
    vec3 pos = aPos;
    
    // Creiamo delle onde combinando più seni
    float wave1 = sin(pos.x * 0.2 + uTime * 1.5) * 0.5;
    float wave2 = cos(pos.z * 0.3 + uTime * 2.0) * 0.3;
    pos.y += wave1 + wave2;

    WaveHeight = wave1 + wave2; // Valore per capire dove c'è la cresta
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = aNormal; // In un sistema avanzato calcoleremmo la normale dell'onda
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}