#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 shadowMat;

void main() {
    gl_Position = shadowMat * vec4(aPos, 1.0);
}
