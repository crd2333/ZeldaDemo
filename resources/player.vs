#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 worldFragPos;
out vec3 normal;
out vec2 texCoords;
out vec4 shadowFragPos;

uniform mat4 model;
uniform mat4 normalMat; // 在外部计算好 normal matrix（避免 GPU 频繁求逆）
layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
    mat4 shadowMat;
};

void main() {
    worldFragPos = vec3(model * vec4(aPos, 1.0));
    shadowFragPos = shadowMat * vec4(worldFragPos, 1.0);
    normal = normalize(vec3(normalMat * vec4(aNormal, 1.0)));
    texCoords = aTexCoords;

    gl_Position = proj_view * vec4(worldFragPos, 1.0);
}