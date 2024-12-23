#version 330 core

in vec3 worldFragPos;
in vec3 normal;
in vec2 texCoords;
in vec4 shadowFragPos;
in vec3 Wcolor;


out vec4 FragColor;

uniform vec3 objectColor;
uniform float alpha;

layout (std140) uniform View {
    vec3 viewPos;
};

layout (std140) uniform Light {
    vec3 lightColor;
    vec3 lightPos;
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
};

uniform sampler2D shadowMap;
uniform sampler2D diffuseTexture;

vec2 poissonDisk[32] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790),
    vec2(-0.71901624, -0.29906216), vec2(0.84558609, -0.66890725),
    vec2(-0.194184101, -0.82938870), vec2(0.24495938, 0.19387760),
    vec2(-0.81588581, 0.35771432), vec2(-0.71544232, -0.77912464),
    vec2(-0.28277543, 0.17676845), vec2(0.87484398, 0.65648379),
    vec2(0.34323325, -0.87511554), vec2(0.43742981, -0.37373420),
    vec2(-0.16496911, -0.31893023), vec2(0.69197514, 0.09090188),
    vec2(-0.14188840, 0.89706507), vec2(-0.71409955, 0.81437590),
    vec2(0.09984126, 0.68641367), vec2(0.04383161, -0.04100790)
);
const int SAMPLES = 32;
const float blurSize = 1 / 4096.0;
float calcShadow(vec4 FragPos, vec3 normal, vec3 sunDir) {
    float shadow = 0.0;
    float diff = dot(normal, sunDir);
    vec3 shadowPos = FragPos.xyz / FragPos.w; // 转换到 NDC 坐标系，在透视投影的 w 分量不为 1 时有用
    shadowPos = shadowPos * 0.5 + 0.5; // 转换到 [0, 1] 范围
    shadowPos.z -= 0.001; // 防止 z-fighting
    float bias = max(0.05 * (1.0 - dot(normal, sunDir)), 0.005);
    float currentDepth = shadowPos.z;
    for (int i = 0; i < SAMPLES; ++i) {
        float pcfDepth = texture(shadowMap, shadowPos.xy + poissonDisk[i] * blurSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= float(SAMPLES);
    if (shadowPos.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main() {
    vec3 color = (objectColor == vec3(1.0,1.0,0.99))? Wcolor :objectColor;
    // vec3 objectColor = texture(diffuseTexture, texCoords).rgb;
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - worldFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - worldFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * color;

    float shadow = calcShadow(shadowFragPos, normal, lightDir);

    result = result * (1 - shadow);

    FragColor = vec4(result, alpha);
}
