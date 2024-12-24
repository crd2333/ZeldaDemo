#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 worldFragPos;
in vec4 shadowFragPos;
in mat3 TBN;

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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_trans1;
uniform sampler2D shadowMap;
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform vec3 Ke;
uniform bool useTransTexture;

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
const float blurSize = 32 / 4096.0;
float calcShadow(vec4 FragPos, vec3 normal, vec3 sunDir) {
    float shadow = 0.0;
    vec3 shadowPos = FragPos.xyz / FragPos.w; // 转换到 NDC 坐标系，在透视投影的 w 分量不为 1 时有用
    shadowPos = shadowPos * 0.5 + 0.5; // 转换到 [0, 1] 范围
    if (shadowPos.z > 1.0)
        return 0.0;
    float bias = max(0.05 * (1.0 - dot(normal, sunDir)), 0.005);
    float currentDepth = shadowPos.z;
    for (int i = 0; i < SAMPLES; ++i) {
        float pcfDepth = texture(shadowMap, shadowPos.xy + poissonDisk[i] * blurSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= float(SAMPLES);

    shadow = pow(shadow, 0.781) * (1 - lightAmbient.r);
    return shadow;
}

void main() {
    vec3 color = texture(texture_diffuse1, texCoords).rgb;

    float alpha = useTransTexture ? texture(texture_trans1, texCoords).r : 1.0;
    if (alpha < 0.1) {
        discard;
    }

    // get normal from normal map
    vec3 normal = texture(texture_normal1, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // ambient
    vec3 ambient = lightAmbient;
    // diffuse
    vec3 lightDir = normalize(lightPos - worldFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff;
    // specular
    vec3 viewDir = normalize(viewPos - worldFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = lightSpecular * spec;
    // calculate shadow
    float shadow = calcShadow(shadowFragPos, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    vec3 result = lighting + Ke;

    result = mix(result, color, 0.5); // 还是之前的好看，用之前的吧（

    FragColor = vec4(result, alpha);
}