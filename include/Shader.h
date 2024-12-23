// From LearnOpenGL repo, but added some modifications
// Link: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h

#pragma once

#include "Def.h"

class Shader {
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    // self-defined constructor, directly initialized from string content
    Shader(std::string vertexCode, std::string fragmentCode, std::string geometryCode = "");

    // activate the shader
    void use() { glUseProgram(ID); }
    // utility uniform functions
    void setBool(const std::string &name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
    void setInt(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
    void setFloat(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
    void setVec2(const std::string &name, const glm::vec2 &value) const { glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
    void setVec2(const std::string &name, float x, float y) const { glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); }
    void setVec3(const std::string &name, const glm::vec3 &value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
    void setVec3(const std::string &name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
    void setVec4(const std::string &name, const glm::vec4 &value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
    void setVec4(const std::string &name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); }
    void setMat2(const std::string &name, const glm::mat2 &mat) const { glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void setMat3(const std::string &name, const glm::mat3 &mat) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void setMat4(const std::string &name, const glm::mat4 &mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }

private:
    void checkCompileErrors(GLuint shader, std::string type); // utility function for checking shader compilation/linking errors
};

// Uniform Buffer Object 用于共享多个着色器之间的 uniform 变量，只需一次更新，所有着色器的变量都会一起更新，避免冗余代码
// note: the calculation pf size and offset is left to the user
class UBO {
private:
    GLuint UBO_ID = 0;
    GLuint bindingPoint;
    std::string blockName;
public:
    // constructor of UBO, define size of the buffer, binding point and block name
    UBO(GLsizeiptr size, GLuint bindingPoint, const std::string& blockName);
    ~UBO();

    void setData(GLintptr offset, GLsizeiptr size, const void* data); // note: the calculation pf size and offset is left to the user

    void Bind(const Shader& shader) const; // decide which shader the UBO is bound to
};