#version 330 core
out vec4 FragColor;

in float Height;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightDir;   // Da dove arriva la luce del Sole
uniform vec3 lightColor; // Di che colore è la luce

void main() {
    // 1. Colore del Bioma (come avevamo prima)
    vec3 objectColor;
    if (Height < 2.0)      objectColor = vec3(0.2, 0.5, 0.8);
    else if (Height < 3.0) objectColor = vec3(0.8, 0.7, 0.5);
    else if (Height < 8.0) objectColor = vec3(0.3, 0.6, 0.3);
    else if (Height < 12.0)objectColor = vec3(0.5, 0.5, 0.5);
    else                   objectColor = vec3(0.9, 0.9, 0.9);

    // 2. Luce Ambientale (evita che le ombre siano nere al 100%)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // 3. Luce Diffusa (L'impatto diretto del Sole)
    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); 
    // Prodotto scalare tra la Normale e la Luce!
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    vec3 diffuse = diff * lightColor;

    // 4. Risultato finale: moltiplichiamo la luce per il colore del terreno
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}