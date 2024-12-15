#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Terrain.h"
#include "BoxGeometry.h"
#include "GameWindow.h"

enum PlayerState {
    IDLE_LAND = 0,
    WALKING_LAND,
    RUNNING_LAND,
    IDLE_WATER,
    SWIMMING_WATER,
    FAST_SWIMMING_WATER,
    IDLE_CLIMB,
    CLIMBING,
    JUMPING,
    LAND_TO_CLIMB,
    CLIMB_TO_LAND
};

class Player {
public:
    Player(glm::vec3 initialPosition, glm::vec3 fixedLength);
    
    void ProcessMoveInput(int direction, bool shift, bool jump, Terrain* terrain, Shader player_shader);
    void Transfer();
private:
    int state;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 upVector;
    glm::vec3 length;
    glm::vec3 color;
    float speed;

    // 速度
    float walkSpeed;
    float runSpeed;
    float swimSpeed;
    float fastSwimSpeed;
    float climbSpeed;
    float jumpHorizenSpeed;
    float jumpUpSpeed;  
    float jumpHeight;
    glm::vec3 jumpDirection;
    float targetJumpHeight;
    bool jumpUp;
    void Render(Shader player_shader);
    void DoJump();
};
