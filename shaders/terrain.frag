#version 330 core
out vec4 FragColor;

in float Height;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 viewPos;
uniform vec3 fogColor;
uniform float fogDensity;

void main() {
    vec3 color;
    
    // DEFINIZIONE DEI BIOMI
    if (Height < 4.0) {
        // 1. Definiamo i tre colori della zona costiera
        vec3 deepSand = vec3(0.08, 0.07, 0.05);   // Abissi
        vec3 shallowSand = vec3(0.35, 0.32, 0.25); // Sabbia bagnata (sotto il pelo dell'acqua)
        vec3 beachSand = vec3(0.82, 0.76, 0.62);   // Spiaggia asciutta

        // 2. Calcoliamo la sfumatura per gli abissi (da -5.0 a 1.0 di altezza)
        float depthFactor = clamp((Height + 5.0) / 6.0, 0.0, 1.0);
        vec3 underwaterColor = mix(deepSand, shallowSand, depthFactor);

        // 3. Sfuma dolcemente tra la sabbia bagnata e la spiaggia asciutta (tra altezza 1.0 e 3.0)
        float shoreFactor = smoothstep(1.0, 3.0, Height);
        
        // Applica il mix finale
        color = mix(underwaterColor, beachSand, shoreFactor);
    } 
    else if (Height < 15.0) {
        // Transizione morbida anche tra spiaggia ed erba (opzionale, ma consigliato!)
        vec3 beachSand = vec3(0.82, 0.76, 0.62);
        vec3 grass = vec3(0.25, 0.55, 0.28);
        float grassFactor = smoothstep(3.5, 6.0, Height);
        color = mix(beachSand, grass, grassFactor);
    } 
    else if (Height < 25.0) {
        // Transizione erba -> roccia
        vec3 grass = vec3(0.25, 0.55, 0.28);
        vec3 rock = vec3(0.45, 0.45, 0.47);
        float rockFactor = smoothstep(13.0, 17.0, Height);
        color = mix(grass, rock, rockFactor);
    } 
    else {
        // Transizione roccia -> neve
        vec3 rock = vec3(0.45, 0.45, 0.47);
        vec3 snow = vec3(0.95, 0.95, 0.98);
        float snowFactor = smoothstep(23.0, 26.0, Height);
        color = mix(rock, snow, snowFactor);
    }

    // ILLUMINAZIONE
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); 
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * color;

    // FOG
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, result, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}