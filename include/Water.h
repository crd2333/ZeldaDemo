//
// Created by mf on 2024/12/11.
//

#pragma once

#include "Def.h"
#include "MyTexture.h"
#include "Shader.h"
#include "FrameBuffer.h"

class WaterFrameBuffer : public FrameBuffer { // 继承 FrameBuffer，一个 FBO 拥有两个纹理附件
// private:
public:
    Texture2D_attach* Texture2DBuffer2 = nullptr; // 二号颜色缓冲

public:
    WaterFrameBuffer(GLuint width, GLuint height);
    ~WaterFrameBuffer();

    void BindTextureBuffer2(GLint unit = 0) const;
    void UnBindTextureBuffer2() const;
};

class Water {
private:
    GLuint VAO, VBO;
    const glm::vec2 mapScale; // size of the whole map
    const float heightScale;  // scale of the height
    const float height; // each river or lake has and has only one height
    const float* points;
    const int pointNum;
public:
    Texture2D* dudvMap = nullptr, *normalMap = nullptr; // dudv map and normal map
    WaterFrameBuffer* waterFBO = nullptr; // FBO for water reflection and refraction

    Water(const glm::vec2 mapScale, const float heightScale, const float height, const float* points, const int pointNum);
    ~Water();

    void draw(Shader& shader, GLenum mode=GL_TRIANGLE_FAN) const;

    float getHeight() const { return height * heightScale; }
    bool checkInside(const float x, const float z) const;
    void RefractionPreProcess();
    void RefractionPostProcess();
    void ReflectionPreProcess();
    void ReflectionPostProcess();
};

// 遍历 waters，查看是否 (x,z) 落在水的区域内，如果是则返回水的高度，否则返回 -1
float checkHeight(const float worldX, const float worldZ);

extern std::vector<Water*> waters; // 存储所有的水面

// predefined river or lake
extern const int river_of_dead_num;
extern const float river_of_dead_height;
extern const float river_of_dead[]; // 40 is casually chosen
