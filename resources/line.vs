#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 worldFragPos;

uniform mat4 model;
layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
};

void main()
{
    worldFragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = proj_view * vec4(worldFragPos, 1.0);
}