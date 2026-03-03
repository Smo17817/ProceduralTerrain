#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    vec3 waterColor = vec3(0.0, 0.3, 0.5); // Blu oceanico
    
    // Illuminazione base
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    vec3 diffuse = diff * lightColor;

    // Colore finale con Alpha = 0.6 (60% opaco, 40% trasparente)
    FragColor = vec4((ambient + diffuse) * waterColor, 0.6);
}