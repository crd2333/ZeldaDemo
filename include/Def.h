//
// Created by mf on 2024/12/5.
//
// 存储全局宏定义与类的前置声明
// 如果因为交叉引用而逐渐变得屎山，可以考虑声明到这里

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <map>

// window config
#define SCR_WIDTH 1500
#define SCR_HEIGHT 1000
#define SCR_SCALE ((float)SCR_WIDTH / (float)SCR_HEIGHT)
#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096
#define SHADOW_SCALE ((float)SHADOW_WIDTH / (float)SHADOW_HEIGHT)

// terrain config
#define MAP_SZIE glm::vec2(400.0f)
#define HEIGHT_SCALE 300.0f
#define RESOLUTION_RATIO 2
#define SAMPLE_NUM 3

#define Err(message) do {                                                           \
    std::cout << "\33[1;31m[" << __FILE__ << ":" << __LINE__ << ", in function " << \
    __FUNCTION__ << "] " << message << "\33[0m\n";                                  \
    exit(-1);                                                                       \
} while (0)

#define Warn(message) do {                                                          \
    std::cout << "\33[1;33m[" << __FILE__ << ":" << __LINE__ << ", in function " << \
    __FUNCTION__ << "] " << message << "\33[0m\n";                                  \
} while (0)

#define Log(message) do {                                                           \
    std::cout << "\33[1;32m[" << __FILE__ << ":" << __LINE__ << ", in function " << \
    __FUNCTION__ << "] " << message << "\33[0m\n";                                  \
} while (0)

#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

#define Threshold 0.001f

class Camera;
class Player;
class Terrain;