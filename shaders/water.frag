#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in float WaveHeight;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos; // Passa la posizione della camera dal main.cpp
uniform float uTime;

void main() {
    vec3 baseColor = vec3(0.0, 0.3, 0.5); // Blu profondo
    
    // 1. Spuma (Foam)
    // Se l'altezza dell'onda è sopra una certa soglia, schiariamo verso il bianco
    float foam = smoothstep(0.4, 0.8, WaveHeight);
    vec3 finalColor = mix(baseColor, vec3(0.9, 0.9, 1.0), foam);

    // 2. Riflesso Speculare (Brillio del Sole)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(normalize(lightDir), normalize(Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor * 0.8;

    // 3. Trasparenza dinamica
    float alpha = 0.6 + (foam * 0.3); // Più opaco dove c'è spuma

    FragColor = vec4(finalColor + specular, alpha);
}