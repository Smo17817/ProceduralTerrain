#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID; // L'ID del programma shader sulla GPU

    // Costruttore che legge e compila gli shader
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // Attiva lo shader
    void use();
    
    // Funzioni per inviare dati (Uniforms) allo shader
    void setMat4(const std::string &name, const float* mat) const;
};

#endif