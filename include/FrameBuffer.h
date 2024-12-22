//
// Created by mf on 2024/12/5.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "MyTexture.h"
#include "Def.h"

// 帧缓冲对象的抽象封装
// 一个帧缓冲对象包含三个子部分：color buffer、depth buffer 和 stencil buffer
//     其中 color buffer 是必须的否则不完整，但可以显式声明不使用任何颜色数据进行渲染
//     后二者可以结合成 depth-stencil buffer 以节省显存
// 每种 buffer 可以是一个 texture attachment 或 renderbuffer object(RBO)
//     理论上来说都做成 texture 也是可以的，但 RBO 作为非通用数据缓冲专门被引入 OpenGL，其性能更好
//     但是 RBO 不能被采样，它是只写的，适用于需要深度和模板值用于测试的场合，如果需要采样就只能用 texture
// 一个完整的 FBO 必须：附加至少一个 buffer，至少有一个 color attachment，所有的 attachment 都是完整的（分配了内存），每个 buffer 都应该有相同的样本数

// 这里我预定义几种常见的 FBO 类型进行封装
enum FBO_TYPE {
    FBO_COLOR_TEXT_ONLY, // 只使用纹理作为附件的颜色缓冲，如《learnOpenGL-抗锯齿》小节中的 intermediateFBO
    FBO_DEPTH_TEXT_ONLY, // 只使用纹理作为附件的深度缓冲，如《learnOpenGL-阴影映射》小节中的 depthMapFBO
    FBO_DEPTH_CUBE_ONLY, // 只使用立方体纹理作为附件的深度缓冲，如《learnOpenGL-点阴影》小节中的 depthMapFBO
    FBO_CO_TEXT_DEPSTEN_RBO,             // 颜色缓冲使用纹理，深度和模板缓冲使用 RBO，如《learnOpenGL-帧缓冲》小节中的 FBO
    FBO_CO_TEXT_DEPSTEN_RBO_MULTISAMPLE, // 颜色缓冲使用纹理，深度和模板缓冲使用 RBO，且使用多重采样，如《learnOpenGL-抗锯齿》小节中的 framebuffer
};

// basic abstraction of framebuffer object
class FrameBuffer {
public:
    FrameBuffer(GLuint width, GLuint height, FBO_TYPE type);
    virtual ~FrameBuffer();

    void Bind() const;
    void UnBind() const;

    void Clear(glm::vec4 clearColor = glm::vec4(0.0f), GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) const;
    void Clear(GLbitfield mask = GL_DEPTH_BUFFER_BIT) const; // cpp 只能从右到左依次省略参数，不能跳过，只能重载

    void Enable(GLbitfield mask = GL_DEPTH_TEST) const;
    void Disable(GLbitfield mask = GL_DEPTH_TEST) const;

    // 做类似于 glBlitNamedFramebuffer 的操作
    void FromBlitTo(const FrameBuffer& draw_target, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLuint filter = GL_NEAREST) const;

    // bind texture for other shaders to use
    void BindTextureBuffer(GLint unit = 0) const;
    void UnBindTextureBuffer() const;

// private:
public:
    GLuint FBO; // 帧缓冲对象
    GLuint Width, Height; // 帧缓冲的宽高
    GLuint RBO; // 渲染缓冲对象
    Texture2D_attach* Texture2DBuffer = nullptr; // 颜色或深度纹理缓冲
    CubeMap_attach*  CubeBuffer = nullptr;    // 立方体纹理缓冲
};
