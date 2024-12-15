//
// Created by mf on 2024/12/11.
//

#pragma once

#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include "MyTexture.h"
#include "Def.h"
#include "Shader.h"

class Water {
private:
    GLuint VAO, VBO;
    const glm::vec2 mapScale; // size of the whole map
    const float heightScale;  // scale of the height
    const float height; // each river or lake has and has only one height
    const float* points;
    const int pointNum;
    Texture2D* dudvMap = nullptr, *normalMap = nullptr; // dudv map and normal map
public:
    Water(const glm::vec2 mapScale, const float heightScale, const float height, const float* points, const int pointNum);
    ~Water();

    void draw(Shader& shader, GLenum mode=GL_TRIANGLE_FAN) const;

    float getHeight() const { return height; }
    void RefractionPreProcess();
    void RefractionPostProcess();
    void ReflectionPreProcess();
    void ReflectionPostProcess();
};

// predefined river or lake
extern const int river_of_dead_num;
extern const float river_of_dead_height;
extern const float river_of_dead[]; // 40 is casually chosen
