#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 texCoords;
out vec3 worldFragPos;
out vec4 shadowFragPos;
out mat3 TBN;

uniform mat4 model;
layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
    mat4 shadowMat;
};

void main() {
    worldFragPos = vec3(model * vec4(aPos, 1.0));
    shadowFragPos = shadowMat * vec4(worldFragPos, 1.0);
    texCoords = aTexCoords;
    TBN = mat3(normalize(aTangent), normalize(aBitangent), normalize(aNormal));
    gl_Position = proj_view * vec4(worldFragPos, 1.0);
}