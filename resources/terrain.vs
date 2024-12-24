#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 texCoords;
out vec3 normal;
out vec4 shadowFragPos;
out vec3 worldFragPos;
out mat3 TBN;

layout (std140) uniform Matrices {
    mat4 proj_view; // 把 projection 和 view 合到一起，少做一次乘法
    mat4 shadowMat;
};
// terrain 不使用 model 矩阵，因为生成顶点的时候就已经把它变换到 mapScale * [-0.5, 0.5]^2 的范围
uniform float waterHeight;
uniform bool doRefraction;
uniform bool doReflection;

void main() {
    texCoords = aTexCoords;
    normal = aNormal;

    worldFragPos = aPos;
    shadowFragPos = shadowMat * vec4(aPos, 1.0);

    // 计算 TBN 矩阵
    vec3 T = normalize(aTangent);
    vec3 B = normalize(aBitangent);
    vec3 N = normalize(aNormal);
    TBN = mat3(T, B, N);

    gl_Position = proj_view * vec4(aPos, 1.0);

    if (doRefraction) {
        gl_ClipDistance[0] = dot(vec4(aPos, 1.0), vec4(0.0, -1.0, 0.0, waterHeight + 4.0));
        // 裁剪平面的法向量为 (0.0, -1.0, 0.0)，即平面朝下的方向；裁剪平面的偏移量为 waterHeight + 4.0，即水面高度加上一个偏移量
        // 通过点积计算顶点 aPos 到裁剪平面的距离，并将结果赋值给 gl_ClipDistance[0]，如果结果为负值（在水面上方），则顶点被裁剪掉
    }
    if (doReflection) {
        gl_ClipDistance[0] = dot(vec4(aPos, 1.0), vec4(0.0, 1.0, 0.0, -waterHeight + 0.6));
        // 裁剪平面的法向量为 (0.0, 1.0, 0.0)，即平面朝上的方向；裁剪平面的偏移量为 -waterHeight + 0.6，即水面高度的负值加上一个偏移量
    }
}