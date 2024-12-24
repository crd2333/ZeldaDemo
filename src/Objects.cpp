#include "Objects.h"

void Bomb::moveParabola(Terrain* terrain, float t) {
    if (land) return;
    velocity.y -= gravity * t;
    position += velocity * t;
    float terrainHeight = terrain->getHeight(position.x, position.z);
    if (position.y <= terrainHeight + 1.0f) {
        land = true;
        std::cout << "Bomb exploded at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    }
}

void Bomb::Explode(Shader& shader, float deltaTime) {
    shader.use();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    shader.setVec3("objectColor", glm::vec3(0.53f, 0.81f, 0.98f));
    shader.setFloat("alpha", 0.5f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    life -= deltaTime;
    if (life <= 0.0f) {
        life=0.0f;
        explode = false;
    }
}