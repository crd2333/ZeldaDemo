// Player.cpp
#include "Player.h"
#include <glm/gtx/string_cast.hpp>

Player::Player(glm::vec3 initialPosition, glm::vec3 fixedLength)
    : position(initialPosition), length(fixedLength), color(1.0f, 0.0f, 0.0f),
      speed(0.0f), state(IDLE_LAND), direction(0.0f, 0.0f, 0.0f), upVector(0.0f, 1.0f, 0.0f),
      walkSpeed(2.0f), runSpeed(4.0f), swimSpeed(1.5f), fastSwimSpeed(3.0f), climbSpeed(1.0f), 
      jumpSpeed(3.5f), jumpHeight(2.0f), currentJumpHeight(0.0f)
{
}

void Player::Render(Shader player_shader) {
  BoxGeometry box(length.x, length.y, length.z);

  // 创建模型矩阵
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  glm::vec3 defaultUp(0.0f, 1.0f, 0.0f);
  glm::vec3 up = glm::normalize(upVector);
  float dot = glm::dot(defaultUp, up);

  if (dot > 0.9999f) {
    // 向量几乎相同，不需要旋转
  } else if (dot < -0.9999f) {
    // 向量相反，旋转 180 度，虽然我也不知道会不会出现，感觉不会（
    glm::vec3 rotationAxis = glm::cross(defaultUp, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glm::length(rotationAxis) < 0.0001f)
      rotationAxis = glm::cross(defaultUp, glm::vec3(0.0f, 0.0f, 1.0f));
    rotationAxis = glm::normalize(rotationAxis);
    model = glm::rotate(model, glm::pi<float>(), rotationAxis);
  } else {
    glm::vec3 rotationAxis = glm::cross(defaultUp, up);
    rotationAxis = glm::normalize(rotationAxis);
    float angle = glm::acos(dot);
    model = glm::rotate(model, angle, rotationAxis);
  }

  player_shader.use();
  player_shader.setMat4("model", model);
  player_shader.setVec3("color", color);
  glBindVertexArray(box.VAO);
  glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}


void Player::ProcessMoveInput(int direction, bool shift, bool jump, Terrain* terrain, 
                            Shader player_shader) {
    // direction 0 w 1 s 2 a 3 d -1 表示没有输入 暂时先考虑平面的移动
    // shift 为 true 时表示按下了 shift 键，即跑步
    // jump 为 true 时表示按下了空格键，即跳跃
    glm::vec3 newPosition = position;
    if (direction != -1) {
        switch (self.state) {
            case IDLE_LAND:
                if (shift) {
                    self.state = RUNNING_LAND;
                    self.speed = self.runSpeed;
                }
                else {
                    self.state = WALKING_LAND;
                    self.speed = self.walkSpeed;
                }
                break;
            case WALKING_LAND:
                if (shift) {
                    self.state = RUNNING_LAND;
                    self.speed = self.runSpeed;
                }
                else 
                    self.speed = self.walkSpeed;
                break;
            case RUNNING_LAND:
                if (!shift) {
                    self.state = WALKING_LAND;
                    self.speed = self.walkSpeed;
                }
                else 
                    self.speed = self.runSpeed;
                break;
            case IDLE_WATER:
                if (shift) {
                    self.state = FAST_SWIMMING_WATER;
                    self.speed = self.fastSwimSpeed;
                }
                else {
                    self.state = SWIMMING_WATER;
                    self.speed = self.swimSpeed;
                }
                break;
            case SWIMMING_WATER:
                if (shift) {
                    self.state = FAST_SWIMMING_WATER;
                    self.speed = self.fastSwimSpeed;
                }
                else 
                    self.speed = self.swimSpeed;
                break;
            case FAST_SWIMMING_WATER:
                if (!shift) {
                    self.state = SWIMMING_WATER;
                    self.speed = self.swimSpeed;
                }
                else 
                    self.speed = self.fastSwimSpeed;
                break;
            case IDLE_CLIMB:
                self.state = CLIMBING;
            case CLIMBING:
                self.speed = self.climbSpeed;
                break;
            case JUMPING:
            case LAND_TO_CLIMB:
            case CLIMB_TO_LAND:
                break;
            default:
                break;
        }
    }
    switch (direction) {
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
            if (self.state == JUMPING) break;
            if (self.state == IDLE_LAND || self.state == WALKING_LAND || self.state == RUNNING_LAND) 
                self.state = IDLE_LAND;
            else if (self.state == IDLE_WATER || self.state == SWIMMING_WATER || self.state == FAST_SWIMMING_WATER) 
                self.state = IDLE_WATER;
            else if (self.state == IDLE_CLIMB || self.state == CLIMBING) 
                self.state = IDLE_CLIMB;
        default:
            break;
    }
    if (self.state != JUMPING && jump && (self.state != IDLE_CLIMB && self.state != CLIMBING && 
        self.state != LAND_TO_CLIMB && self.state != CLIMB_TO_LAND)) {
        self.state = JUMPING;
        self.jumpDirection = self.direction;
        self.jumpUp = true;
        self.targetJumpHeight = self.position.y + self.jumpHeight;
    }
    if (direction == -1 ) return;
    // todo 判断边界
    // todo 加入判断水的逻辑

    glm::vec3 new_normal = terrain->getNormal(newPosition.x, newPosition.z);
    glm::vec3 terrainPosition(newPosition.x, terrain->getHeight(newPosition.x, newPosition.z), newPosition.z);
    newPosition = terrainPosition + new_normal * (self.length.y / 2.0f);
    self.direction = glm::normalize(newPosition - self.position);
    // 处理跳跃的逻辑
    if(self.state == JUMPING) {
        DoJump();
    } else {
        self.position = self.position + self.direction * self.speed * deltaTime;
        new_normal = terrain->getNormal(self.position.x, self.position.z);
        self.upVector = new_normal;
        terrainPosition = glm::vec3(self.position.x, terrain->getHeight(self.position.x, self.position.z), self.position.z);
        self.position = terrainPosition + new_normal * (self.length.y / 2.0f);
        if (self.state == IDLE_LAND || self.state == WALKING_LAND || self.state == RUNNING_LAND) {
            if (glm::acos(new_normal.y) > glm::radians(45.0f)) {
                self.state = LAND_TO_CLIMB;
            }
        }
        else if (self.state == IDLE_CLIMB || self.state == CLIMBING) {
            if (glm::acos(new_normal.y) < glm::radians(45.0f)) {
                self.state = CLIMB_TO_LAND;
            }
        }
    }

    self.Render(player_shader);
    return;
}


void Player::DoJump() {
    self.position = self.position + self.position * self.jumpHorizenSpeed * deltaTime;
    if (self.jumpUp) {
        self.currentJumpHeight += self.jumpUpSpeed * deltaTime;
        if (self.currentJumpHeight >= self.targetJumpHeight) {
            self.jumpUp = false;
            self.currentJumpHeight = self.targetJumpHeight;
        }
    } else {
        self.currentJumpHeight -= self.jumpUpSpeed * deltaTime;
        glm::vec3 new_normal = terrain->getNormal(self.position.x, self.position.z);
        glm::vec3 terrainPosition(self.position.x, terrain->getHeight(self.position.x, self.position.z), self.position.z);
        glm::vec3 ground_position = terrainPosition + new_normal * (self.length.y / 2.0f);
        if (self.currentJumpHeight <= ground_position.y) {
            self.currentJumpHeight = ground_position.y;
            self.state = IDLE_LAND;
        }
    }
    self.position.y = self.currentJumpHeight;
    self.upVector = terrain->getNormal(self.position.x, self.position.z);
    return;
}