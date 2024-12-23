#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 ParticleColor;

uniform mat4 model;
layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
    mat4 shadowMat;
};

void main()
{
    ParticleColor = aColor;
    gl_Position = proj_view * vec4(vec3(model * vec4(aPos, 1.0)), 1.0);
    gl_PointSize = 10.0;
}