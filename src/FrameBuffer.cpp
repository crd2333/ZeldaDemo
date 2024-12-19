#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(GLuint width, GLuint height, FBO_TYPE type) : Width(width), Height(height) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    if (type == FBO_COLOR_TEXT_ONLY) {
        Texture2DBuffer = new Texture2D_attach();
        Texture2DBuffer->Generate(width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture2DBuffer->ID, 0);
    } else if (type == FBO_DEPTH_TEXT_ONLY) {
        Texture2DBuffer = new Texture2D_attach();
        Texture2DBuffer->internal_format = GL_DEPTH_COMPONENT;
        Texture2DBuffer->format = GL_DEPTH_COMPONENT;
        Texture2DBuffer->data_type = GL_FLOAT;
        Texture2DBuffer->filter_min = GL_NEAREST;
        Texture2DBuffer->filter_max = GL_NEAREST;
        Texture2DBuffer->wrap_s = GL_CLAMP_TO_BORDER; // 设置纹理坐标裁剪到边界
        Texture2DBuffer->wrap_t = GL_CLAMP_TO_BORDER; // 让超出深度贴图的部分不会受到阴影影响
        Texture2DBuffer->Generate(width, height);
        glBindTexture(GL_TEXTURE_2D, Texture2DBuffer->ID);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // 设置边界颜色为白色，超出深度贴图的部分被设置为最大深度值 1.0
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture2DBuffer->ID, 0);
        glDrawBuffer(GL_NONE); // 显式声明不使用颜色数据进行渲染
        glReadBuffer(GL_NONE);
    } else if (type == FBO_DEPTH_CUBE_ONLY) {
        CubeBuffer = new CubeMap_attach();
        CubeBuffer->internal_format = GL_DEPTH_COMPONENT;
        CubeBuffer->format = GL_DEPTH_COMPONENT;
        CubeBuffer->data_type = GL_FLOAT;
        CubeBuffer->filter_min = GL_NEAREST;
        CubeBuffer->filter_max = GL_NEAREST;
        CubeBuffer->Generate(width, height);
        glBindTexture(GL_TEXTURE_CUBE_MAP, CubeBuffer->ID);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeBuffer->ID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else if (type == FBO_CO_TEXT_DEPSTEN_RBO) {
        Texture2DBuffer = new Texture2D_attach();
        Texture2DBuffer->Generate(width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture2DBuffer->ID, 0);
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        // 使用同一个 RBO，附加到帧缓冲的深度*和*模板附件上
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    } else if (type == FBO_CO_TEXT_DEPSTEN_RBO_MULTISAMPLE) {
        Texture2DBuffer = new Texture2D_attach();
        Texture2DBuffer->Multisample = true;
        Texture2DBuffer->samples = 4;
        Texture2DBuffer->Generate(width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, Texture2DBuffer->ID, 0);
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Texture2DBuffer->samples, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    } else {
        std::cout << "Framebuffer type not supported yet!" << std::endl;
        exit(-1);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete. Check framebuffers!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &FBO);
    if (Texture2DBuffer) delete Texture2DBuffer;
    if (CubeBuffer) delete CubeBuffer;
    if (RBO) glDeleteRenderbuffers(1, &RBO);
}

void FrameBuffer::Bind() const {
    glViewport(0, 0, Width, Height);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}
void FrameBuffer::UnBind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void FrameBuffer::Clear(glm::vec4 clearColor, GLbitfield mask) const {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(mask);
}
void FrameBuffer::Clear(GLbitfield mask) const {
    glClear(mask);
}

void FrameBuffer::Enable(GLbitfield mask) const {
    if (mask & GL_DEPTH_TEST) glEnable(GL_DEPTH_TEST);
    if (mask & GL_STENCIL_TEST) glEnable(GL_STENCIL_TEST);
}
void FrameBuffer::Disable(GLbitfield mask) const {
    if (mask & GL_DEPTH_TEST) glDisable(GL_DEPTH_TEST);
    if (mask & GL_STENCIL_TEST) glDisable(GL_STENCIL_TEST);
}

// 做类似于 glBlitNamedFramebuffer 的操作
void FrameBuffer::FromBlitTo(const FrameBuffer& draw_target, GLbitfield mask, GLuint filter) const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_target.FBO);
    glBlitFramebuffer(0, 0, Width, Height, 0, 0, draw_target.Width, draw_target.Height, mask, filter);
}

// bind texture for other shaders to use
void FrameBuffer::BindTextureBuffer(GLint unit) const {
    if (!Texture2DBuffer && !CubeBuffer)
        Err("No corresponding texture buffer in this framebuffer!");
    if (Texture2DBuffer)
        Texture2DBuffer->Bind(unit);
    else
        CubeBuffer->Bind(unit);
}
void FrameBuffer::UnBindTextureBuffer() const {
    if (!Texture2DBuffer && !CubeBuffer)
        Err("No corresponding texture buffer in this framebuffer!");
    if (Texture2DBuffer)
        Texture2DBuffer->UnBind();
    else
        CubeBuffer->UnBind();
}