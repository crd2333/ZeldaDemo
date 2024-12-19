#version 330 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec4 shadowFragPos;
in vec3 worldFragPos;

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
uniform sampler2D terrain_texture1;

const float shadowBlurSize = 0.0005;

float calcShadow(vec4 FragPos, vec3 normal, vec3 sunDir) {
    float shadow = 0.0;
    float diff = dot(normal, sunDir);
    if (diff > 0.0) { // 只考虑那些朝向光源的片段
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
    }
    return shadow;
}

void main() {
    // vec3 color = texture(terrain_texture1, texCoords);
    vec3 color = normal;

    // ambient
    vec3 ambient = lightAmbient * 0.4;
    // diffuse
    vec3 sunDir = normalize(lightPos - worldFragPos); // vertex to light
    float diff =  max(dot(normal, sunDir), 0.0); // maybe negative
    vec3 diffuse = lightDiffuse * diff;
    // specular
    vec3 viewDir = normalize(viewPos - worldFragPos); // vertex to camera
    vec3 reflectDir = reflect(-sunDir, normal); // 反射光线
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // vec3 specular = lightSpecular * spec;
    vec3 specular = vec3(0.0); // no specular for terrain

    float shadow = calcShadow(shadowFragPos, normal, sunDir);
    color = (ambient + (1 - shadow) * (diffuse + specular)) * color; // apply shadow, exclude ambient

    // FragColor = vec4(shadow, shadow, shadow, 1.0);
    FragColor = vec4(color, 1.0);
}