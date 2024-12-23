#include "ExplosionEffect.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cstdlib> 

ExplosionEffect::ExplosionEffect(glm::vec3 pos)
    : position(pos), duration(2.0f), elapsed(0.0f), active(true)
{
    init();
}

ExplosionEffect::~ExplosionEffect()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ExplosionEffect::init()
{
    const unsigned int NUM_PARTICLES = 100;

    particles.reserve(NUM_PARTICLES);
    for (unsigned int i = 0; i < NUM_PARTICLES; ++i)
    {
        Particle p;
        p.Position = position;

        float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.1415926f;
        float phi = static_cast<float>(rand()) / RAND_MAX * 3.1415926f;
        float speed = static_cast<float>(rand()) / RAND_MAX * 15.0f + 10.0f; 
        p.Velocity = glm::vec3(
            speed * sin(phi) * cos(theta),
            speed * sin(phi) * sin(theta),
            speed * cos(phi)
        );
        p.Color = glm::vec4(
            static_cast<float>(rand()) / RAND_MAX * 1.0f, 
            static_cast<float>(rand()) / RAND_MAX * 0.5f, 
            0.0f, 
            1.0f  
        );
        p.Life = 1.0f; 
        particles.push_back(p);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, Color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void ExplosionEffect::update(float deltaTime)
{
    if (!active) return;

    elapsed += deltaTime;
    if (elapsed >= duration)
    {
        active = false;
        return;
    }

    for (auto& p : particles)
    {
        p.Life -= deltaTime;
        if (p.Life > 0.0f)
        {
            p.Position += p.Velocity * deltaTime;
            p.Velocity.y -= 9.8f * deltaTime;
            p.Color.a = p.Life;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), &particles[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ExplosionEffect::draw(Shader &shader)
{
    if (!active) return;
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.size()));
    glBindVertexArray(0);
}
