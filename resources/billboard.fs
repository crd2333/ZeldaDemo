#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D billboardTex;

void main() {
    vec4 color = texture(billboardTex, TexCoords);
    if (color.a < 0.1)
        discard;
    FragColor = color;
    // FragColor = vec4(color.a, color.a, color.a, 1.0);
}