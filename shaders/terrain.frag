#version 330 core
out vec4 FragColor;

in float Height;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    vec3 color;
    
    // DEFINIZIONE DEI BIOMI
    if (Height < 2.0) {
        // ABISSI: Più si scende (Height cala), più il colore diventa scuro
        float depthFactor = clamp((Height + 5.0) / 7.0, 0.0, 1.0); // Calcola la profondità
        vec3 deepBlue = vec3(0.01, 0.02, 0.1);  // Quasi nero
        vec3 surfaceBlue = vec3(0.1, 0.3, 0.6); // Blu superficiale
        color = mix(deepBlue, surfaceBlue, depthFactor);
    } 
    else if (Height < 4.0) {
        color = vec3(0.8, 0.7, 0.5); // Sabbia/Spiaggia
    } 
    else if (Height < 15.0) {
        color = vec3(0.3, 0.6, 0.3); // Erba
    } 
    else if (Height < 25.0) {
        color = vec3(0.5, 0.5, 0.5); // Roccia
    } 
    else {
        color = vec3(0.9, 0.9, 0.9); // Neve
    }

    // ILLUMINAZIONE
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); 
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}