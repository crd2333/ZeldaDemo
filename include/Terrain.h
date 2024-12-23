//
// Created by mf on 2024/12/10.
//

#pragma once

#include "Def.h"
#include "MyTexture.h"
#include "Shader.h"

/**
 *    Terrain is generated like a grid
 *                              z(cols)
 *  0.0 ───────────────────── 1.0 ─>
 *  │   0──────────────2
 *  │   │  upper   /   0
 *  │   │       /   /  │
 *  │   │    /   /     │ ......
 *  │   │ /   /        │
 *  │   1   /   lower  │
 *  │    1─────────────2
 *  │      ......
 *  1.0
 *  │
 *  v  x(rows)
 */

// TODO: 把地形切块，只渲染视锥内的部分，降低开销（尤其是水的反射和折射部分，因为我们的水不包含整个地图）

class Terrain {
private:
    GLuint VAO, VBO, EBO;
    int map_rows, map_cols; // resolution of the height map
    int rows, cols;         // resolution of the terrain map
    float gridSizeX, gridSizeZ;         // size per grid
    const glm::vec2 mapScale; // size of the terrain (size of the whole map)
    const float heightScale;  // scale of the height
    float textureScale; // scale of the texture (TODO: more complex blending)
    std::vector<glm::vec3> vertices;  // rows * cols
    std::vector<glm::vec2> texCoords; // rows * cols
    std::vector<glm::vec3> normals;   // rows * cols
    std::vector<std::array<GLuint, 6>> indices; // (rows - 1) * (cols - 1)
    // textures
    Texture2D* grass_texture = nullptr, *rock_texture = nullptr, *snow_texture = nullptr; // TODO, more textures

public:
    Terrain(const glm::vec2 mapScale, const float heightScale, const int resolutionRatio, const int sampleNum);
    ~Terrain();

    // 给定世界坐标 x, z，返回该点的高度和法向量
    float getHeight(const float& worldX, const float& worldZ) const;
    glm::vec3 getNormal(const float& worldX, const float& worldZ) const;

    void draw(Shader& shader, GLenum mode=GL_TRIANGLES) const;

    void debugHeight(const float resoluiton=5.0f) const;

private:
    glm::vec3 barycentricCoord(const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 p3, const glm::vec2 pos) const; // 给定三角形的三个顶点和一个点，计算该点的重心坐标 alpha, beta, gamma

    void generateMesh(unsigned char* heightMap, const int sampleNum); // 根据高度图生成网格型 mesh
};
