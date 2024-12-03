#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
// 截取三角形图元，转化为线条图元(debug normal vector)

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2; // default length

uniform mat4 projection;

void GenerateLine(int index) {
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}