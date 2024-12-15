#version 330 core
out vec4 FragColor;

in vec3 worldFragPos;
in vec2 texCoords;
// in vec4 projectionFragPos; // for reflection and refraction 可能可以考虑把投影位置转换到纹理坐标的过程直接在 vs 中做完，不知道有没有什么隐患

uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform float time;
uniform vec3 viewPos;

#define DISTORTION_SCALE 0.1
#define DISTORTION_POWER 0.781

void main() {
    // 采样 dudvMap 获取扰动值
    vec2 distortion = texture(dudvMap, texCoords).rg * DISTORTION_SCALE; // 只需前两个分量
    distortion = pow(distortion, vec2(DISTORTION_POWER)); // 引入非线性
    // 采样 normalMap 获取法线
    vec3 normal = texture(normalMap, texCoords + time + distortion).rgb;

    vec3 blueColor = vec3(0.0, 0.25, 1.0);

    FragColor = vec4(blueColor, 1.0); // 先不管那么多，先把颜色弄出来
}