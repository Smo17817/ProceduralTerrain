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

uniform vec3 fogColor;
uniform float fogDensity;

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

    // Strong specular
    vec3 reflectDir = reflect(lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
    vec3 specular = spec * lightColor * 1.2;

    // Fresnel
    float fresnel = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0);

    // Water color
    vec3 deepWater = vec3(0.0, 0.15, 0.35);
    vec3 shallowWater = vec3(0.0, 0.4, 0.6);

    float heightFactor = clamp((FragPos.y - waterLevel) * 0.1, 0.0, 1.0);
    vec3 waterColor = mix(deepWater, shallowWater, heightFactor);

    vec3 finalColor = waterColor * (0.4 + diffuse) + specular;
    finalColor = mix(finalColor, vec3(1.0), fresnel * 0.6);

    // FOAM LOGIC

    // Crests (normal less vertical)
    float crestFactor = 1.0 - norm.y;

    // Wave height
    float heightFactorFoam = smoothstep(0.2, 0.6, WaveHeight - waterLevel);

    // Animated noise
    float foamNoise = noise(FragPos.xz * 0.15 + uTime * 0.3);

    float foam = crestFactor * heightFactorFoam * foamNoise;

    foam = smoothstep(0.2, 0.6, foam);

    // Foam color
    vec3 foamColor = vec3(1.0);

    finalColor = mix(finalColor, foamColor, foam * 0.8);

    // Dynamic alpha
    float alpha = 0.65 + fresnel * 0.3;

    // FOG
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 foggedColor = mix(fogColor, finalColor, fogFactor);

    FragColor = vec4(foggedColor, alpha);
}