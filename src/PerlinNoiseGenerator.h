#ifndef PERLIN_NOISE_GENERATOR_H
#define PERLIN_NOISE_GENERATOR_H

#include <vector>
#include <numeric>
#include <random>
#include <cmath>
#include <algorithm>

class PerlinNoiseGenerator {
private:
    std::vector<int> p;

    float fade(float t) const {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    float lerp(float t, float a, float b) const {
        return a + t * (b - a);
    }

    float grad(int hash, float x, float y) const {
        int h = hash & 3;
        float u = h < 2 ? x : y;
        float v = h < 2 ? y : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    PerlinNoiseGenerator(unsigned int seed = 42) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        
        p.insert(p.end(), p.begin(), p.end());
    }

    // Base noise (single layer)
    float noise(float x, float y) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;

        x -= std::floor(x);
        y -= std::floor(y);

        float u = fade(x);
        float v = fade(y);

        int aa = p[p[X] + Y];
        int ab = p[p[X] + Y + 1];
        int ba = p[p[X + 1] + Y];
        int bb = p[p[X + 1] + Y + 1];

        float res = lerp(v, lerp(u, grad(aa, x, y), grad(ba, x - 1.0f, y)),
                            lerp(u, grad(ab, x, y - 1.0f), grad(bb, x - 1.0f, y - 1.0f)));
        
        return res;
    }

    // Fractal Brownian Motion (fBm) for realistic terrain
    float fractal(int octaves, float x, float y, float persistence = 0.5f, float lacunarity = 2.0f) const {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f;  // Used to normalize the result
        
        for(int i = 0; i < octaves; i++) {
            total += noise(x * frequency, y * frequency) * amplitude;
            
            maxValue += amplitude;
            
            amplitude *= persistence; // The weight of each layer decreases
            frequency *= lacunarity;  // The detail of each layer increases
        }
        
        return total / maxValue;
    }
};

#endif