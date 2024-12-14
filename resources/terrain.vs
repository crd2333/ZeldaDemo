#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 texCoords;
out vec3 normal;
// out vec3 shadowFragPos;
// out vec3 worldFragPos;

layout (std140) uniform Proj_View {
    mat4 projection;
    mat4 view;
};
// terrain 不使用 model 矩阵，因为生成顶点的时候就已经把它变换到 mapScale * [-0.5, 0.5]^2 的范围
// uniform mat4 shadowMat; // todo

void main() {
    texCoords = aTexCoords;
    normal = aNormal;

    // worldFragPos = aPos;
    // vec4 shadowFrag = shadowMat * vec4(aPos, 1.0);
    // shadowFragPos = shadowFrag.xyz;

    gl_Position = projection * view * vec4(aPos, 1.0);
}