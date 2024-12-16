#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float near, float far) :
  MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM),
  Position(position), WorldUp(up), Near(near), Far(far), Yaw(yaw), Pitch(pitch) {
    updateCameraVectors();
}

// constructor with scalar values (delegate to vec3 version)
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float near, float far) :
  Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch, near, far) {}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UP)   // UP and DOWN are self-defined
        Position += Up * velocity;
    if (direction == DOWN)
        Position -= Up * velocity;

    // constrain the camera's position
    Position.x = CLAMP(Position.x, -MAP_SZIE.x / 2, MAP_SZIE.x / 2 - 1.f); // minus additional 1.f to avoid the edge case
    Position.z = CLAMP(Position.z, -MAP_SZIE.y / 2, MAP_SZIE.y / 2 - 1.f);
    Position.y = CLAMP(Position.y, terrain->getHeight(Position.x, Position.z) + 3.0f, 600.0f);
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom = CLAMP(Zoom - (float)yoffset, 1.0f, 45.0f);
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));
}

void Camera::BindUBO(const Shader& shader) const {
    uboProjView->Bind(shader);
}

void Camera::SetUBO() const {
    glm::mat4 projection = GetPerspectiveMatrix();
    glm::mat4 view = GetViewMatrix();
    SetUBO(&projection, &view);
}

void Camera::SetUBO(glm::mat4* projection, glm::mat4* view) const {
    uboProjView->setData(0, sizeof(glm::mat4), projection);
    uboProjView->setData(sizeof(glm::mat4), sizeof(glm::mat4), view);
}

void Camera::UpdateThirdPerson(const glm::vec3& playerPos, const glm::vec3& playerFront, Terrain* terrain, float distance, float heightOffset) {
    // 长方体后方一定距离，并有一定的高度偏移
    glm::vec3 playerForward = glm::normalize(glm::vec3(playerFront.x, 0.0f, playerFront.z)); 
    glm::vec3 desiredPos = playerPos - playerForward * distance + glm::vec3(0.0f, heightOffset, 0.0f);

    // 使用简单的射线投射来检测阻挡
    const int maxSteps = 100;
    float step = 1.0f / maxSteps;
    glm::vec3 direction = glm::normalize(desiredPos - playerPos);
    float totalDistance = glm::length(desiredPos - playerPos);
    float currentDistance = 0.0f;
    glm::vec3 finalPos = desiredPos;

    for(int i = 0; i < maxSteps; ++i) {
        currentDistance += step * totalDistance;
        glm::vec3 samplePos = playerPos + direction * currentDistance;
        float terrainHeight = terrain->getHeight(samplePos.x, samplePos.z);
        if(samplePos.y < terrainHeight + 1.0f) { // 防止相机与地形重叠
            finalPos = playerPos + direction * (currentDistance - step * totalDistance);
            break;
        }
    }

    // 设置相机的位置
    Position = finalPos;

    // 设置相机的 Front 向量为指向长方体的位置
    Front = glm::normalize(playerPos - Position);

    // 重新计算 Right 和 Up 向量
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}