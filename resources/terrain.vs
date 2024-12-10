#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 texCoords;
out vec3 normal;
// out vec3 shadowFragPos;
// out vec3 worldFragPos;

uniform mat4 view;
uniform mat4 projection;
// uniform mat4 shadowMat; // todo

void main() {
    texCoords = aTexCoords;
    normal = aNormal;

    // worldFragPos = aPos;
    // vec4 shadowFrag = shadowMat * vec4(aPos, 1.0);
    // shadowFragPos = shadowFrag.xyz;

    gl_Position = projection * view * vec4(aPos, 1.0);
}