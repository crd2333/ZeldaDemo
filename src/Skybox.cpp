#include "Skybox.h"

Skybox::Skybox(std::string path) {
    // initialize and bind VAO, VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // initialize cube map
    if (path.back() != '/') path += '/';
    std::vector<std::string> faces = {
        path + "right.jpg",
        path + "left.jpg",
        path + "top.jpg",
        path + "bottom.jpg",
        path + "front.jpg",
        path + "back.jpg",
    };
    cube_map = new CubeMap();
    cube_map->Generate(faces);
    // initialize shader
    shader = new Shader(skybox_vs, skybox_fs);
    shader->use();
    shader->setInt("skybox", 0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete cube_map;
    delete shader;
}

void Skybox::draw(const glm::mat4 &projection, const glm::mat4 &view) const {
    cube_map->Bind(0);
    shader->use();
    shader->setMat4("proj_view", projection * glm::mat4(glm::mat3(view))); // remove translation from the view matrix
    glDepthFunc(GL_LEQUAL); // 深度缓冲将会填上 1.0 值，第一次绘制完后之后将再也不会通过，因此要改为小于或等于
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);   // set back to default
}
