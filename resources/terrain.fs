#version 330 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
// in vec3 shadowFragPos;
// in vec3 worldFragPos;

// todo: light, shadow

uniform sampler2D terrain_texture1;

void main() {
    // todo: light, shadow
    // FragColor = texture(terrain_texture1, texCoords);
    FragColor = vec4(normal, 1.0);
}