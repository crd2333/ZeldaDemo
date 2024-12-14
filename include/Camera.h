// From LearnOpenGL repo
// Link: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Terrain.h"
#include "Shader.h"
#include "Def.h"

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
#define PITCH 0.0f
#define SPEED 20.0f
#define SENSITIVITY 0.1f
#define ZOOM 45.0f
#define NEAR 0.1f
#define FAR 400.0f
#define CAMERA_POS glm::vec3(-25.0f, 150.0f, -5.0f)
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
    UBO* uboProjView = nullptr; // UBO for camera, remember to set it before using!

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

    // UBO for camera (projection and view matrix shared by binded shaders)
    void BindUBO(const Shader& shader) const;
    void SetUBO() const;
    void SetUBO(glm::mat4* projection, glm::mat4* view) const; // if manually set projection and view matrix (like use ortho matrix)

    // returns the projection and view matrix calculated using Zoom value, Euler Angles and the LookAt Matrix
    glm::mat4 GetPerspectiveMatrix() const { return glm::perspective(glm::radians(Zoom), SCR_SCALE, Near, Far); }
    glm::mat4 GetOrthoMatrix(float left, float right, float bottom, float top) const { return glm::ortho(left, right, bottom, top, Near, Far); }
    glm::mat4 GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};
