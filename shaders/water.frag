#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float WaveHeight;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float waterLevel;
uniform float uTime;

//
// Simple pseudo-noise
//
float noise(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Diffuse
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular forte
    vec3 reflectDir = reflect(lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
    vec3 specular = spec * lightColor * 1.2;

    // Fresnel
    float fresnel = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0);

    // Colore acqua
    vec3 deepWater = vec3(0.0, 0.15, 0.35);
    vec3 shallowWater = vec3(0.0, 0.4, 0.6);

    float heightFactor = clamp((FragPos.y - waterLevel) * 0.1, 0.0, 1.0);
    vec3 waterColor = mix(deepWater, shallowWater, heightFactor);

    vec3 finalColor = waterColor * (0.4 + diffuse) + specular;
    finalColor = mix(finalColor, vec3(1.0), fresnel * 0.6);

    // FOAM LOGIC

    // Creste (normale meno verticale)
    float crestFactor = 1.0 - norm.y;

    // Altezza onda
    float heightFactorFoam = smoothstep(0.2, 0.6, WaveHeight - waterLevel);

    // Rumore animato
    float foamNoise = noise(FragPos.xz * 0.15 + uTime * 0.3);

    float foam = crestFactor * heightFactorFoam * foamNoise;

    foam = smoothstep(0.2, 0.6, foam);

    // Colore spuma
    vec3 foamColor = vec3(1.0);

    finalColor = mix(finalColor, foamColor, foam * 0.8);

    // Alpha dinamico
    float alpha = 0.65 + fresnel * 0.3;

    FragColor = vec4(finalColor, alpha);
}