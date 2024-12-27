#include "Water.h"

WaterFrameBuffer::WaterFrameBuffer(GLuint width, GLuint height) : FrameBuffer(width, height, FBO_CO_TEXT_DEPSTEN_RBO) {
    // 生成第两个纹理附件，用于反射（FrameBuffer 里面的那个用于折射）
    Texture2DBuffer2 = new Texture2D_attach();
    Texture2DBuffer2->Generate(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Texture2DBuffer2->ID, 0);
}

WaterFrameBuffer::~WaterFrameBuffer() {
    if (Texture2DBuffer2) delete Texture2DBuffer2;
}

void WaterFrameBuffer::BindTextureBuffer2(GLint unit) const {
    if (!Texture2DBuffer2)
        Err("No corresponding texture buffer in this framebuffer!");
    Texture2DBuffer2->Bind(unit);
}

void WaterFrameBuffer::UnBindTextureBuffer2() const {
    if (!Texture2DBuffer2)
        Err("No corresponding texture buffer in this framebuffer!");
    Texture2DBuffer2->UnBind();
}


Water::Water(const glm::vec2 mapScale, const float heightScale, const float height, const float* points, const int pointNum) : mapScale(mapScale), heightScale(heightScale), height(height), points(points), pointNum(pointNum) {
    // textures
    dudvMap = new Texture2D();
    normalMap = new Texture2D();
    dudvMap->Generate("resources/textures/water_dudv.jpg");
    normalMap->Generate("resources/textures/water_normal.jpg");

    // generate and bind buffer
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * pointNum, points, GL_STATIC_DRAW);
    // data and attribute: vertices, texCoords
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0);

    waters.push_back(this); // add to the list
}

Water::~Water() {
    if (dudvMap) delete dudvMap;
    if (normalMap) delete normalMap;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    for (auto it = waters.begin(); it != waters.end(); it++) { // remove from the list
        if (*it == this) {
            waters.erase(it);
            break;
        }
    }
}

void Water::draw(Shader &shader, GLenum mode) const {
    shader.use();
    shader.setVec2("mapScale", mapScale);
    shader.setFloat("height", height * heightScale);
    shader.setFloat("time", glfwGetTime());
    // set textures
    glBindVertexArray(VAO);
    glDrawArrays(mode, 0, pointNum);
    glBindVertexArray(0);
}

bool Water::checkInside(const float x, const float z) const {
    bool inside = false;
    for (int i = 0, j = pointNum - 1; i < pointNum; j = i++) {
        float xi = points[i * 4];
        float zi = points[i * 4 + 1];
        float xj = points[j * 4];
        float zj = points[j * 4 + 1];
        // 采用射线法遍历所有的边，如果
        //   1. z 值落在 i, j 的 z 坐标之间（那么就可能有交点）
        //   2. 向 x 正方向发射一条射线，如果交点的 x 坐标大于 x，那么就说明有交点
        //   此时，记一次交点个数
        // 最终如果交点个数是奇数，说明在内部
        if (((zi > z) != (zj > z)) && (x < (xj - xi) * (z - zi) / (zj - zi) + xi))
            inside = !inside;
    }
    return inside;
}

void Water::RefractionPreProcess() {
    glEnable(GL_CLIP_DISTANCE0); // 开启裁剪平面
    waterFBO->Bind();
    waterFBO->Clear(glm::vec4(0.18f, 0.2f, 0.18f, 1.0f), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waterFBO->Texture2DBuffer->ID, 0);
}

void Water::RefractionPostProcess() {
    glDisable(GL_CLIP_DISTANCE0);
    waterFBO->UnBind();
}

void Water::ReflectionPreProcess() {
    // glFrontFace(GL_CW);// in reflection pass, camera is mirrored, faces are mirrored too
    glDisable(GL_CULL_FACE); // 不知道为什么会有例外，干脆直接禁用算了
    glEnable(GL_CLIP_DISTANCE0);
    waterFBO->Bind();
    waterFBO->Clear(glm::vec4(0.18f, 0.2f, 0.18f, 1.0f), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waterFBO->Texture2DBuffer2->ID, 0);
}

void Water::ReflectionPostProcess() {
    // glFrontFace(GL_CCW);// in reflection pass, camera is mirrored, faces are mirrored too
    glEnable(GL_CULL_FACE);
    glDisable(GL_CLIP_DISTANCE0);
    waterFBO->UnBind();
}

// 遍历 waters，查看是否 (x,z) 落在水的区域内，如果是则返回水的高度，否则返回 -1
float checkHeight(const float worldX, const float worldZ) {
    float x = worldX / MAP_SZIE.x, z = worldZ / MAP_SZIE.y;
    for (auto water : waters) {
        if (water->checkInside(x, z)) // 假定一个点只会在一个水面上（water 不会重叠）
            return water->getHeight();
    }
    return -1.0f;
}

std::vector<Water*> waters; // 存储所有的水面

const int river_of_dead_num = 14;  // this is casually chosen
const float river_of_dead_height = 0.449f;
// const float river_of_dead_height = 0.6f;
const float river_of_dead[] = {
    // positions           // texCoords
    -0.257641f, 0.160123f, 0.522718f, 1.000000f,
    -0.196208f, 0.212426f, 0.661263f, 1.000000f,
    -0.130880f, 0.240105f, 0.843150f, 1.000000f,
    -0.090963f, 0.222242f, 1.000000f, 0.981994f,
    -0.067192f, 0.160380f, 1.000000f, 0.863666f,
    -0.049241f, 0.116434f, 1.000000f, 0.662314f,
    -0.084323f, 0.053296f, 1.000000f, 0.543970f,
    -0.177273f, -0.006323f, 1.000000f, 0.269460f,
    -0.217724f, -0.062214f, 0.788183f, 0.000000f,
    -0.297764f, -0.126270f, 0.432402f, 0.000000f,
    -0.404485f, -0.155284f, 0.165863f, 0.000000f,
    -0.459566f, -0.135117f, 0.000000f, 0.031073f,
    -0.455263f, -0.010098f, 0.000000f, 0.331472f,
    -0.341657f, 0.065866f, 0.198011f, 1.000000f,
};