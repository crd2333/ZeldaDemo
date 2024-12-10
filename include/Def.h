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
#define SCALE ((float)SCR_HEIGHT / SCR_WIDTH)

// camera config
#define CAMERA_POS glm::vec3(-25.0f, 150.0f, -5.0f)
#define CAMERA_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_YAW 45.0f
#define CAMERA_PITCH -45.0f

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
