#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform float uTime;
uniform vec3 skyColor;
uniform vec3 viewPos;
uniform vec3 lightColor; 

uniform float uLayerFraction; 
uniform float uLayerOffset;   

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
    // 1. Coordinate Calculation and Cloud Animation
    vec2 uv = WorldPos.xz * 0.003 + vec2(uTime * 0.02, uTime * 0.015) + vec2(uLayerOffset);
    
    // 2. FBM (Fractal Brownian Motion) Generation for density
    float n = 0.0; 
    float amp = 0.5; 
    float freq = 1.0;
    for(int i = 0; i < 5; i++) {
        n += noise(uv * freq) * amp;
        freq *= 2.0; 
        amp *= 0.5;
    }

    // 3. Layer Shape Modeling (vertical rounding)
    float layerShape = sin(uLayerFraction * 3.14159); 
    float clouds = smoothstep(0.55 - (layerShape * 0.25), 0.75 - (layerShape * 0.15), n);

    // 4. Horizon Fade (based on distance from camera)
    float dist = length(WorldPos.xz - viewPos.xz);
    float fade = 1.0 - smoothstep(400.0, 1000.0, dist);
    clouds *= fade;

    // 5. Shading and Color
    // Ensure a minimum of night lighting so clouds don't disappear in the dark
    vec3 lightIntensity = max(lightColor, vec3(0.15)); 
    // Mix between sky color (shadow) and white (light) based on layer height
    vec3 cloudBaseColor = mix(skyColor * 0.8, vec3(1.0), uLayerFraction * 0.6 + 0.4);
    
    float alpha = clouds * 0.6 * layerShape;

    // If density is negligible, discard the fragment to optimize rendering
    if(alpha < 0.01) discard;

    FragColor = vec4(cloudBaseColor * lightIntensity, alpha);
}