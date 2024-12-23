#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Terrain.h"
#include "ExplosionEffect.h"
#include "geometry/SphereGeometry.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

class Bomb {
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    glm::vec3 velocity;
    SphereGeometry sphereGeometry;
    float speed;
    float gravity;
    bool exploded;
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
public:
    Bomb(glm::vec3 position, glm::vec3 direction);
    ~Bomb() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    void draw(Shader &shader);
    void update(Terrain* terrain, float deltaTime);
    bool isExploded() { return exploded; }
    glm::vec3 getPosition() const { return position; }
};

extern std::vector<Bomb> bombs;
extern std::vector<ExplosionEffect> explosions;