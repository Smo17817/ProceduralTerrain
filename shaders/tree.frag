#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float LocalY;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 viewPos;
uniform vec3 fogColor;
uniform float fogDensity;

void main() {
    // 1. PROCEDURAL COLORING (Trunk vs Leaves)
    vec3 trunkColor = vec3(0.35, 0.22, 0.15); // Wood brown
    vec3 leafColor = vec3(0.18, 0.45, 0.20);  // Forest green
    
    // Create a transition between trunk and leaves
    // NOTE: The values 0.5 and 1.5 depend on the dimensions of your "TreeLow.obj" file.
    // If the tree is entirely brown or entirely green, try raising or lowering these two numbers!
    float isLeaf = smoothstep(10.0, 120.0, LocalY); 
    vec3 objectColor = mix(trunkColor, leafColor, isLeaf);

    // 2. LIGHTING (Ambient + Diffuse)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); 
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;

    // 3. FOG (Same math as the terrain)
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, result, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}