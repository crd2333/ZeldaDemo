// Player.cpp
#include "Player.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Player::Player(glm::vec3 initialPosition, glm::vec3 fixedLength, Terrain* terrain)
    : state(IDLE_LAND), position(initialPosition), direction(1.0f, 0.0f, 0.0f), upVector(0.0f, 1.0f, 0.0f),
    length(fixedLength), landColor(0.55f, 0.27f, 0.07f), swimColor(1.0f, 0.7f, 0.4f),
    speed(0.0f), walkSpeed(8.0f), runSpeed(16.0f), swimSpeed(6.0f), fastSwimSpeed(12.0f),
    climbSpeed(20.0f),
    // 游泳参数
    swimtheta_delta(0.0f),swimFlag(false),
    // 跳跃参数
    jumpHorizenSpeed(14.0f), jumpUpSpeed(15.0f), jumpHeight(5.0f),
    jumpDirection(0.0f, 0.0f, 1.0f), targetJumpHeight(0.0f), jumpUp(true),
    // 攀爬参数
    climbtheta(-20.0f), climbRotateAxis(0.0f, 0.0f, 1.0f),climbcolor(1.0f,0.0f,0.0f),
    // 滑翔参数
    flyColor(0.05f,0.45f,0.25f),
    // 人物相关模型参数
    playerShield(3.0f, 4.0f),
    playerWeapon(0.3f, 4.0f),
    boxGeometry(fixedLength.x, fixedLength.y, fixedLength.z)
{
    color = landColor;
    swimLength = glm::vec3(fixedLength.y, fixedLength.x, fixedLength.z);
    Update(terrain);
    std::cout << "Player created at " << glm::to_string(position) << std::endl;
    vertices = boxGeometry.vertices;
    indices = boxGeometry.indices;
    vertices_weapon1 = playerWeapon.vertices;
    indices_weapon1 = playerWeapon.indices;
    vertices_weapon2 = playerShield.vertices;
    indices_weapon2 = playerShield.indices;

    InitializeBuffers();
}


Player::~Player() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO_weapon2);
    glDeleteBuffers(1, &VBO_weapon2);
    glDeleteBuffers(1, &EBO_weapon2);
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

    // 武器
    glGenVertexArrays(1, &VAO_weapon1);
    glGenBuffers(1, &VBO_weapon1);
    glGenBuffers(1, &EBO_weapon1);

    glBindVertexArray(VAO_weapon1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_weapon1);
    glBufferData(GL_ARRAY_BUFFER, vertices_weapon1.size() * sizeof(Vertex), &vertices_weapon1[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_weapon1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_weapon1.size() * sizeof(unsigned int), &indices_weapon1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);

    // 盾牌
    glGenVertexArrays(1, &VAO_weapon2);
    glGenBuffers(1, &VBO_weapon2);
    glGenBuffers(1, &EBO_weapon2);

    glBindVertexArray(VAO_weapon2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_weapon2);
    glBufferData(GL_ARRAY_BUFFER, vertices_weapon2.size() * sizeof(Vertex), &vertices_weapon2[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_weapon2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_weapon2.size() * sizeof(unsigned int), &indices_weapon2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(3);
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

    // 储存旧的位置，上向量
    glm::vec3 oldPosition = position;
    glm::vec3 oldUpVector = upVector;

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
    if(glm::dot(averageNormal, defaultUp) < 0.8f && state != CLIMBING){
        state = CLIMBING;
        climbtheta = abs( glm::acos(glm::dot(averageNormal, upVector))/ PI * 180.0f);
        climbtheta = max(5.0f, climbtheta);
        upVector = defaultUp;
        climbRotateAxis = - glm::normalize(glm::cross(averageNormal, upVector));
    }
    if(glm::dot(averageNormal, defaultUp) > 0.8f){
        if(state == CLIMBING ){
            state = IDLE_LAND;
        }
        climbtheta_delta = 0.0f;
        climbtheta = 0.0f;
    }
    if(state != CLIMBING && state != JUMPING){
        upVector = averageNormal;
    }

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

    // 判断新旧位置是否过于接近，过接近则不更新
    if(glm::distance(position, oldPosition) < Threshold){
        position = oldPosition;
        upVector = oldUpVector;
    }

}


void Player::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    glm::vec3 defaultUp(0.0f, 1.0f, 0.0f);
    glm::vec3 defaultFront(1.0f, 0.0f, 0.0f);
    glm::vec3 front(direction.x,0.0f,direction.z);
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
        glm::vec3 rotationAxis =  glm::normalize(glm::cross(defaultUp, up));
        float angle = glm::acos(dot);
        model = glm::rotate(model, angle, rotationAxis);
    }

    dot = glm::dot(defaultFront, front);
    if (dot > 0.9999f) {
        // 向量几乎相同，不需要旋转
    } else if (dot < -0.9999f) {
        model = glm::rotate(model, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    } else {
        // 计算旋转轴和角度
        glm::vec3 rotationAxis =  glm::normalize(glm::cross(defaultFront, front));
        float angle = glm::acos(dot);
        model = state == CLIMBING ? model:glm::rotate(model, angle, rotationAxis);
    }

    //攀爬时的旋转
    if(state == CLIMBING){
        model = glm::rotate(model, -glm::radians(climbtheta_delta), climbRotateAxis);
    }
    //游泳时的旋转
    if(swimFlag){
        model = glm::rotate(model, glm::radians(swimtheta_delta), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    shader.use();
    shader.setFloat("alpha", alpha);

    // 绘制盾牌
    glm::mat4 modelWeapon_2 = glm::rotate(model, PI + PI/2 ,glm::vec3(0.0f,1.0f,0.0f));
    modelWeapon_2 = glm::translate(modelWeapon_2, glm::vec3(length.x/2, 0.0f, 0.0f));
    modelWeapon_2 = glm::translate(modelWeapon_2, glm::vec3(0.0f, 0.0f, -3.0f));
    modelWeapon_2 = glm::scale(modelWeapon_2, glm::vec3(shieldFactor, shieldFactor, shieldFactor));
    shader.setMat4("model", modelWeapon_2);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(modelWeapon_2))); // 在外部计算好 normal matrix（避免 GPU 频繁求逆）
    shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.99f));//这里的颜色特殊，参数是着色器特殊判断符，修改颜色到对应类里面修改
    glBindVertexArray(VAO_weapon2);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_weapon2.size()), GL_UNSIGNED_INT, 0);

    // 绘制武器
    glm::mat4 modelWeapon_1 = glm::rotate(model, PI + PI/2 ,glm::vec3(0.0f,1.0f,0.0f));
    modelWeapon_1 = glm::translate(modelWeapon_1, glm::vec3(length.x/3, 0.0f, 0.0f));
    modelWeapon_1 = glm::translate(modelWeapon_1, glm::vec3(0.0f, 0.0f, -3.0*weaponFactor));
    modelWeapon_1 = glm::scale(modelWeapon_1, glm::vec3(weaponFactor, weaponFactor, weaponFactor));
    shader.setMat4("model", modelWeapon_1);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(modelWeapon_1))); // 在外部计算好 normal matrix（避免 GPU 频繁求逆）
    shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.99f));//这里的颜色特殊，参数是着色器特殊判断符，修改颜色到对应类里面修改
    glBindVertexArray(VAO_weapon1);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_weapon1.size()), GL_UNSIGNED_INT, 0);

    // 绘制人物
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model))); // 在外部计算好 normal matrix（避免 GPU 频繁求逆）
    shader.setVec3("objectColor", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);


    glBindVertexArray(0);
}


void Player::ProcessMoveInput(moveDirection move_Direction, bool shift, bool jump, bool fly, bool bomb_state, bool reset,
                bool mouseLeft, bool mouseRight,Terrain* terrain, Bomb* playerBomb, float deltaTime,
                BroadLeaf* broadLeaf, WhiteBirch* whiteBirch, TreeApple* treeApple,
                WoodBox* woodBoxs, int numWoodbox, MetalBox_breakable* metalBox_breakables , int numMetalBox,
                MetalBox_B * metalBox_Bs, int numMetalBox_B, MetalBox_C * metalBox_Cs, int numMetalBox_C) {
    // shift 为 true 时表示按下了 shift 键，即跑步
    // jump 为 true 时表示按下了空格键，即跳跃
    int countBroadLeaf = 12, countWhiteBirch = 53, countTreeApple = 11;
    if (reset) position = glm::vec3(-70.0f, 0.0f, -5.0f);
    actionCount_unused += deltaTime;
    if(actionCount_unused > 1){
        actionCount_unused = 0;
        actionCount ++;
    }
    glm::vec3 newPosition = position;
    glm::vec3 towardDirection;
    if (move_Direction != moveDirection::MOVE_STATIC) {
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
            case CLIMBING:
                speed = climbSpeed;
            break;
            default:
                break;
        }
    }
    // 处理未跳跃时的位移变化
    if (state != JUMPING) {
        glm::vec3 moveDirection = direction;
        glm::vec3 forward = moveDirection;
        glm::vec3 right = glm::normalize(glm::cross(forward, upVector));
        switch (move_Direction) {
            case moveDirection::MOVE_FORWARD:
                newPosition += forward * speed * deltaTime;
                towardDirection = forward;
                break;
            case moveDirection::MOVE_BACKWARD:
                newPosition -= forward * speed * deltaTime;
                towardDirection = -forward;
                break;
            case moveDirection::MOVE_LEFT:
                newPosition -= right * speed * deltaTime;
                towardDirection = -right;
                break;
            case moveDirection::MOVE_RIGHT:
                newPosition += right * speed * deltaTime;
                towardDirection = right;
                break;
            case moveDirection::MOVE_STATIC: // 切换为静止
                if (state == IDLE_LAND || state == WALKING_LAND || state == RUNNING_LAND)
                    state = IDLE_LAND;
                else if (state == IDLE_WATER || state == SWIMMING_WATER || state == FAST_SWIMMING_WATER)
                    state = IDLE_WATER;
                break;
            default:
                break;
        }
        // if (glm::distance(newPosition,position) > 0.001f && !swimFlag) {
        //     newPosition.y = terrain->getHeight(newPosition.x, newPosition.z) + length.y / 2.0f;
        //     newPosition = position + glm::normalize(newPosition - position) * speed * deltaTime;
        // }

    } else {
        towardDirection = jumpDirection;
    }

    // 处理跳跃时的状态+参数设置
    if (state != JUMPING && jump) {
        state = JUMPING;
        upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        jumpDirection = direction;
        switch (move_Direction)
        {
        case moveDirection::MOVE_FORWARD:
            jumpDirection = glm::normalize(jumpDirection);
            break;
        case moveDirection::MOVE_BACKWARD:
            jumpDirection = -glm::normalize(jumpDirection);
            break;
        case moveDirection::MOVE_LEFT:
            jumpDirection = -glm::normalize(glm::cross(jumpDirection, upVector));
            break;
        case moveDirection::MOVE_RIGHT:
            jumpDirection = glm::normalize(glm::cross(jumpDirection, upVector));
            break;
        case moveDirection::MOVE_STATIC:
            jumpDirection = glm::vec3(0.0f, 0.0f, 0.0f);
        default:
            break;
        }
        jumpUp = true;
        targetJumpHeight = position.y + jumpHeight;
    }

    // 判断水的逻辑
    float waterHeight = checkHeight(newPosition.x, newPosition.z);
    if ( abs(waterHeight + 1.0f) > 0.001f && waterHeight > newPosition.y - 0.5f) {
        if (state == IDLE_LAND) state = IDLE_WATER;
        else if (state == WALKING_LAND) state = SWIMMING_WATER;
        else if (state == RUNNING_LAND) state = FAST_SWIMMING_WATER;
        if (state == IDLE_WATER || state == SWIMMING_WATER || state == FAST_SWIMMING_WATER) {
            newPosition.y = waterHeight + 0.49f;
            upVector = glm::vec3(0.0f, 1.0f, 0.0f);
            position = newPosition;
        }
        if(actionCount % 1 == 0 && swimtheta_delta <= 90.0f){
            swimtheta_delta = swimtheta_delta >= 90.0f ? 90.0f : swimtheta_delta + 5;
            actionCount ++;
        }
        color = swimColor;
        if (!swimFlag) {
            swimFlag = true; //初次入水
            std::cout << "First time in water" << std::endl;
            return;
        }
    }
    else if (newPosition.y > waterHeight + 0.5f) {
        if (state == IDLE_WATER) state = IDLE_LAND;
        else if (state == SWIMMING_WATER) state = IDLE_LAND;
        else if (state == FAST_SWIMMING_WATER) state = IDLE_LAND;
        if (swimFlag) {
            swimFlag = false; //初次出水
            swimtheta_delta = 0.0f;
            std::cout << "First time out of water" << std::endl;
            color = landColor;
            return;
        }
    }
    // 处理攀爬的逻辑
    if (state == CLIMBING && actionCount % 1 == 0) {
        climbtheta_delta = climbtheta_delta >= climbtheta ? climbtheta : climbtheta_delta + 2;
        if(climbtheta_delta >= climbtheta){
            color = climbcolor;
        }
        actionCount ++;
    }else if(state == IDLE_LAND || state == WALKING_LAND || state == RUNNING_LAND){
        color = landColor;
    }

    // 处理跳跃的逻辑
    if(state == JUMPING) {
        DoJump(terrain, deltaTime,fly);
    } else if (!swimFlag ) {
        position = newPosition;
        Update(terrain);
    }

    // 树木不能穿模
    float x1 = position.x;
    float z1 = position.z;
    float x2, z2, distance;
    glm::vec3 temPosition;
    for (int i=0;i<countBroadLeaf;i++) {
        x2 = broadLeaf[i].position.x;
        z2 = broadLeaf[i].position.z;
        distance = sqrt((x1-x2)*(x1-x2) + (z1-z2)*(z1-z2));
        if (distance < 5.0f) {
            temPosition = broadLeaf[i].position - towardDirection * 5.5f;
            position.x = temPosition.x;
            position.z = temPosition.z;
        }
    }
    for (int i=0;i<countWhiteBirch;i++) {
        if (!whiteBirch[i].breaked) {
            x2 = whiteBirch[i].position.x;
            z2 = whiteBirch[i].position.z;
            distance = sqrt((x1-x2)*(x1-x2) + (z1-z2)*(z1-z2));
            if (distance < 1.0f) {
                temPosition = whiteBirch[i].position - towardDirection * 1.5f;
                position.x = temPosition.x;
                position.z = temPosition.z;
            }
        }
    }
    for (int i=0;i<countTreeApple;i++) {
        if (!treeApple[i].breaked) {
            x2 = treeApple[i].position.x;
            z2 = treeApple[i].position.z;
            distance = sqrt((x1-x2)*(x1-x2) + (z1-z2)*(z1-z2));
            if (distance < 1.0f) {
                temPosition = treeApple[i].position - towardDirection * 1.5f;
                position.x = temPosition.x;
                position.z = temPosition.z;
            }
        }
    }

    // 箱子不能碰撞穿模
    glm::vec3 forward = direction;
    glm::vec3 right = glm::normalize(glm::cross(forward, upVector));
    bool Collision_front = false;
    bool Collision_left = false;
    bool Collision_right = false;
    bool Collision_back = false;
    for (int i = 0; i < numWoodbox; i++) {
        if(glm::distance(position+forward * glm::vec3(2.0f), woodBoxs[i].position) < 1.0f){
                Collision_front |= true;
        }else{
            Collision_front |= false;
        }
        if(glm::distance(position-forward * glm::vec3(2.0f), woodBoxs[i].position) < 1.0f){
                Collision_back |= true;
        }else{
            Collision_back |= false;
        }
        if(glm::distance(position+right * glm::vec3(2.0f), woodBoxs[i].position) < 1.0f){
                Collision_right |= true;
        }else{
            Collision_right |= false;
        }
        if(glm::distance(position-right * glm::vec3(2.0f), woodBoxs[i].position) < 1.0f){
                Collision_left |= true;
        }else{
            Collision_left |= false;
        }
    }

    for(int i = 0; i < numMetalBox; i++){
        if(glm::distance(position+forward * glm::vec3(2.0f), metalBox_breakables[i].position) < 1.0f){
                Collision_front |= true;
        }else{
            Collision_front |= false;
        }
        if(glm::distance(position-forward * glm::vec3(2.0f), metalBox_breakables[i].position) < 1.0f){
                Collision_back |= true;
        }else{
            Collision_back |= false;
        }
        if(glm::distance(position+right * glm::vec3(2.0f), metalBox_breakables[i].position) < 1.0f){
                Collision_right |= true;
        }else{
            Collision_right |= false;
        }
        if(glm::distance(position-right * glm::vec3(2.0f), metalBox_breakables[i].position) < 1.0f){
                Collision_left |= true;
        }else{
            Collision_left |= false;
        }
    }

    for(int i = 0; i < numMetalBox_B; i++){
        if(glm::distance(position+forward * glm::vec3(2.0f), metalBox_Bs[i].position) < 1.0f){
                Collision_front |= true;
        }else{
            Collision_front |= false;
        }
        if(glm::distance(position-forward * glm::vec3(2.0f), metalBox_Bs[i].position) < 1.0f){
                Collision_back |= true;
        }else{
            Collision_back |= false;
        }
        if(glm::distance(position+right * glm::vec3(2.0f), metalBox_Bs[i].position) < 1.0f){
                Collision_right |= true;
        }else{
            Collision_right |= false;
        }
        if(glm::distance(position-right * glm::vec3(2.0f), metalBox_Bs[i].position) < 1.0f){
                Collision_left |= true;
        }else{
            Collision_left |= false;
        }
    }

    for(int i = 0; i < numMetalBox_C; i++){
        if(glm::distance(position+forward * glm::vec3(2.0f), metalBox_Cs[i].position) < 1.0f){
                Collision_front |= true;
        }else{
            Collision_front |= false;
        }
        if(glm::distance(position-forward * glm::vec3(2.0f), metalBox_Cs[i].position) < 1.0f){
                Collision_back |= true;
        }else{
            Collision_back |= false;
        }
        if(glm::distance(position+right * glm::vec3(2.0f), metalBox_Cs[i].position) < 1.0f){
                Collision_right |= true;
        }else{
            Collision_right |= false;
        }
        if(glm::distance(position-right * glm::vec3(2.0f), metalBox_Cs[i].position) < 1.0f){
                Collision_left |= true;
        }else{
            Collision_left |= false;
        }
    }

    if(Collision_front){
        position -= forward * speed * deltaTime;
    }
    if(Collision_back){
        position += forward * speed * deltaTime;
    }
    if(Collision_right){
        position -= right * speed * deltaTime;
    }
    if(Collision_left){
        position += right * speed * deltaTime;
    }


    // 处理击剑动画
    if(actionCount % 1 == 0 && weaponFactor <= 1.0f && mouseLeft){
        weaponFactor = weaponFactor >= 1.0f ? 1.0f : weaponFactor + 0.05;
        actionCount ++;
        isAttacking = true;
    }else if(!mouseLeft){
        weaponFactor = 0.0f;
        isAttacking = false;
    }
    // 处理举盾动画
    if(actionCount % 1 == 0 && shieldFactor <= 1.0f && mouseRight){
            shieldFactor = shieldFactor >= 1.0f ? 1.0f : shieldFactor + 0.1;
            actionCount ++;
    }else if(!mouseRight){
        shieldFactor = 0.0f;
    }

    if (bomb_state && !playerBomb->explode) {
        playerBomb->active = ( playerBomb->active + 1 ) % 3;
        if (playerBomb->active == 1) {
            playerBomb->velocity = direction * 15.0f;
            playerBomb->velocity.y += 10.0f;
        }
        if (playerBomb->active == 0) {
            playerBomb->land = false;
            playerBomb->life = 1.0f;
            playerBomb->explode = true;
            x1 = playerBomb->position.x;
            z1 = playerBomb->position.z;
            for (int i = 0; i < countWhiteBirch; i++) {
                x2 = whiteBirch[i].position.x;
                z2 = whiteBirch[i].position.z;
                distance = sqrt((x1-x2)*(x1-x2) + (z1-z2)*(z1-z2));
                if (distance < 4.0f && !whiteBirch[i].breaked && playerBomb->position.y - whiteBirch[i].position.y < 7.0f) {
                    whiteBirch[i].breaked = true;
                }
            }
            for (int i = 0; i < countTreeApple; i++){
                x2 = treeApple[i].position.x;
                z2 = treeApple[i].position.z;
                distance = sqrt((x1-x2)*(x1-x2) + (z1-z2)*(z1-z2));
                if (distance < 4.0f && !treeApple[i].breaked && playerBomb->position.y - treeApple[i].position.y < 4.0f) {
                    treeApple[i].breaked = true;
                }
            }
        }
    }

    if (playerBomb->active == 1) {
        playerBomb->position = position + upVector * length.y;
    } else if (playerBomb->active == 2)
        playerBomb->moveParabola(terrain, deltaTime);


    // 判断边界
    glm::vec3 length = getLength();
    if (position.x > MAP_SZIE.x / 2.0f - length.x / 2.0f) position.x = MAP_SZIE.x / 2.0f - length.x / 2.0f;
    else if (position.x < - MAP_SZIE.x / 2.0f + length.x / 2.0f) position.x = - MAP_SZIE.x / 2.0f + length.x / 2.0f;
    if (position.z > MAP_SZIE.y / 2.0f - length.z / 2.0f) position.z = MAP_SZIE.y / 2.0f - length.z / 2.0f;
    else if (position.z < - MAP_SZIE.y / 2.0f + length.z / 2.0f) position.z = - MAP_SZIE.y / 2.0f + length.z / 2.0f;
    return;
}

void Player::DrawLine(Shader& line_shader) {
    glm::vec3 start = position;
    glm::vec3 end = start + direction * 50.0f;
    end.y += 10.0f;
    std::cout << "Draw line from " << glm::to_string(start) << " to " << glm::to_string(end) << std::endl;
    float vertices[] = {
            start.x, start.y, start.z,
            end.x, end.y, end.z
    };
    unsigned int indices[] = {0, 1};

    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &lineEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    line_shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    line_shader.setMat4("model", model);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // glDeleteVertexArrays(1, &lineVAO);
    // glDeleteBuffers(1, &lineVBO);
    // glDeleteBuffers(1, &lineEBO);
}


void Player::DoJump(Terrain* terrain, float deltaTime,bool fly) {
    float currentJumpHeight = position.y;
    bool isTrulyFly = (currentJumpHeight > terrain->getHeight(position.x, position.z) + length.y + 4.05f || isflying) ? fly : false;
    if(isTrulyFly){
        isflying = true;
    }
    position += isTrulyFly? direction * jumpHorizenSpeed * deltaTime : jumpDirection * jumpHorizenSpeed * deltaTime;
    float speedInair = isTrulyFly? jumpUpSpeed * 0.3 : jumpUpSpeed;
    color = isTrulyFly? flyColor : landColor;
    if (jumpUp) {
        currentJumpHeight += speedInair * deltaTime;
        if (currentJumpHeight >= targetJumpHeight || isTrulyFly) {
            jumpUp = false;
            currentJumpHeight = isTrulyFly? currentJumpHeight : targetJumpHeight;
        }
    } else {
        currentJumpHeight -= speedInair * deltaTime;
        glm::vec3 new_normal = terrain->getNormal(position.x, position.z);
        glm::vec3 terrainPosition(position.x, terrain->getHeight(position.x, position.z), position.z);
        glm::vec3 ground_position = terrainPosition + new_normal * (length.y / 2.0f);
        if (currentJumpHeight <= ground_position.y) {
            currentJumpHeight = ground_position.y;
            state = IDLE_LAND;
            isflying = false;
        }
    }
    position.y = currentJumpHeight;
}

void Player::Rebind() {
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