#include "Mesh.h"

// ---------- class Mesh ----------
// constructor
Mesh::Mesh(std::vector<_Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, glm::vec3 Ke)
    : vertices(vertices), indices(indices), textures(textures), Ka(Ka), Kd(Kd), Ks(Ks), Ke(Ke) {
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

// render the mesh
void Mesh::draw(Shader &shader, const int unit_offset) {
    shader.use();
    // active proper texture unit before binding
    glUniform3fv(glGetUniformLocation(shader.ID, "Ka"), 1, glm::value_ptr(Ka));
    glUniform3fv(glGetUniformLocation(shader.ID, "Kd"), 1, glm::value_ptr(Kd));
    glUniform3fv(glGetUniformLocation(shader.ID, "Ks"), 1, glm::value_ptr(Ks));
    glUniform3fv(glGetUniformLocation(shader.ID, "Ke"), 1, glm::value_ptr(Ke));

    // bind appropriate textures
    GLuint diffuseNr  = 1;
    GLuint specularNr = 1;
    GLuint normalNr   = 1;
    GLuint heightNr   = 1;
    GLuint transNr    = 1;

    // first set uniform bool useTransTexture to false, avoid using the texture set previously
    glUniform1i(glGetUniformLocation(shader.ID, "useTransTexture"), GL_FALSE);
    for (GLuint i = 0; i < textures.size(); i++) { // 最后一个是默认的白色纹理
        glActiveTexture(GL_TEXTURE0 + i + unit_offset); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer GLuint to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "texture_height")
            number = std::to_string(heightNr++);
        else if (name == "texture_trans") {
            glUniform1i(glGetUniformLocation(shader.ID, "useTransTexture"), GL_TRUE);
            number = std::to_string(transNr++);
        }

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i + unit_offset);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLuint>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(_Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)offsetof(_Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)offsetof(_Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)offsetof(_Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)offsetof(_Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(_Vertex), (void*)offsetof(_Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)offsetof(_Vertex, m_Weights));
    glBindVertexArray(0);
}
// ---------- class Mesh ----------


// ---------- class MeshVC ----------
MeshVC::MeshVC(const std::string& filePath) {
    loadFromFile(filePath);
    setupMesh();
}

MeshVC::~MeshVC() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void MeshVC::draw(Shader& shader) {
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void MeshVC::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        if (line[0] == 'f') {
            unsigned int vertexIndex[3];
            ss >> prefix >> vertexIndex[0] >> vertexIndex[1] >> vertexIndex[2];
            indices.push_back(vertexIndex[0] - 1);
            indices.push_back(vertexIndex[1] - 1);
            indices.push_back(vertexIndex[2] - 1);
        } else if (line[0] == 'v') {
            float x, y, z, r, g, b;
            ss >> prefix >> x >> y >> z >> r >> g >> b;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
        }
    }
}

void MeshVC::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Vertex colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
// ---------- class MeshVC ----------
