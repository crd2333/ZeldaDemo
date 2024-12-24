#pragma once

#include "Def.h"
#include "Terrain.h"
#include "geometry/BoxGeometry.h"
#include "geometry/PlaneGeometry.h"
#include "geometry/SphereGeometry.h"
#include "geometry/SwordQiGeometry.h"
#include "GameWindow.h"
#include "Shader.h"
#include "Camera.h"
#include "Water.h"
#include "Objects.h"
#include <iostream>

enum PlayerState {
    IDLE_LAND = 0,
    WALKING_LAND,
    RUNNING_LAND,
    IDLE_WATER,
    SWIMMING_WATER,
    FAST_SWIMMING_WATER,
    CLIMBING,
    JUMPING
};

enum moveDirection {
    MOVE_FORWARD = 0,
    MOVE_BACKWARD = 1,
    MOVE_LEFT = 2,
    MOVE_RIGHT = 3,
    MOVE_STATIC = -1
};

class Player {
private:
    int state;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 upVector;
    glm::vec3 length;
    glm::vec3 swimLength;
    glm::vec3 color;
    glm::vec3 landColor;
    glm::vec3 swimColor;
    glm::vec3 flyColor;
    float alpha=1.0f;
    float speed;

    // 速度
    float walkSpeed;
    float runSpeed;
    float swimSpeed;
    float fastSwimSpeed;
    float climbSpeed;
    float jumpHorizenSpeed;
    float jumpUpSpeed;
    // 跳跃相关参数
    float jumpHeight;
    glm::vec3 jumpDirection;
    float targetJumpHeight;
    bool jumpUp;
    bool isflying = false;
    // 游泳相关参数
    bool swimFlag; // 初次进水由0变1，初次出水由1变0
    float swimtheta_delta;
    // 攀爬相关参数
    glm::vec3 climbcolor;
    float climbtheta;
    float climbtheta_delta;
    glm::vec3 climbRotateAxis;
    // 盾牌相关参数
    float shieldFactor = 1.0f;
    // 武器相关参数
    float weaponFactor = 1.0f;
    bool isAttacking = false;
    // 动画计数器1s1拍
    int actionCount = 0;
    float actionCount_unused = 0;



public:
    Player(glm::vec3 initialPosition, glm::vec3 fixedLength, Terrain* terrain);
    ~Player();
    void draw(Shader& shader);
    void ProcessMoveInput(moveDirection move_Direction, bool shift, bool jump, bool fly, bool bomb_state,
         bool reset, bool mouseLeft,bool mouseRight, Terrain* terrain, Bomb* playerBomb, float deltaTime,
         BroadLeaf* broadLeaf, WhiteBirch* whiteBirch, TreeApple* treeApple,
         WoodBox* woodBoxs, int numWoodbox, MetalBox_breakable* metalBox_breakables , int numMetalBox,
                MetalBox_B * metalBox_Bs, int numMetalBox_B, MetalBox_C * metalBox_Cs, int numMetalBox_C);
    // void Transfer();
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDirection() const { return direction; }
    void setDirection(const glm::vec3& newDirection) { direction = newDirection; }
    glm::vec3 getUpVector() const { return upVector; }
    void setUpVector(const glm::vec3& newUpVector) { upVector = newUpVector; }
    glm::vec3 getLength() const { return length; }
    void setLength(const glm::vec3& newLength) { length = newLength; }
    glm::vec3 getColor() const { return color; }
    void setColor(const glm::vec3& newColor) { color = newColor; }
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getWalkSpeed() const { return walkSpeed; }
    void setWalkSpeed(float newWalkSpeed) { walkSpeed = newWalkSpeed; }
    float getRunSpeed() const { return runSpeed; }
    void setRunSpeed(float newRunSpeed) { runSpeed = newRunSpeed; }
    float getSwimSpeed() const { return swimSpeed; }
    void setSwimSpeed(float newSwimSpeed) { swimSpeed = newSwimSpeed; }
    float getFastSwimSpeed() const { return fastSwimSpeed; }
    void setFastSwimSpeed(float newFastSwimSpeed) { fastSwimSpeed = newFastSwimSpeed; }
    float getClimbSpeed() const { return climbSpeed; }
    void setClimbSpeed(float newClimbSpeed) { climbSpeed = newClimbSpeed; }
    float getJumpHorizenSpeed() const { return jumpHorizenSpeed; }
    void setJumpHorizenSpeed(float newJumpHorizenSpeed) { jumpHorizenSpeed = newJumpHorizenSpeed; }
    float getJumpUpSpeed() const { return jumpUpSpeed; }
    void setJumpUpSpeed(float newJumpUpSpeed) { jumpUpSpeed = newJumpUpSpeed; }
    float getJumpHeight() const { return jumpHeight; }
    void setJumpHeight(float newJumpHeight) { jumpHeight = newJumpHeight; }
    void setAlpha(float newAlpha) { alpha = newAlpha; }
    bool isAttack() { return isAttacking; }
private:
    void DoJump(Terrain* terrain, float deltaTime, bool fly = false);
    void Update(Terrain* terrain);
    void Rebind();
    unsigned int VAO, VBO, EBO;
    unsigned int lineVAO, lineVBO, lineEBO;
    unsigned int VAO_weapon1, VBO_weapon1, EBO_weapon1;
    unsigned int VAO_weapon2, VBO_weapon2, EBO_weapon2;
    std::vector<Vertex> vertices, vertices_weapon1, vertices_weapon2;
    std::vector<unsigned int> indices, indices_weapon1, indices_weapon2;
    BoxGeometry boxGeometry;
    PlaneGeometry playerShield;
    SwordQiGeometry playerWeapon;
    void InitializeBuffers();
    void DrawLine(Shader& line_shader);
};
