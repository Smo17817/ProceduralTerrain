#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float uTime;

out vec3 FragPos;
out vec3 Normal;
out float WaveHeight;

const float PI = 3.14159265;

// Wave parameters
vec3 gerstnerWave(vec3 pos, vec2 dir, float steepness, float wavelength, float speed)
{
    float k = 2.0 * PI / wavelength;
    float c = speed;
    float f = k * dot(dir, pos.xz) - c * uTime;
    float a = steepness / k;

    pos.x += dir.x * a * cos(f);
    pos.z += dir.y * a * cos(f);
    pos.y += a * sin(f);

    return pos;
}

vec3 computeNormal(vec3 pos, vec2 dir, float steepness, float wavelength, float speed)
{
    float k = 2.0 * PI / wavelength;
    float c = speed;
    float f = k * dot(dir, pos.xz) - c * uTime;
    float a = steepness / k;

    float dx = dir.x * steepness * cos(f);
    float dz = dir.y * steepness * cos(f);

    vec3 tangent = normalize(vec3(1.0 - dx, dir.x * steepness * sin(f), -dz));
    vec3 bitangent = normalize(vec3(-dx, dir.y * steepness * sin(f), 1.0 - dz));

    return normalize(cross(bitangent, tangent));
}

void main()
{
    vec3 pos = aPos;

    vec2 dir1 = normalize(vec2(1.0, 0.5));
    vec2 dir2 = normalize(vec2(-0.7, 0.3));
    vec2 dir3 = normalize(vec2(0.2, -1.0));

    pos = gerstnerWave(pos, dir1, 0.15, 60.0, 1.2);
    pos = gerstnerWave(pos, dir2, 0.10, 35.0, 1.6);
    pos = gerstnerWave(pos, dir3, 0.05, 20.0, 2.0);

    vec3 normal = computeNormal(aPos, dir1, 0.15, 60.0, 1.2);
    normal += computeNormal(aPos, dir2, 0.10, 35.0, 1.6);
    normal += computeNormal(aPos, dir3, 0.05, 20.0, 2.0);
    normal = normalize(normal);

    WaveHeight = pos.y;

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}