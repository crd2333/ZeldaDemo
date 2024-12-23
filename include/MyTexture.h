//
// Created by mf on 2024/12/5.
//

#pragma once

#include "Def.h"

class Texture2D {
public:
    GLuint ID = 0; // 每个纹理的唯一标识
    // image infomation
    GLint  width, height, components; // 图像的宽度、高度和通道数（由读取时 stb_image 返回）
    bool   flip_y;                    // 是否翻转图像的 y 轴
    GLuint image_format;              // 要读取的图像的格式
    // texture infomation
    GLuint internal_format; // 读取后纹理对象的格式
    GLuint data_type;       // 读取后纹理的数据格式，一般都是 GL_UNSIGNED_BYTE
    GLuint wrap_s;          // Wrapping mode on S axis (X axis)，一般是 GL_REPEAT 或 GL_CLAMP_TO_EDGE
    GLuint wrap_t;          // Wrapping mode on T axis
    GLuint filter_min;      // Filtering mode if texture pixels < screen pixels，一般是 GL_LINEAR 或 GL_NEAREST
    GLuint filter_max;      // Filtering mode if texture pixels > screen pixels
    bool   mipmap;          // 是否生成 mipmap

    Texture2D();
    ~Texture2D();

    void Generate(std::string path); // 根据设置的参数，利用 stb_image 读取图像数据并生成纹理

    void Bind(GLint unit = 0) const; // Binds the texture as the current active GL_TEXTURE_2D texture object
    void UnBind() const;
};


// 用于立方体贴图的纹理（如天空盒）
class CubeMap {
public:
    GLuint ID;
    // image infomation
    int    width, height, components;
    bool   flip_y;
    GLuint image_format;
    // texture infomation
    GLuint internal_format;
    GLuint data_type;
    GLuint wrap_s;
    GLuint wrap_t;
    GLuint wrap_r;
    GLuint filter_min;
    GLuint filter_max;

    CubeMap();
    ~CubeMap();

    void Generate(std::vector<std::string> paths); // 根据设置的参数，利用 stb_image 读取图像数据并生成纹理

    void Bind(GLint unit = 0) const; // Binds the texture as the current active GL_TEXTURE_CUBE_MAP texture object
    void UnBind() const;
};


// 作为帧缓冲附件的纹理
class Texture2D_attach {
public:
    GLuint ID;
    // texture infomation
    GLuint internal_format;
    GLuint format;
    GLuint data_type;
    GLuint wrap_s;
    GLuint wrap_t;
    GLuint filter_min;
    GLuint filter_max;
    bool   Multisample;
    int samples = -1;

    Texture2D_attach();
    ~Texture2D_attach();

    void Generate(int width, int height); // 生成一个空的纹理作为附件（只分配内存而没有填充，用于帧缓冲 framebuffer）

    void Bind(GLint unit = 0) const; // Binds the texture as the current active GL_TEXTURE_2D texture object
    void UnBind() const;
};

// 作为帧缓冲附件的立方体贴图纹理
class CubeMap_attach {
public:
    GLuint ID;
    // texture infomation
    GLuint internal_format;
    GLuint format;
    GLuint data_type;
    GLuint wrap_s;
    GLuint wrap_t;
    GLuint wrap_r;
    GLuint filter_min;
    GLuint filter_max;

    CubeMap_attach();
    ~CubeMap_attach();

    void Generate(int width, int height); // 生成一个空的立方体贴图纹理作为附件（只分配内存而没有填充，用于帧缓冲 framebuffer）

    void Bind(GLint unit = 0) const; // Binds the texture as the current active GL_TEXTURE_CUBE_MAP texture object
    void UnBind() const;
};
