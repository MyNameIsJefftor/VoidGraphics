#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	//Program ID
	unsigned int ID;
	Shader(const char* vertexPath, const char* fragPath, const char* geoPath);
    Shader(const char* vertexPath, const char* fragPath);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
    void setMat(const std::string& name, glm::mat4 value) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
};

Shader::Shader(const char* vertexPath, const char* fragPath, const char* geoPath)
{
    //retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragPath);
        gShaderFile.open(geoPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // read file into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        // close file
        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        // convert stream -> string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometryCode = gShaderStream.str();
    }

    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();

    unsigned int vertex, frag, geom;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Vertex < CompFail\n" << infoLog << std::endl;
    };

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fShaderCode, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Frag < CompFail\n" << infoLog << std::endl;
    };
    
    geom = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geom, 1, &gShaderCode, NULL);
    glCompileShader(geom);

    glGetShaderiv(geom, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geom, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Frag < CompFail\n" << infoLog << std::endl;
    };

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, frag);
    glAttachShader(ID, geom);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Program < LinkFail\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);
    glDeleteShader(geom);
};

Shader::Shader(const char* vertexPath, const char* fragPath)
{
    //retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // read file into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file
        vShaderFile.close();
        fShaderFile.close();
        // convert stream -> string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, frag;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Vertex < CompFail\n" << infoLog << std::endl;
    };

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fShaderCode, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Frag < CompFail\n" << infoLog << std::endl;
    };

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, frag);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERR : Shader < Program < LinkFail\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);
};

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);

}
void Shader::setMat(const std::string& name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));

}
void Shader::setVec3(const std::string& name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));

}
#endif

