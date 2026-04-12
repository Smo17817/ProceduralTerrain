#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int ID; // The ID of the shader program on the GPU

    // Constructor that reads and compiles the shaders
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // Activate the shader
    void use();
    
    // Functions to send data (Uniforms) to the shader
    void setMat4(const std::string &name, const float* mat) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setFloat(const std::string &name, float value) const;
};

#endif