#include "Light.h"

Light::Light(glm::vec3 LightColor, glm::vec3 lightDirection, glm::vec3 lightAmbient, glm::vec3 lightDiffuse, glm::vec3 lightSpecular) :
  LightColor(LightColor), LightDirection(lightDirection),
  LightAmbient(lightAmbient), LightDiffuse(glm::vec3(lightDiffuse)), LightSpecular(lightSpecular) {}

void Light::BindUBO(const Shader& shader) const {
    UBOLight->Bind(shader);
}

void Light::SetUBO() const {
    UBOLight->setData(0, sizeof(glm::vec3), glm::value_ptr(LightColor));
    UBOLight->setData(sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(LightDirection));
    UBOLight->setData(2 * sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(LightAmbient));
    UBOLight->setData(3 * sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(LightDiffuse));
    UBOLight->setData(4 * sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(LightSpecular));
}

void Light::ImGuiLight() {
    ImGui::ColorEdit3WithDefault("Light Color", glm::value_ptr(LightColor), LIGHT_COLOR);
    ImGui::SliderFloat3("Light Direction", glm::value_ptr(LightDirection), -1.0f, 1.0f);
    ImGui::SliderFloat3WithDefault("Light Ambient", glm::value_ptr(LightAmbient), 0.0f, 1.0f, LIGHT_AMBIENT);
    ImGui::SliderFloat3WithDefault("Light Diffuse", glm::value_ptr(LightDiffuse), 0.0f, 1.0f, LIGHT_DIFFUSE);
    ImGui::SliderFloat3WithDefault("Light Specular", glm::value_ptr(LightSpecular), 0.0f, 1.0f, LIGHT_SPECULAR);
}
