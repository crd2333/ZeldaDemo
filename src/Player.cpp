// Player.cpp
#include "Player.h"
#include "Terrain.h"
#include "geometry/BoxGeometry.h"
#include "shader.h"
#include "GameWindow.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Player::Player(glm::vec3 initialPosition, glm::vec3 fixedLength, Terrain* terrain)
    : state(IDLE_LAND), position(initialPosition), direction(1.0f, 0.0f, 1.0f), 
    upVector(0.0f, 1.0f, 0.0f), length(fixedLength), color(0.55f, 0.27f, 0.07f), 
    speed(0.0f), walkSpeed(2.0f), runSpeed(4.0f), swimSpeed(1.5f), fastSwimSpeed(3.0f), 
    climbSpeed(1.0f), jumpHorizenSpeed(3.5f), jumpUpSpeed(2.5f), jumpHeight(5.0f), 
    jumpDirection(0.0f, 0.0f, 1.0f), targetJumpHeight(0.0f), jumpUp(true),
    boxGeometry(fixedLength.x, fixedLength.y, fixedLength.z)
{
    Update(terrain);
    std::cout << "Player created at " << glm::to_string(position) << std::endl;
    vertices = boxGeometry.vertices;
    indices = boxGeometry.indices;
    InitializeBuffers();
}


Player::~Player() {
    // 删除 OpenGL 缓冲区
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Player::InitializeBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); 
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); 
}


void Player::Update(Terrain* terrain) {
    // 四个角
    float w = length.x / 2.0f;
    float h = length.y / 2.0f;
    float d = length.z / 2.0f;
    std::vector<glm::vec3> localCorners = {
        glm::vec3(-w, -h, -d),
        glm::vec3(w, -h, -d),
        glm::vec3(w, -h, d),
        glm::vec3(-w, -h, d)
    };

    // 计算旋转矩阵，使得本地 Y 轴对齐到 upVector
    glm::vec3 defaultUp(0.0f, 1.0f, 0.0f);
    glm::vec3 up = glm::normalize(upVector);
    float dot = glm::dot(defaultUp, up);
    glm::mat4 rotationMatrix = glm::mat4(1.0f);

    if (dot > 0.9999f) {
        // 向量几乎相同，不需要旋转
    } else if (dot < -0.9999f) {
        // 向量相反，旋转 180 度
        glm::vec3 rotationAxis = glm::cross(defaultUp, glm::vec3(1.0f, 0.0f, 0.0f));
        if (glm::length(rotationAxis) < 0.0001f)
            rotationAxis = glm::cross(defaultUp, glm::vec3(0.0f, 0.0f, 1.0f));
        rotationAxis = glm::normalize(rotationAxis);
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), rotationAxis);
    } else {
        // 计算旋转轴和角度
        glm::vec3 rotationAxis = glm::cross(defaultUp, up);
        rotationAxis = glm::normalize(rotationAxis);
        float angle = glm::acos(dot);
        rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);
    }

    // 将本地角点旋转并转换到世界坐标系
    std::vector<glm::vec3> worldCorners;
    for(auto& corner : localCorners){
        glm::vec4 rotated = rotationMatrix * glm::vec4(corner, 1.0f);
        glm::vec3 worldCorner = glm::vec3(rotated) + position;
        worldCorners.push_back(worldCorner);
    }

    // 每个角点的地形高度和法向量
    float maxTerrainHeight = -std::numeric_limits<float>::infinity();
    std::vector<glm::vec3> sampledNormals;
    for(auto& corner : worldCorners){
        float terrainHeight = terrain->getHeight(corner.x, corner.z);
        maxTerrainHeight = glm::max(maxTerrainHeight, terrainHeight);
        glm::vec3 terrainNormal = terrain->getNormal(corner.x, corner.z);
        sampledNormals.push_back(glm::normalize(terrainNormal));
    }

    // 计算采样点的平均法向量，更新 upVector
    glm::vec3 averageNormal(0.0f);
    for(auto& n : sampledNormals){
        averageNormal += n;
    }
    averageNormal /= sampledNormals.size();
    averageNormal = glm::normalize(averageNormal);
    upVector = averageNormal;

    // 计算新的旋转矩阵，使得新的 upVector 对齐
    dot = glm::dot(defaultUp, upVector);
    rotationMatrix = glm::mat4(1.0f);
    if (dot > 0.9999f) {
        // 向量几乎相同，不需要旋转
    } else if (dot < -0.9999f) {
        // 向量相反，旋转 180 度
        glm::vec3 rotationAxis = glm::cross(defaultUp, glm::vec3(1.0f, 0.0f, 0.0f));
        if (glm::length(rotationAxis) < 0.0001f)
            rotationAxis = glm::cross(defaultUp, glm::vec3(0.0f, 0.0f, 1.0f));
        rotationAxis = glm::normalize(rotationAxis);
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), rotationAxis);
    } else {
        // 计算旋转轴和角度
        glm::vec3 rotationAxis = glm::cross(defaultUp, upVector);
        rotationAxis = glm::normalize(rotationAxis);
        float angle = glm::acos(dot);
        rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);
    }

    // 将本地角点旋转并转换到世界坐标系
    worldCorners.clear();
    for(auto& corner : localCorners){
        glm::vec4 rotated = rotationMatrix * glm::vec4(corner, 1.0f);
        glm::vec3 worldCorner = glm::vec3(rotated) + position;
        worldCorners.push_back(worldCorner);
    }

    // 采样移动后的位置的地形高度，确保长方体底部不低于地形
    float finalTerrainHeight = -std::numeric_limits<float>::infinity();
    for(auto& corner : worldCorners){
        float terrainHeight = terrain->getHeight(corner.x, corner.z);
        finalTerrainHeight = glm::max(finalTerrainHeight, terrainHeight);
    }

    // 更新 y，使得长方体底部不低于最高的地形高度
    position.y = finalTerrainHeight + (length.y / 2.0f) * upVector.y;

}


void Player::Render(Shader & player_shader, const glm::mat4 &projection, const glm::mat4 &view) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    glm::vec3 defaultUp(0.0f, 1.0f, 0.0f);
    glm::vec3 up = glm::normalize(upVector);

    float dot = glm::dot(defaultUp, up);
    if (dot > 0.9999f) {
        // 向量几乎相同，不需要旋转
    } else if (dot < -0.9999f) {
        // 向量相反，旋转 180 度，应该用不到吧
        glm::vec3 rotationAxis = glm::cross(defaultUp, glm::vec3(1.0f, 0.0f, 0.0f));
        if (glm::length(rotationAxis) < 0.0001f)
            rotationAxis = glm::cross(defaultUp, glm::vec3(0.0f, 0.0f, 1.0f));
        rotationAxis = glm::normalize(rotationAxis);
        model = glm::rotate(model, glm::pi<float>(), rotationAxis);
    } else {
        // 计算旋转轴和角度
        glm::vec3 rotationAxis = glm::cross(defaultUp, up);
        rotationAxis = glm::normalize(rotationAxis);
        float angle = glm::acos(dot);
        model = glm::rotate(model, angle, rotationAxis);
    }

    player_shader.use();
    player_shader.setMat4("model", model);
    player_shader.setMat4("projection", projection);
    player_shader.setMat4("view", view); 
    player_shader.setVec3("objectColor", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); 
}


void Player::ProcessMoveInput(int moveDirection, bool shift, bool jump, Terrain* terrain, 
                             float deltaTime) {
    // moveDirection 0 w 1 s 2 a 3 d -1 表示没有输入 暂时先考虑平面的移动
    // shift 为 true 时表示按下了 shift 键，即跑步
    // jump 为 true 时表示按下了空格键，即跳跃

    glm::vec3 newPosition = position;
    if (moveDirection != -1) {
        switch (state) {
            case IDLE_LAND:
                if (shift) {
                    state = RUNNING_LAND;
                    speed = runSpeed;
                }
                else {
                    state = WALKING_LAND;
                    speed = walkSpeed;
                }
                break;
            case WALKING_LAND:
                if (shift) {
                    state = RUNNING_LAND;
                    speed = runSpeed;
                }
                else 
                    speed = walkSpeed;
                break;
            case RUNNING_LAND:
                if (!shift) {
                    state = WALKING_LAND;
                    speed = walkSpeed;
                }
                else 
                    speed = runSpeed;
                break;
            case IDLE_WATER:
                if (shift) {
                    state = FAST_SWIMMING_WATER;
                    speed = fastSwimSpeed;
                }
                else {
                    state = SWIMMING_WATER;
                    speed = swimSpeed;
                }
                break;
            case SWIMMING_WATER:
                if (shift) {
                    state = FAST_SWIMMING_WATER;
                    speed = fastSwimSpeed;
                }
                else 
                    speed = swimSpeed;
                break;
            case FAST_SWIMMING_WATER:
                if (!shift) {
                    state = SWIMMING_WATER;
                    speed = swimSpeed;
                }
                else 
                    speed = fastSwimSpeed;
                break;
            case IDLE_CLIMB:
                state = CLIMBING;
            case CLIMBING:
                speed = climbSpeed;
                break;
            case JUMPING:
            case LAND_TO_CLIMB:
            case CLIMB_TO_LAND:
                break;
            default:
                break;
        }
    }
    switch (moveDirection) {
        case 0:
            newPosition += glm::vec3(0.0f, 0.0f, -speed * deltaTime);
            break;
        case 1:
            newPosition += glm::vec3(0.0f, 0.0f, speed * deltaTime);
            break;
        case 2:
            newPosition += glm::vec3(-speed * deltaTime, 0.0f, 0.0f);
            break;
        case 3:
            newPosition += glm::vec3(speed * deltaTime, 0.0f, 0.0f);
            break;
        case -1: // 切换为静止
            if (state == JUMPING) break;
            if (state == IDLE_LAND || state == WALKING_LAND || state == RUNNING_LAND) 
                state = IDLE_LAND;
            else if (state == IDLE_WATER || state == SWIMMING_WATER || state == FAST_SWIMMING_WATER) 
                state = IDLE_WATER;
            else if (state == IDLE_CLIMB || state == CLIMBING) 
                state = IDLE_CLIMB;
        default:
            break;
    }
    if (state != JUMPING && jump && (state != IDLE_CLIMB && state != CLIMBING && 
        state != LAND_TO_CLIMB && state != CLIMB_TO_LAND)) {
        state = JUMPING;
        jumpDirection = direction;
        jumpUp = true;
        targetJumpHeight = position.y + jumpHeight;
    }
    if (moveDirection == -1 && state != JUMPING) return;
    // todo 判断边界
    // todo 加入判断水的逻辑
    // todo climb的逻辑

    glm::vec3 new_normal = terrain->getNormal(newPosition.x, newPosition.z);
    glm::vec3 terrainPosition(newPosition.x, terrain->getHeight(newPosition.x, newPosition.z), newPosition.z);
    newPosition = terrainPosition + new_normal * (length.y / 2.0f);
    direction = glm::normalize(newPosition - position);
    // 处理跳跃的逻辑
    if(state == JUMPING) {
        DoJump(terrain, deltaTime);
    } else {
        position = position + direction * speed * deltaTime;
        Update(terrain);
        if (state == IDLE_LAND || state == WALKING_LAND || state == RUNNING_LAND) {
            if (glm::acos(new_normal.y) > glm::radians(45.0f)) {
                state = LAND_TO_CLIMB;
            }
        }
        else if (state == IDLE_CLIMB || state == CLIMBING) {
            if (glm::acos(new_normal.y) < glm::radians(45.0f)) {
                state = CLIMB_TO_LAND;
            }
        }
    }
    return;
}


void Player::DoJump(Terrain* terrain, float deltaTime) {
    float currentJumpHeight = position.y;
    position += direction * jumpHorizenSpeed * deltaTime;
    if (jumpUp) {
        currentJumpHeight += jumpUpSpeed * deltaTime;
        if (currentJumpHeight >= targetJumpHeight) {
            jumpUp = false;
            currentJumpHeight = targetJumpHeight;
        }
    } else {
        currentJumpHeight -= jumpUpSpeed * deltaTime;
        glm::vec3 new_normal = terrain->getNormal(position.x, position.z);
        glm::vec3 terrainPosition(position.x, terrain->getHeight(position.x, position.z), position.z);
        glm::vec3 ground_position = terrainPosition + new_normal * (length.y / 2.0f);
        if (currentJumpHeight <= ground_position.y) {
            currentJumpHeight = ground_position.y;
            state = IDLE_LAND;
        }
    }
    position.y = currentJumpHeight;
    upVector = terrain->getNormal(position.x, position.z);
    return;
}