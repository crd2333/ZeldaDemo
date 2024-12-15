#include "MyTexture.h"

// util function, check the image format and components
void CheckImageFormat(const std::string& path, GLuint& image_format, int components) {
    std::string format_str = image_format == GL_RED ? "gray" : image_format == GL_RGB ? "RGB" : "RGBA";
    if (components == 1) {
        if (image_format != GL_RED) {
            Warn("The image format for " + path + " is " + format_str + ", but is loaded as gray.");
            image_format = GL_RED;
        }
    } else if (components == 3) {
        if (image_format == GL_RGBA) {
            Warn("The image format for " + path + " is " + format_str + ", but is loaded as RGB.");
            image_format = GL_RGB;
        }
    } else if (components == 4) {
        if (image_format == GL_RGB) {
            Warn("The image format for " + path + " is " + format_str + ", but is loaded as RGBA.");
            image_format = GL_RGBA;
        }
    } else {
        Err("The image format for " + path + " is " + format_str + ", but the components is " + std::to_string(components) + " and cannot be loaded.");
    }
}


/* ---------- Texture2D class ---------- */
Texture2D::Texture2D() : flip_y(false), image_format(GL_RGB), internal_format(GL_RGB), data_type(GL_UNSIGNED_BYTE),
                wrap_s(GL_REPEAT), wrap_t(GL_REPEAT), filter_min(GL_LINEAR), filter_max(GL_LINEAR), mipmap(true) {
    glGenTextures(1, &ID);
}
Texture2D::~Texture2D() {
    glDeleteTextures(1, &ID);
}

// 根据设置的参数，利用 stb_image 读取图像数据并生成纹理
void Texture2D::Generate(std::string path) {
    glBindTexture(GL_TEXTURE_2D, ID);

    // read image data from file
    stbi_set_flip_vertically_on_load(flip_y);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &components, 0);
    if (__builtin_expect(!!data, 1)) {
        CheckImageFormat(path, image_format, components);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, data_type, data);
        if (mipmap) glGenerateMipmap(GL_TEXTURE_2D); // 生成 mipmap
    } else {
        Err("Failed to load texture for " + path);
    }
    stbi_image_free(data);

    // set the texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

// Binds the texture as the current active GL_TEXTURE_2D texture object
void Texture2D::Bind(GLint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}
void Texture2D::UnBind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}


/* ---------- CubeMap class ---------- */
CubeMap::CubeMap() : flip_y(false), image_format(GL_RGB), internal_format(GL_RGB), data_type(GL_UNSIGNED_BYTE),
                wrap_s(GL_CLAMP_TO_EDGE), wrap_t(GL_CLAMP_TO_EDGE), wrap_r(GL_CLAMP_TO_EDGE),
                filter_min(GL_LINEAR), filter_max(GL_LINEAR) {
    glGenTextures(1, &ID);
}
CubeMap::~CubeMap() {
    glDeleteTextures(1, &ID);
}

// 根据设置的参数，利用 stb_image 读取图像数据并生成纹理
void CubeMap::Generate(std::vector<std::string> paths) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    // read image data from file
    // order: +X(right), -X(left), +Y(top), -Y(bottom), +Z(front), -Z(back)
    stbi_set_flip_vertically_on_load(flip_y);
    for (unsigned int i = 0; i < paths.size(); i++) {
        unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &components, 0);
        if (__builtin_expect(!!data, 1)) {
            CheckImageFormat(paths[i], image_format, components);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, image_format, data_type, data);
        } else {
            Err("Failed to load cubemap texture for " + paths[i]);
        }
        stbi_image_free(data);
    }

    // set the texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);
}

// Binds the texture as the current active GL_TEXTURE_CUBE_MAP texture object
void CubeMap::Bind(GLint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}
void CubeMap::UnBind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


/* ---------- Texture2D_attach class ---------- */
Texture2D_attach::Texture2D_attach() : internal_format(GL_RGB), format(GL_RGB), data_type(GL_UNSIGNED_BYTE),
                        wrap_s(GL_CLAMP_TO_EDGE), wrap_t(GL_CLAMP_TO_EDGE),
                        filter_min(GL_LINEAR), filter_max(GL_LINEAR), Multisample(false) {
    glGenTextures(1, &ID);
}
Texture2D_attach::~Texture2D_attach() {
    glDeleteTextures(1, &ID);
}

// 生成一个空的纹理作为附件（只分配内存而没有填充，用于帧缓冲 framebuffer）
void Texture2D_attach::Generate(int width, int height) {
    glBindTexture(Multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, ID);
    if (Multisample)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_TRUE);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, data_type, NULL);
    // set the texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

// Binds the texture as the current active GL_TEXTURE_2D texture object
void Texture2D_attach::Bind(GLint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}
void Texture2D_attach::UnBind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}


/* ---------- CubeMap_attach class ---------- */
CubeMap_attach::CubeMap_attach() : internal_format(GL_RGB), format(GL_RGB), data_type(GL_UNSIGNED_BYTE),
                    wrap_s(GL_CLAMP_TO_EDGE), wrap_t(GL_CLAMP_TO_EDGE), wrap_r(GL_CLAMP_TO_EDGE),
                    filter_min(GL_LINEAR), filter_max(GL_LINEAR) {
    glGenTextures(1, &ID);
}
CubeMap_attach::~CubeMap_attach() {
    glDeleteTextures(1, &ID);
}

// 生成一个空的立方体贴图纹理作为附件（只分配内存而没有填充，用于帧缓冲 framebuffer）
void CubeMap_attach::Generate(int width, int height) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, data_type, NULL);
    // set the texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter_max);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);
}

// Binds the texture as the current active GL_TEXTURE_CUBE_MAP texture object
void CubeMap_attach::Bind(GLint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}
void CubeMap_attach::UnBind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}