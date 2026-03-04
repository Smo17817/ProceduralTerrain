#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform float uTime;
uniform vec3 skyColor;
uniform vec3 viewPos; // <- FONDAMENTALE per il fade

// hash
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main() {
    // 1. SCALA AUMENTATA: 0.004 invece di 0.0006 per vedere più nuvole
    // E muoviamole sia in X che in Z col tempo
    vec2 uv = WorldPos.xz * 0.004 + vec2(uTime * 0.02, uTime * 0.015);

    // FBM
    float n = 0.0;
    float amp = 0.5;
    float freq = 1.0;

    for(int i = 0; i < 5; i++) {
        n += noise(uv * freq) * amp;
        freq *= 2.0;
        amp *= 0.5;
    }

    // 2. SOGLIE ABBASSATE per avere nuvole più dense
    float clouds = smoothstep(0.3, 0.65, n);

    // 3. FADE corretto: distanza calcolata rispetto alla telecamera (viewPos)
    float radius = 600.0; 
    float dist = length(WorldPos.xz - viewPos.xz);
    float fade = 1.0 - smoothstep(radius * 0.5, radius * 0.95, dist);

    clouds *= fade;

    // Colore bianco puro, mescolato leggermente con il cielo ai bordi
    vec3 cloudColor = mix(vec3(1.0), skyColor, 0.1);
    float alpha = clouds * 0.85; // Opacità massima 85%

    if(alpha < 0.02)
        discard;

    FragColor = vec4(cloudColor, alpha);
}