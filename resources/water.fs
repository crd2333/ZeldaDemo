#version 330 core
out vec4 FragColor;

in vec3 worldFragPos;
in vec2 texCoords;
in vec4 shadowFragPos;
// in vec4 projectionFragPos; // for reflection and refraction 可能可以考虑把投影位置转换到纹理坐标的过程直接在 vs 中做完，不知道有没有什么隐患

uniform sampler2D shadowMap;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform float time;
layout (std140) uniform View {
    vec3 viewPos;
};

#define DISTORTION_SCALE 0.01
#define DISTORTION_SCALE_TIME 0.1
#define DISTORTION_POWER 0.781

layout (std140) uniform Light {
    vec3 lightColor;
    vec3 lightPos;
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
};

float calcShadow(vec4 FragPos, vec3 normal, vec3 sunDir) {
    float shadow = 0.0;
    float diff = dot(normal, sunDir);
    vec3 shadowPos = FragPos.xyz / FragPos.w; // 转换到 NDC 坐标系，在透视投影的 w 分量不为 1 时有用
    shadowPos = shadowPos * 0.5 + 0.5; // 转换到 [0, 1] 范围
    // shadowPos.z -= 0.001; // 防止 z-fighting
    float bias = max(0.05 * (1.0 - dot(normal, sunDir)), 0.005);
    float currentDepth = shadowPos.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); // 查询第 0 级 mipmap 的尺寸，得到一个 texel 的大小
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, shadowPos.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (shadowPos.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main() {
    // 采样 dudvMap 获取扰动值
    vec2 distortion = texture(dudvMap, texCoords).rg * DISTORTION_SCALE; // 只需前两个分量
    distortion = pow(distortion, vec2(DISTORTION_POWER)); // 引入非线性
    // 采样 normalMap 获取法线
    vec2 dist_time = vec2(time) * DISTORTION_SCALE_TIME;
    vec3 normal = texture(normalMap, texCoords + dist_time + distortion).rgb;
    // vec3 normal = vec3(0.0, 1.0, 0.0); // 先假装这是法线

    vec3 color = vec3(0.0, 0.25, 1.0); // 假装这是纹理采出来的

    vec3 lightDir = normalize(lightPos - worldFragPos);
    // ambient
    vec3 ambient = lightAmbient * 0.4;
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff;
    // specular
    vec3 viewDir = normalize(viewPos - worldFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
    vec3 specular = lightSpecular * spec;

    float shadow = calcShadow(shadowFragPos, normal, lightDir);

    // vec3 res = (ambient + diffuse + specular) * color;
    color = (ambient + (1 - shadow) * (diffuse + specular)) * color; // apply shadow, exclude ambient

    // FragColor = vec4(shadow, shadow, shadow, 1.0); // 先不管那么多，先把颜色弄出来
    FragColor = vec4(color, 1.0);
}