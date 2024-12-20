#include "Camera.h"
#include "GameWindow.h"

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

void Camera::UpdateThirdPerson(Terrain* terrain, Player *player,
    float distance, float heightOffset) {
    glm::vec3 playerPos = player->getPosition();
    // Calculate desired camera position based on spherical coordinates
    float x = distance * sin(sphericalPhi) * cos(sphericalTheta);
    float y = distance * cos(sphericalPhi) - heightOffset;
    float z = distance * sin(sphericalPhi) * sin(sphericalTheta);
    glm::vec3 desiredPos = playerPos + glm::vec3(x, y, z);

    // Ray casting to prevent terrain obstruction
    const int maxSteps = 50;
    float step = distance / maxSteps;
    glm::vec3 direction = glm::normalize(desiredPos - playerPos);
    player->setDirection(-direction);
    float currentDistance = 0.0f;
    glm::vec3 finalPos = desiredPos;

    for(int i = 0; i < maxSteps; ++i) {
        currentDistance += step;
        glm::vec3 samplePos = playerPos + direction * currentDistance;
        float terrainHeight = terrain->getHeight(samplePos.x, samplePos.z);
        if(samplePos.y < terrainHeight + 1.0f) { // Prevent camera from intersecting terrain
            finalPos = playerPos + direction * (currentDistance - step);
            break;
        }
    }

    // targetPosition = finalPos;

    // // Smoothly interpolate to target position
    // float distanceToTarget = glm::length(targetPosition - Position);
    // if (distanceToTarget > minDistance) {
    //     Position = glm::mix(Position, targetPosition, smoothSpeed * deltaTime);
    // }

    Position = finalPos;

    // Update Front vector to look at the player
    Front = glm::normalize(playerPos - Position);

    // Recalculate Right and Up vectors
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}

void Camera::ProcessMouseOrbit(float deltaX, float deltaY) {
    float angleX = deltaX * MouseSensitivity;
    float angleY = deltaY * MouseSensitivity;

    sphericalTheta += angleX;
    sphericalPhi += angleY;

    constrainAngles();
}

void Camera::constrainAngles() {
    float phiMin = glm::radians(10.0f);
    float phiMax = glm::radians(85.0f);
    if (sphericalPhi < phiMin)
        sphericalPhi = phiMin;
    if (sphericalPhi > phiMax)
        sphericalPhi = phiMax;

    if (sphericalTheta < 0.0f)
        sphericalTheta += 2.0f * glm::pi<float>();
    if (sphericalTheta > 2.0f * glm::pi<float>())
        sphericalTheta -= 2.0f * glm::pi<float>();
}
