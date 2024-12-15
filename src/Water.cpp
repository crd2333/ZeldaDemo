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
}

Water::~Water() {
    if (dudvMap) delete dudvMap;
    if (normalMap) delete normalMap;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Water::draw(Shader& shader, GLenum mode) const {
    dudvMap->Bind(0);
    normalMap->Bind(1);
    shader.use();
    shader.setVec2("mapScale", mapScale);
    shader.setFloat("height", height * heightScale);
    shader.setFloat("time", glfwGetTime());
    // set textures
    glBindVertexArray(VAO);
    glDrawArrays(mode, 0, pointNum);
    glBindVertexArray(0);
}

void RefractionPreProcess() {

}

void RefractionPostProcess() {

}

void ReflectionPreProcess() {

}

void ReflectionPostProcess() {

}


const int river_of_dead_num = 21;  // this is casually chosen
const float river_of_dead_height = 0.445f;
const float river_of_dead[] = {
    -0.461886f, -0.097880f, 0.000000f, 0.162841f,
    -0.448966f, -0.149182f, 0.027833f, 0.000000f,
    -0.334954f, -0.141582f, 0.302114f, 0.000000f,
    -0.238605f, -0.119731f, 0.555724f, 0.000000f,
    -0.208094f, -0.062729f, 0.714749f, 0.000000f,
    -0.206489f, -0.006678f, 0.929277f, 0.000000f,
    -0.135030f, 0.032273f, 1.000000f, 0.414931f,
    -0.069994f, 0.059824f, 1.000000f, 0.517698f,
    -0.034667f, 0.111126f, 1.000000f, 0.629815f,
    -0.032258f, 0.190928f, 1.000000f, 0.805283f,
    -0.048316f, 0.241280f, 1.000000f, 0.948814f,
    -0.078024f, 0.250780f, 0.955075f, 1.000000f,
    -0.114957f, 0.252680f, 0.858178f, 1.000000f,
    -0.198460f, 0.211829f, 0.687247f, 1.000000f,
    -0.251451f, 0.167177f, 0.528171f, 1.000000f,
    -0.328530f, 0.122526f, 0.000000f, 0.990774f,
    -0.351012f, 0.057924f, 0.000000f, 0.525501f,
    -0.355829f, -0.004778f, 0.000000f, 0.204191f,
    -0.370282f, -0.042779f, 0.000000f, 0.073190f,
    -0.469039f, -0.077930f, 0.000000f, 0.189569f,
    -0.462689f, -0.097880f, 0.000000f, 0.164046f,
};