#include "Water.h"

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

void Water::draw(Shader& shader, GLenum mode) const {
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

}

void RefractionPreProcess() {

}

void RefractionPostProcess() {

}

void ReflectionPreProcess() {

}

void ReflectionPostProcess() {

}

// 遍历 waters，查看是否 (x,z) 落在水的区域内，如果是则返回水的高度，否则返回 -1
float checkHeight(const float x, const float z) {

}

std::vector<Water*> waters; // 存储所有的水面

const int river_of_dead_num = 14;  // this is casually chosen
const float river_of_dead_height = 0.449f;
// const float river_of_dead_height = 0.6f;
const float river_of_dead[] = {
    -0.267641f, 0.210123f, 0.522718f, 1.000000f,
    -0.196208f, 0.282426f, 0.661263f, 1.000000f,
    -0.080880f, 0.320105f, 0.843150f, 1.000000f,
    -0.030963f, 0.272242f, 1.000000f, 0.981994f,
    -0.017192f, 0.224380f, 1.000000f, 0.863666f,
    -0.029241f, 0.116434f, 1.000000f, 0.662314f,
    -0.084323f, 0.053296f, 1.000000f, 0.543970f,
    -0.177273f, -0.006323f, 1.000000f, 0.269460f,
    -0.217724f, -0.062214f, 0.788183f, 0.000000f,
    -0.297764f, -0.126270f, 0.432402f, 0.000000f,
    -0.404485f, -0.155284f, 0.165863f, 0.000000f,
    -0.459566f, -0.135117f, 0.000000f, 0.031073f,
    -0.455263f, -0.023098f, 0.000000f, 0.331472f,
    -0.341657f, 0.145866f, 0.198011f, 1.000000f,
};