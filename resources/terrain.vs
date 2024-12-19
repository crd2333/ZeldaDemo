#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 texCoords;
out vec3 normal;
out vec4 shadowFragPos;
out vec3 worldFragPos;

layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
    mat4 shadowMat;
};
// terrain 不使用 model 矩阵，因为生成顶点的时候就已经把它变换到 mapScale * [-0.5, 0.5]^2 的范围

void main() {
    texCoords = aTexCoords;
    normal = aNormal;

    worldFragPos = aPos;
    shadowFragPos = shadowMat * vec4(aPos, 1.0);

    gl_Position = proj_view * vec4(aPos, 1.0);
}