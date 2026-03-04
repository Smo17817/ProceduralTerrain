#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform float uTime;
uniform vec3 skyColor;
uniform vec3 viewPos;

// Nuove variabili per la tridimensionalità
uniform float uLayerFraction; // Va da 0.0 (base) a 1.0 (cima)
uniform float uLayerOffset;   // Sposta leggermente il rumore per ogni strato

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
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    // Spostiamo le UV leggermente per ogni strato, per dare "volume"
    vec2 uv = WorldPos.xz * 0.003 + vec2(uTime * 0.02, uTime * 0.015) + vec2(uLayerOffset);

    // FBM
    float n = 0.0;
    float amp = 0.5;
    float freq = 1.0;

    for(int i = 0; i < 5; i++) {
        n += noise(uv * freq) * amp;
        freq *= 2.0;
        amp *= 0.5;
    }

    // SCULTURA 3D: La nuvola è più spessa al centro e svanisce in alto e in basso
    // sin(uLayerFraction * PI) crea un arco: 0 ai bordi, 1 al centro
    float layerShape = sin(uLayerFraction * 3.14159); 
    
    // Riduciamo la soglia al centro per far apparire più nuvola
    float thresholdMin = 0.55 - (layerShape * 0.25); 
    float thresholdMax = 0.75 - (layerShape * 0.15);
    float clouds = smoothstep(thresholdMin, thresholdMax, n);

    // Fading all'orizzonte (basato sulla telecamera)
    float radius = 600.0;
    float dist = length(WorldPos.xz - viewPos.xz);
    float fade = 1.0 - smoothstep(radius * 0.5, radius * 0.95, dist);
    clouds *= fade;

    // OMBREGGIATURA: La base della nuvola (uLayerFraction vicino a 0) sarà più scura!
    vec3 cloudBaseColor = mix(skyColor * 0.6, vec3(1.0), uLayerFraction * 0.8 + 0.2);
    
    // AUMENTIAMO L'OPACITÀ:
    // Prima moltiplicavamo per 0.25. Alziamo il valore a 0.6 (o anche 0.8) 
    // per rendere ogni singolo strato di nuvola molto più "denso" e visibile.
    float alpha = clouds * 0.6 * layerShape; 

    if(alpha < 0.02)
        discard;

    FragColor = vec4(cloudBaseColor, alpha);
}