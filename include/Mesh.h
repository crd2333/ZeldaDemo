#pragma once

#include "Def.h"
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

struct _Vertex {
    glm::vec3 Position;  // position
    glm::vec3 Normal;    // normal
    glm::vec2 TexCoords; // texCoords
    glm::vec3 Tangent;   // tangent
    glm::vec3 Bitangent; // bitangent
    int m_BoneIDs[MAX_BONE_INFLUENCE];   // bone indexes which will influence this vertex
    float m_Weights[MAX_BONE_INFLUENCE]; // weights from each bone
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    std::vector<_Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;
    GLuint VAO;
    glm::vec3 Ka; // ambient
    glm::vec3 Kd; // diffuse
    glm::vec3 Ks; // specular
    glm::vec3 Ke; // emissive

    // constructor
    Mesh(std::vector<_Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures,
         glm::vec3 Ka=glm::vec3(0.0f), glm::vec3 Kd=glm::vec3(0.0f), glm::vec3 Ks=glm::vec3(0.0f), glm::vec3 Ke=glm::vec3(0.0f));
    // render the mesh
    void draw(Shader &shader);

private:
    // render data
    GLuint VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};

class MeshVC {
public:
    MeshVC(const std::string& filePath);
    ~MeshVC();
    void draw(Shader& shader);

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;

    void loadFromFile(const std::string& filePath);
    void setupMesh();
};
