#include "Bomb.h"

std::vector<Bomb> bombs;
std::vector<ExplosionEffect> explosions;

Bomb::Bomb(glm::vec3 position, glm::vec3 direction) : position(position), direction(direction), 
    color(1.0f,0.0f,0.0f), sphereGeometry(0.1f), speed(20.0f), gravity(9.8f), exploded(false) {
    direction.y += 0.7f;
    velocity = direction * speed;
    vertices = sphereGeometry.vertices;
    indices = sphereGeometry.indices;
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

void Bomb::draw(Shader &shader) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Bomb::update(Terrain* terrain, float deltaTime) {
    if (exploded) return;
    velocity.y -= gravity * deltaTime;
    position += velocity * deltaTime;
    float terrainHeight = terrain->getHeight(position.x, position.z);
    if (position.y - 0.5f <= terrainHeight) {
        position.y = terrainHeight + 0.5f;
        exploded = true;
        std::cout << "Bomb exploded at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    }
}