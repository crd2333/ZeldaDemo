#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexcoord;

out vec2 texCoords;
out vec4 shadowFragPos;
out vec3 worldFragPos;
out vec4 projectionFragPos; // for reflection and refraction

layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
    mat4 shadowMat;
};
// water 不使用 model 矩阵，因为生成顶点的时候就已经把它变换到 [-0.5, 0.5]^2 的范围
uniform vec2 mapScale;
uniform float height; // 每条河有自己的 height

#define TEXT_SCALE 100.0

void main() {
    texCoords = aTexcoord; // texture 需要 Scale 吗？好像要，再说吧

    worldFragPos = vec3(aPos.x * mapScale.x, height, aPos.y * mapScale.y);
    shadowFragPos = shadowMat * vec4(worldFragPos, 1.0);

    gl_Position = proj_view * vec4(worldFragPos, 1.0);
    projectionFragPos = gl_Position;
}