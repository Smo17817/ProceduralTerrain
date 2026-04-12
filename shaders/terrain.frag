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

// New uniform to make the shadows scroll
uniform float uTime; 

// --- NOISE FUNCTIONS (Copied from the clouds) ---
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p); vec2 f = fract(p);
    float a = hash(i); float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0)); float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    vec3 color;
    
    // BIOME DEFINITION
    if (Height < 4.0) {
        vec3 deepSand = vec3(0.08, 0.07, 0.05);
        vec3 shallowSand = vec3(0.35, 0.32, 0.25);
        vec3 beachSand = vec3(0.82, 0.76, 0.62);
        float depthFactor = clamp((Height + 5.0) / 6.0, 0.0, 1.0);
        vec3 underwaterColor = mix(deepSand, shallowSand, depthFactor);
        float shoreFactor = smoothstep(1.0, 3.0, Height);
        color = mix(underwaterColor, beachSand, shoreFactor);
    } 
    else if (Height < 15.0) {
        vec3 beachSand = vec3(0.82, 0.76, 0.62);
        vec3 grass = vec3(0.25, 0.55, 0.28);
        float grassFactor = smoothstep(3.5, 6.0, Height);
        color = mix(beachSand, grass, grassFactor);
    } 
    else if (Height < 25.0) {
        vec3 grass = vec3(0.25, 0.55, 0.28);
        vec3 rock = vec3(0.45, 0.45, 0.47);
        float rockFactor = smoothstep(13.0, 17.0, Height);
        color = mix(grass, rock, rockFactor);
    } 
    else {
        vec3 rock = vec3(0.45, 0.45, 0.47);
        vec3 snow = vec3(0.95, 0.95, 0.98);
        float snowFactor = smoothstep(23.0, 26.0, Height);
        color = mix(rock, snow, snowFactor);
    }

    // --- CLOUD SHADOW CALCULATION ---
    // Same scale and speed used in cloud.frag
    vec2 uv = FragPos.xz * 0.003 + vec2(uTime * 0.02, uTime * 0.015);
    float n = 0.0; float amp = 0.5; float freq = 1.0;
    
    // 4 iterations are enough for the shadow (lighter on the GPU)
    for(int i = 0; i < 4; i++) {
        n += noise(uv * freq) * amp;
        freq *= 2.0; amp *= 0.5;
    }

    // If n is high, there is a dense cloud.
    float cloudDensity = smoothstep(0.45, 0.75, n); 
    
    // Create a shadow multiplier: 1.0 = Full sun, 0.4 = Dark shadow (max 60% darkness)
    float shadowFactor = 1.0 - (cloudDensity * 0.6); 

    // LIGHTING
    // At night the lunar ambient is sufficient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(-lightDir); 
    
    // Apply the sun (if below the horizon it will be 0 thanks to max)
    float diff = max(dot(norm, lightDirNormalized), 0.0);
    
    // HERE IS THE MAGIC: Multiply the direct light by the cloud shadow
    vec3 diffuse = diff * lightColor * shadowFactor; 

    vec3 result = (ambient + diffuse) * color;

    // FOG
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, result, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}