#include "Terrain.h"

Terrain::Terrain(const glm::vec2 mapScale, const float heightScale, const int resolutionRatio, const int sampleNum) : mapScale(mapScale), heightScale(heightScale), textureScale(3.f) {
    // height map and generate mesh
    int components;
    // unsigned char* heightMap = stbi_load("resources/textures/heightmap.png", &cols, &rows, &components, 0);
    unsigned char* heightMap = stbi_load("resources/textures/heightmap.png", &map_cols, &map_rows, &components, 0);
    if (!heightMap || components != 1)
        Err("Something wrong with the height map! Check the path and format.");

    rows = map_rows * resolutionRatio; // resolution of the terrain
    cols = map_cols * resolutionRatio;
    vertices.resize(rows * cols);  // pre-allocate memory
    texCoords.resize(rows * cols);
    normals.resize(rows * cols);
    indices.resize((rows - 1) * (cols - 1));
    generateMesh(heightMap, sampleNum);
    stbi_image_free(heightMap); // release after generating mesh
    gridSizeX = mapScale.x / (cols - 1);
    gridSizeZ = mapScale.y / (rows - 1);

    // TODO: more textures
    grass_texture = new Texture2D();
    grass_texture->image_format = GL_RGBA;
    grass_texture->Generate("resources/textures/grass.png");

    // generate and bind buffer
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * (2 * sizeof(glm::vec3) + sizeof(glm::vec2)), NULL, GL_STATIC_DRAW); // vertices, texCoords, normals
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::array<GLuint, 6>), &indices[0], GL_STATIC_DRAW);
    // data and attribute: vertices, texCoords, normals
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.size() * sizeof(glm::vec2), &texCoords[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(glm::vec3) + sizeof(glm::vec2)), vertices.size() * sizeof(glm::vec3), &normals[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices.size() * (sizeof(glm::vec3) + sizeof(glm::vec2))));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0);
}

Terrain::~Terrain() {
    if (grass_texture) delete grass_texture;
    if (rock_texture) delete rock_texture;
    if (snow_texture) delete snow_texture;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// 给定世界坐标 x, z，返回该点的高度
float Terrain::getHeight(const float& worldX, const float& worldZ) const {
    // 世界坐标到网格坐标
    float terrainX = worldX + 0.5 * mapScale.x; // terrain x: [-mapScale.x / 2, mapScale.x / 2] -> [0, mapScale.x]
    float terrainZ = worldZ + 0.5 * mapScale.y;
    int gridX = terrainX / gridSizeX;       // grid x: [0, mapScale.x] -> [0, cols - 1]
    int gridZ = terrainZ / gridSizeZ;
    // 世界坐标到网格内偏移量
    float coordX = fmod(terrainX, gridSizeX) / gridSizeX; // [0, mapScale.x] -> [0, gridSizeX) -> [0, 1)
    float coordZ = fmod(terrainZ, gridSizeZ) / gridSizeZ;
    glm::vec3 barycentric;
    glm::vec3 valueToInterpolate;
    if (coordX + coordZ <= 1.0f) { // upper triangle
        barycentric = barycentricCoord(
                glm::vec2(0, 0),
                glm::vec2(1, 0),
                glm::vec2(0, 1),
                glm::vec2(coordX, coordZ)
            );
        valueToInterpolate = {
                vertices[gridZ * cols + gridX].y,
                vertices[(gridZ + 1) * cols + gridX].y,
                vertices[gridZ * cols + gridX + 1].y
            };
    } else {                       // lower triangle
        barycentric = barycentricCoord(
                glm::vec2(0, 1),
                glm::vec2(1, 0),
                glm::vec2(1, 1),
                glm::vec2(coordX, coordZ)
            );
        valueToInterpolate = {
                vertices[gridZ * cols + gridX + 1].y,
                vertices[(gridZ + 1) * cols + gridX].y,
                vertices[(gridZ + 1) * cols + gridX + 1].y
            };
    }
    return glm::dot(barycentric, valueToInterpolate);
}

// 给定世界坐标 x, z，返回该点的法向量
glm::vec3 Terrain::getNormal(const float& worldX, const float& worldZ) const {
    float terrainX = worldX + 0.5 * mapScale.x;
    float terrainZ = worldZ + 0.5 * mapScale.y;
    int gridX = terrainX / gridSizeX;
    int gridZ = terrainZ / gridSizeZ;
    float coordX = fmod(terrainX, gridSizeX) / gridSizeX;
    float coordZ = fmod(terrainZ, gridSizeZ) / gridSizeZ;

    glm::vec3 barycentric;
    std::vector<glm::vec3> valueToInterpolate;
    if (coordX + coordZ <= 1.0f) { // upper triangle
        barycentric = barycentricCoord(
                glm::vec2(0, 0),
                glm::vec2(1, 0),
                glm::vec2(0, 1),
                glm::vec2(coordX, coordZ)
            );
        valueToInterpolate = {
                normals[gridZ * cols + gridX],
                normals[(gridZ + 1) * cols + gridX],
                normals[gridZ * cols + gridX + 1]
            };
    } else {                       // lower triangle
        barycentric = barycentricCoord(
                glm::vec2(0, 1),
                glm::vec2(1, 0),
                glm::vec2(1, 1),
                glm::vec2(coordX, coordZ)
            );
        valueToInterpolate = {
                normals[gridZ * cols + gridX + 1],
                normals[(gridZ + 1) * cols + gridX],
                normals[(gridZ + 1) * cols + gridX + 1]
            };
    }
    return glm::normalize(barycentric.x * valueToInterpolate[0] + barycentric.y * valueToInterpolate[1] + barycentric.z * valueToInterpolate[2]);
}

void Terrain::draw(Shader& shader, GLenum mode) const {
    // grass_texture->Bind(0);
    grass_texture->UnBind();
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(mode, indices.size() * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// 给定三角形的三个顶点和一个点，计算该点的重心坐标 alpha, beta, gamma
glm::vec3 Terrain::barycentricCoord(const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 p3, const glm::vec2 pos) const {
    // see https://zhuanlan.zhihu.com/p/144360079 for derivation details
    // u.x = (x2 - x1)(y1 - y) - (y2 - y1)(x1 - x) = (y2 - y1)x + (x1 - x2)y + x2y1 - x1y2
    // u.y = (x1 - x)(y3 - y1) - (y1 - y)(x3 - x1) = (y1 - y3)x + (x3 - x1)y + x1y3 - x3y1
    // u.z = (x3 - x1)(y2 - y1) - (y3 - y1)(x2 - x1) = (y2 - y1)x3 + (x1 - x2)y3 +  x2y1 - x1y2
    glm::vec3 u = glm::cross(
            //             i,           j,           k
            glm::vec3(p3.x - p1.x, p2.x - p1.x, p1.x - pos.x),
            glm::vec3(p3.y - p1.y, p2.y - p1.y, p1.y - pos.y)
        );
    if (std::abs(u.z) < 1)
        Warn("Barycentric coordinate is degenerate, u.z = " + std::to_string(u.z));
    return glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

// 根据高度图生成网格型 mesh
void Terrain::generateMesh(unsigned char* heightMap, const int sampleNum) {
    /************************** Generate Vertices and TexCoords ************************/
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // map coords to height map coords
            float heightmapX = (float)j / (cols - 1) * (map_cols - 1);
            float heightmapY = (float)i / (rows - 1) * (map_rows - 1);

            // get nearest neighbor as the base height
            int x = (int)round(heightmapX);
            int y = (int)round(heightmapY);
            x = (x >= map_cols) ? map_cols - 1 : (x < 0) ? 0 : x; // boundary check: clamp to [0, map_cols - 1]
            y = (y >= map_rows) ? map_rows - 1 : (y < 0) ? 0 : y;

            float baseHeight = heightMap[y * map_cols + x] / 255.f * heightScale;

            float cliffHeight = heightScale * 0.1f;

            // weighted sum
            float heightSum = 0.0f;
            float weightSum = 0.0f;
            for (int dy = -sampleNum; dy <= sampleNum; dy++) {
                for (int dx = -sampleNum; dx <= sampleNum; dx++) {
                    if (dx == 0 && dy == 0) continue; // jump the center point
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < map_cols && ny >= 0 && ny < map_rows) { // boundary check
                        float neighborHeight = heightMap[ny * map_cols + nx] / 255.f * heightScale;
                        float weight = 1.0f;
                        if (abs(neighborHeight - baseHeight) > cliffHeight)
                            weight = 0.1f;

                        heightSum += neighborHeight * weight;
                        weightSum += weight;
                    }
                }
            }

            float height = (heightSum + baseHeight) / (weightSum + 1.0f); // average

            // map to [-0.5, 0.5]
            float scaleCol = j / (cols - 1.f) - 0.5f;
            float scaleRow = i / (rows - 1.f) - 0.5f;
            // map to world coordinates
            vertices[i * cols + j] = {scaleCol * mapScale.x, height, scaleRow * mapScale.y};
            texCoords[i * cols + j] = {scaleCol * textureScale, scaleRow * textureScale}; // TODO: how to generate more beautiful texCoords
        }
    }
    /************************** Generate Normals ************************/
    // 1. calculate per-face normals, reserve (rows - 1) * (cols - 1) * 2 space
    std::vector< std::vector<std::pair<glm::vec3, glm::vec3>> > faceNormals = std::vector<std::vector<std::pair<glm::vec3, glm::vec3>>>(rows - 1, std::vector<std::pair<glm::vec3, glm::vec3>>(cols - 1));
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            /**
             *   0──────────────2
             *   │  upper   /   0
             *   │       /   /  │
             *   │    /   /     │
             *   │ /   /        │
             *   1   /   lower  │
             *    1─────────────2
            */
            // upper triangle
            glm::vec3 p1 = vertices[i * cols + j];
            glm::vec3 p2 = vertices[(i + 1) * cols + j];
            glm::vec3 p3 = vertices[i * cols + j + 1];
            glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            faceNormals[i][j].first = normal;
            // lower triangle
            p1 = vertices[i * cols + j + 1];
            p2 = vertices[(i + 1) * cols + j];
            p3 = vertices[(i + 1) * cols + j + 1];
            normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            faceNormals[i][j].second = normal;
        }
    }
    // 2. calculate per-vertex normals by averaging face normals
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            glm::vec3 sum = glm::vec3(0.f);
            /**
             * ┌────────────────────┐
             * │    / │    / │    / │
             * │ /    │ /    │ /    │
             * ├──────1──────2──────┤
             * │    / │ up / │    / │  ......
             * │ /    │ / lo │ /    │
             * ├──────3──────4──────┤
             * │    / │    / │    / │
             * │ /    │ /    │ /    │
             * └────────────────────┘
             *        ......
            */
            // top-left
            if (i != 0 && j != 0)
                sum += faceNormals[i - 1][j - 1].second;
            // top-right
            if (i != 0 && j != cols - 1) {
                sum += faceNormals[i - 1][j].first;
                sum += faceNormals[i - 1][j].second;
            }
            // bottom-right
            if (i != rows - 1 && j != cols - 1)
                sum += faceNormals[i][j].first;
            // bottom-left
            if (i != rows - 1 && j != 0) {
                sum += faceNormals[i][j - 1].first;
                sum += faceNormals[i][j - 1].second;
            }
            normals[i * cols + j] = glm::normalize(sum); // and do average
        }
    }
    /************************** Generate Indices ************************/
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++)
            /**
             *   0──────────────2
             *   │  upper   /   0
             *   │       /   /  │
             *   │    /   /     │
             *   │ /   /        │
             *   1   /   lower  │
             *    1─────────────2
            */
            indices[i * (cols - 1) + j] = {
                (GLuint)(i * cols + j), (GLuint)((i + 1) * cols + j), (GLuint)(i * cols + j + 1),
                (GLuint)(i * cols + j + 1), (GLuint)((i + 1) * cols + j), (GLuint)((i + 1) * cols + j + 1)
            };
    }
}
