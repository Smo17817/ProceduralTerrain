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
        // FONDALE
        // Sabbia scura in profondità, sabbia chiara vicino alla superficie
        float depthFactor = clamp((Height + 5.0) / 7.0, 0.0, 1.0);

        vec3 deepSand = vec3(0.08, 0.07, 0.05);   // sabbia scura
        vec3 shallowSand = vec3(0.35, 0.32, 0.25); // sabbia chiara

        color = mix(deepSand, shallowSand, depthFactor);
    } 
    else if (Height < 4.0) {
        color = vec3(0.82, 0.76, 0.62); // Spiaggia più naturale
    } 
    else if (Height < 15.0) {
        color = vec3(0.25, 0.55, 0.28); // Erba meno saturata
    } 
    else if (Height < 25.0) {
        color = vec3(0.45, 0.45, 0.47); // Roccia più fredda
    } 
    else {
        color = vec3(0.95, 0.95, 0.98); // Neve leggermente fredda
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