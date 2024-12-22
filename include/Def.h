//
// Created by mf on 2024/12/5.
//
// 存储全局宏定义

#pragma once
#include <glm/glm.hpp>
#include <iostream>

// window config
#define SCR_WIDTH 1500
#define SCR_HEIGHT 1000
#define SCR_SCALE ((float)SCR_WIDTH / (float)SCR_HEIGHT)

// terrain config
#define MAP_SZIE glm::vec2(250.0f)
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

#define Threshold 0.01f

class Camera;
class Player;
class Terrain;