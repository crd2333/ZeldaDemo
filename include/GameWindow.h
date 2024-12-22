//
// Created by mf on 2024/12/5.
//

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Camera.h"
#include "Player.h"
#include "Def.h"
#include "Light.h"

// camera and mouse
extern Camera camera;

// window
extern bool mainMenu;

// timing (per-frame time logic)
extern float deltaTime;

GLFWwindow* Create_glfw_Window();
void RenderLoopPreProcess(GLFWwindow* window, Player* player, Terrain* terrain);
void RenderLoopPostProcess(GLFWwindow* window);

void processInput(GLFWwindow* window, Player* player, Terrain* terrain);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// util functions
void ChangeFocus(GLFWwindow* window, bool flag);
namespace ImGui { // add to ImGui namespace
    void InitImGui(GLFWwindow* window);
    void EndImGui();
    void SliderFloatWithDefault(const char* label, float* v, float v_min, float v_max, float v_default);
    void SliderFloat3WithDefault(const char* label, float* v, float v_min, float v_max, float v_default);
    void ColorEdit3WithDefault(const char* label, float* v, glm::vec3 v_default);
}
