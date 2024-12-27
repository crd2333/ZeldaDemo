#include "Light.h"

Light::Light(glm::vec3 lightColor, glm::vec3 lightPos, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular) :
    LightColor(lightColor), LightPos(lightPos), LightAmbient(lightAmbient), LightDiffuse(glm::vec3(lightDiffuse)), LightSpecular(lightSpecular) {}

void Light::load() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // aPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1); // aTexCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    sun_shader = new Shader("resources/billboard.vs", "resources/billboard.fs");
    sun_shader->use();
    sun_shader->setInt("billboard", 0); // 太阳的纹理（光晕）

    sun_texture = new Texture2D();
    sun_texture->image_format = GL_RGBA;
    sun_texture->internal_format = GL_SRGB_ALPHA;
    sun_texture->Generate("resources/textures/sun.png");
}

Light::~Light() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (sun_shader) delete sun_shader;
}

void Light::draw() const {
    sun_texture->Bind(0);
    sun_shader->use();
    sun_shader->setVec3("objPos", LightPos);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Light::SetUBO() const {
    uboLight->setData(0, sizeof(glm::vec3), glm::value_ptr(LightColor));
    uboLight->setData(sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(LightPos));
    uboLight->setData(2 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(LightAmbient));
    uboLight->setData(3 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(LightDiffuse));
    uboLight->setData(4 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(LightSpecular));
}

void Light::ImGuiLight() {
    ImGui::ColorEdit3WithDefault("Light Color", glm::value_ptr(LightColor), LIGHT_COLOR);
    ImGui::SliderFloat3("Light Direction", glm::value_ptr(LightPos), -1.0f, 1.0f);
    ImGui::SliderFloat3WithDefault("Light Ambient", glm::value_ptr(LightAmbient), 0.0f, 1.0f, LIGHT_AMBIENT);
    ImGui::SliderFloat3WithDefault("Light Diffuse", glm::value_ptr(LightDiffuse), 0.0f, 1.0f, LIGHT_DIFFUSE);
    ImGui::SliderFloat3WithDefault("Light Specular", glm::value_ptr(LightSpecular), 0.0f, 1.0f, LIGHT_SPECULAR);
}
