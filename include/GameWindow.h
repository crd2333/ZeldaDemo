//
// Created by mf on 2024/12/5.
//

#pragma once

#include "Def.h"
#include "Camera.h"
#include "Player.h"
#include "Light.h"
#include "Objects.h"

// camera and mouse
extern Camera camera;

// window
extern bool mainMenu;

// timing (per-frame time logic)
extern float deltaTime;

GLFWwindow* Create_glfw_Window();
void RenderLoopPreProcess(GLFWwindow* window, Player* player, Terrain* terrain,
                          Bomb* playerBomb, BroadLeaf* broadLeaf, WhiteBirch* whiteBirch, TreeApple* treeApple,
                          WoodBox* woodBoxs, int numWoodbox, MetalBox_breakable* metalBox_breakables , int numMetalBox,
                          MetalBox_B * metalBox_Bs, int numMetalBox_B, MetalBox_C * metalBox_Cs, int numMetalBox_C,
                          int numBroadLeaf,  int numWhiteBirch, int numTreeApple);
void RenderLoopPostProcess(GLFWwindow* window);

void processInput(GLFWwindow* window, Player* player, Terrain* terrain, Bomb* playerBomb,
                  BroadLeaf* broadLeaf, WhiteBirch* whiteBirch, TreeApple* treeApple,
                  WoodBox* woodBoxs, int numWoodbox, MetalBox_breakable* metalBox_breakables , int numMetalBox,
                  MetalBox_B * metalBox_Bs, int numMetalBox_B, MetalBox_C * metalBox_Cs, int numMetalBox_C,
                  int numBroadLeaf,  int numWhiteBirch, int numTreeApple);
void processBreak(GLFWwindow* window,Player* player, Bomb* bomb, Terrain* terrain,  WoodBox* woodBoxs,
                  int numWoodbox, MetalBox_breakable* metalBox_breakables , int numMetalBox,
                  BroadLeaf* boradLeaf, int numBroadLeaf, WhiteBirch* whiteBirch, int numWhiteBirch,
                  TreeApple* treeApple,int numTreeApple);
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
