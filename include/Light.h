//
// Created by mf on 2024/12/12.
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Def.h"
#include "Shader.h"
#include "GameWindow.h"

// default light values
#define LIGHT_COLOR glm::vec3(1.0f)
// #define LIGHT_DIRECTION glm::vec3(0.43f, 0.76f, -0.33f)
#define LIGHT_POS glm::vec3(0.0f, 300.0f, 400.0f)
#define LIGHT_AMBIENT 0.2f
#define LIGHT_DIFFUSE 1.0f
#define LIGHT_SPECULAR 1.0f

class Light {
public:
    // light attributes
    glm::vec3 LightColor;
    glm::vec3 LightPos;  // the sun will be put very high as if it is infinitely far away. // use parallel light instead
    // glm::vec3 LightDirection;
    glm::vec3 LightAmbient;
    glm::vec3 LightDiffuse;
    glm::vec3 LightSpecular;
    // float AttenuationConstant;
    // float AttenuationLinear;
    // float AttenuationQuadratic;
    // other attributes
    UBO* uboLight = nullptr; // UBO for light, remember to set it before using!
    Shader* sun_shader = nullptr;
    GLuint VAO = 0, VBO = 0;
    GLfloat vertices[16] = {
        // Positions  // TexCoords
        -0.5f, -0.5f, 0.0f,  0.0f,
        0.5f, -0.5f,  1.0f,  0.0f,
        -0.5f,  0.5f, 0.0f,  1.0f,
        0.5f,  0.5f,  1.0f,  1.0f,
    };
    Texture2D* sun_texture = nullptr;

    Light(glm::vec3 lightColor=LIGHT_COLOR, glm::vec3 lightPos=LIGHT_POS,
          glm::vec3 lightAmbient=glm::vec3(LIGHT_AMBIENT), glm::vec3 lightDiffuse=glm::vec3(LIGHT_DIFFUSE), glm::vec3 lightSpecular=glm::vec3(LIGHT_SPECULAR));
    virtual ~Light();

    void load();
    void draw() const;

    void BindUBO(const Shader& shader) const;
    void SetUBO() const;

    void ImGuiLight();
};

extern Light sun;
