// From LearnOpenGL repo
// Link: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Terrain.h"
#include "Shader.h"
#include "Def.h"
#include "Player.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,   // UP and DOWN are self-defined
    DOWN
};

// Default camera values
#define YAW 45.0f
#define PITCH 30.0f
#define SPEED 20.0f
#define SENSITIVITY 0.002f
#define ZOOM 45.0f
#define NEAR 0.1f
#define FAR 400.0f
#define CAMERA_POS glm::vec3(35.0f, 120.0f, 35.0f)
#define CAMERA_UP glm::vec3(0.0f, 1.0f, 0.0f)

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float Near, Far;
    // euler Angles
    float Yaw;
    float Pitch;
    // and other attributes
    Terrain* terrain = nullptr; // pointer to terrain, remember to set it before using!

    // constructor with vectors and scalar values
    Camera(glm::vec3 position=CAMERA_POS, glm::vec3 up=CAMERA_UP, float yaw=YAW, float pitch=PITCH, float near=NEAR, float far=FAR);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float near, float far);
    virtual ~Camera() = default;

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    // returns the projection and view matrix calculated using Zoom value, Euler Angles and the LookAt Matrix
    glm::mat4 GetPerspectiveMatrix() const { return glm::perspective(glm::radians(Zoom), SCR_SCALE, Near, Far); }
    glm::mat4 GetPerspectiveMatrix(const float deg, const float aspect) const { return glm::perspective(glm::radians(deg), aspect, Near, Far); }
    glm::mat4 GetOrthoMatrix(float left, float right, float bottom, float top) const { return glm::ortho(left, right, bottom, top, Near, Far); }
    glm::mat4 GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }

    void UpdateThirdPerson(Terrain* terrain, Player *player,
        float distance = 10.0f, float heightOffset=2.0f);
    void ProcessMouseOrbit(float deltaX, float deltaY);
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
    // 球面坐标参数
    float sphericalTheta; // 水平角度
    float sphericalPhi;   // 垂直角度

    // 新增成员变量
    glm::vec3 targetPosition; // 目标相机位置
    float smoothSpeed = 5.0f; // 平滑因子
    float minDistance = 0.1f; // 最小距离阈值

    // 约束 phi 以限制相机在半球体内
    void constrainAngles();
};
