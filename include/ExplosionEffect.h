// include/ExplosionEffect.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <vector>

// 粒子结构
struct Particle {
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec4 Color;
    float Life; // 粒子的生命周期
};

class ExplosionEffect {
private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
    glm::vec3 position;
    float duration;
    float elapsed;
    bool active;

    void init();
public:
    ExplosionEffect(glm::vec3 pos);
    ~ExplosionEffect();
    void update(float deltaTime);
    void draw(Shader &shader);
    bool isFinished() const { return elapsed >= duration; }
};
