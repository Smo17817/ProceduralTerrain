#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Apri i file
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        // Leggi il contenuto
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        vShaderFile.close();
        fShaderFile.close();
        
        // Converti in stringa
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERRORE::SHADER::FILE_NON_LETTO" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Compila Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERRORE::SHADER::VERTEX::COMPILAZIONE_FALLITA\n" << infoLog << std::endl;
    }

    // Compila Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERRORE::SHADER::FRAGMENT::COMPILAZIONE_FALLITA\n" << infoLog << std::endl;
    }

    // Collega gli shader al Programma
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERRORE::SHADER::PROGRAM::LINKING_FALLITO\n" << infoLog << std::endl;
    }

    // Elimina gli shader originali (ora sono dentro il programma)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() { 
    glUseProgram(ID); 
}

void Shader::setMat4(const std::string &name, const float* mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const { 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}