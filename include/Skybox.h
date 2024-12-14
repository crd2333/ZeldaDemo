//
// Created by mf on 2024/12/5.
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "MyTexture.h"
#include "Shader.h"

class Skybox {
private:
    GLuint VAO = 0, VBO = 0;
    CubeMap* cube_map = nullptr;
    Shader* shader = nullptr;

public:
    Skybox(std::string path);
    ~Skybox();
    void draw(const glm::mat4 &projection, const glm::mat4 &view) const;

private:
    // 天空盒就只是一个立方体，直接硬编码在 code 里
    GLfloat skybox_vertices[3*6*6] = {
        // 天空盒直接把顶点坐标作为纹理坐标，故只用三个 float 表示一个顶点
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    // 直接将天空盒的 shader 硬编码在 code 中（使用原始字符串字面量）
    std::string skybox_vs = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            out vec3 TexCoords;
            uniform mat4 projection;
            uniform mat4 view;
            void main() {
                TexCoords = aPos;
                vec4 pos = projection * view * vec4(aPos, 1.0);
                gl_Position = pos.xyww; // 透视除法是在顶点着色器运行之后执行，这样取使得深度值永远为 1.0（即最远）
            }
        )";
    std::string skybox_fs = R"(
            #version 330 core
            out vec4 FragColor;
            in vec3 TexCoords; // 三维纹理坐标
            uniform samplerCube skybox;
            void main() {
                FragColor = texture(skybox, TexCoords);
            }
        )";
};
