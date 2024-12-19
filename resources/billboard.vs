#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

// billboard 是“告示牌”的意思，是指一种使物体始终朝向摄像机的技术，参考 https://blog.csdn.net/ZJU_fish1996/article/details/84035220
// 常用于太阳、月亮、树木等物体，有两种形式：一种是始终朝向摄像机，另一种是始终朝向摄像机的水平方向（y 轴固定），暂不考虑后者

// uniform mat4 model;
uniform vec3 objPos; // 不采用 model 矩阵，直接传入 objPos 和定义 objSize，in vec2 的 aPos 只作为 offset 使用
const vec2 objSize = vec2(50.0f, 50.0f);

layout (std140) uniform Proj_View {
    mat4 projection;
    mat4 view;
};
uniform bool lockYAxis = false;

void main() {
    // vec3 objPos = vec3(model[3].x, model[3].y, model[3].z);
    // vec2 objSize = vec2(model[0].x, model[1].y);
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(lockYAxis ? 0 : view[0][1], view[1][1], lockYAxis ? 0 : view[2][1]);
    vec3 worldPos = objPos + cameraRight * aPos.x * objSize.x + cameraUp * aPos.y * objSize.y;

    gl_Position = projection * view * vec4(worldPos, 1.0f);
    gl_Position.z = gl_Position.w - 0.001f; // 略微减小深度值，防止 z-fighting
    TexCoords = aTexCoords;
}