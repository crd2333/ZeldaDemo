#version 330 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec4 shadowFragPos;
in vec3 worldFragPos;
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
uniform bool doRefraction;
uniform bool doReflection;

uniform sampler2D shadowMap;
uniform sampler2D terrain_texture1; // grass
uniform sampler2D terrain_texture2; // rock
uniform sampler2D terrain_texture3; // snow
uniform sampler2D terrain_texture4; // snow_grass_intermediate
uniform sampler2D normalMap1;
uniform sampler2D normalMap2;

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
    if (doRefraction) // no shadow for refraction
        return 0.0;
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

// 根据法向量的方向和 y 坐标混合不同的纹理
// 如果法向量跟 y 轴接近垂直（80° ~ 90°），使用岩石纹理
// 如果高度大于 200，使用雪纹理
vec3 mix_color(vec2 texCoords, vec3 normal, float y) {
    float deg = dot(normal, vec3(0.0, 1.0, 0.0)) * 0.5 + 0.5; // 0 ~ 1
    vec3 grassColor = texture(terrain_texture1, texCoords).rgb;
    vec3 rockColor = texture(terrain_texture2, texCoords * 5).rgb;
    vec3 snowColor = texture(terrain_texture3, texCoords).rgb;
    vec3 snowGrassColor = texture(terrain_texture4, texCoords).rgb;

    // 计算高度混合
    float snowRegionBlend = smoothstep(225.0, 250.0, y);
    float snowGrassBlend = smoothstep(200.0, 225.0, y);

    vec3 color = mix(grassColor, snowGrassColor, snowGrassBlend);
    color = mix(color, snowColor, snowRegionBlend);

    float rockBlend = smoothstep(0.7, 0.8, deg);
    color = mix(rockColor, color, rockBlend);
    return color;
}

// 根据法向量的方向和 y 坐标混合不同的法线贴图
vec3 mix_normal(vec2 texCoords, vec3 normal, float y) {
    float deg = dot(normal, vec3(0.0, 1.0, 0.0)) * 0.5 + 0.5; // 0 ~ 1
    vec3 grassNormal = texture(normalMap1, texCoords).rgb;
    vec3 rockNormal = texture(normalMap2, texCoords * 5).rgb;

    if (deg < 0.7)
        return normalize(TBN * (rockNormal * 2.0 - 1.0));
    else
        return (y < 230) ? normalize(TBN * (grassNormal * 2.0 - 1.0)) : normal;
}

void main() {
    // 采样 texture 混合得到最终颜色
    vec3 color = mix_color(texCoords, normal, worldFragPos.y);
    // 从法线贴图中获取扰动后的法线
    vec3 finalNormal = mix_normal(texCoords, normal, worldFragPos.y);

    // ambient is not affected by shadow or light direction
    // diffuse
    vec3 sunDir = normalize(lightPos - worldFragPos); // vertex to light
    float diff =  max(dot(finalNormal, sunDir), 0.0); // maybe negative
    vec3 diffuse = lightDiffuse * diff;
    // specular
    vec3 viewDir = normalize(viewPos - worldFragPos); // vertex to camera
    vec3 reflectDir = reflect(-sunDir, finalNormal); // 反射光线
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // vec3 specular = lightSpecular * spec;
    vec3 specular = vec3(0.0); // no specular for terrain

    float shadow = calcShadow(shadowFragPos, finalNormal, sunDir);
    color = (lightAmbient + (1 - shadow) * smoothstep(0, 1, diffuse + specular)) * color; // apply shadow, exclude ambient

    FragColor = vec4(color, 1.0);
}