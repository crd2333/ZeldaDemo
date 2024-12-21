#version 330 core

in vec3 worldFragPos;
in vec3 normal;
in vec2 texCoords;
in vec4 shadowFragPos;

out vec4 FragColor;

uniform vec3 objectColor;

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

float calcShadow(vec4 FragPos, vec3 normal, vec3 sunDir) {
    float shadow = 0.0;
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
    vec3 color = objectColor;
    // vec3 objectColor = texture(diffuseTexture, texCoords).rgb;
    float ambientStrength = 0.1;
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

    vec3 result = (ambient + diffuse + specular) * objectColor;

    float shadow = calcShadow(shadowFragPos, normal, lightDir);

    result = result * (1 - shadow);

    FragColor = vec4(result, 1.0);
}
