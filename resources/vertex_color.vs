#version 330 core

layout(location = 0) in vec3 aPos; // 顶点位置
layout(location = 1) in vec3 aColor; // 顶点颜色

out vec3 ourColor; // 传递给片段着色器的颜色

layout (std140) uniform Matrices {
    mat4 proj_view; 
};
uniform mat4 model;

void main() {
    gl_Position = proj_view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}