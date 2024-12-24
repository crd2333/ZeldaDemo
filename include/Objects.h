#pragma once
#include "Def.h"
#include "Shader.h"
#include "Terrain.h"
#include "Model.h"
#include "geometry/SphereGeometry.h"

#define gravity 9.8f

class Bomb {
public:

    Shader* shader = nullptr;
    MeshVC* bomb = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    int active = 3;
    bool land = false;
    bool explode = false;
    SphereGeometry sphere;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    float life = 0.0f;

    Bomb() : shader(new Shader("resources/vertex_color.vs", "resources/vertex_color.fs")), sphere(3.5f) {
        bomb = new MeshVC("resources/model/bomb/bomb_vc.txt");
        vertices = sphere.vertices;
        indices = sphere.indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }
    ~Bomb() {
        delete shader;
        delete bomb;
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    void draw(glm::mat4 proj_view) {
        glm::mat4 model =  glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(0.5f));
        float angle = glm::radians(90.0f); // 转换为弧度
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = model * rotationMatrix;
        shader->use();
        shader->setMat4("proj_view", proj_view);
        shader->setMat4("model", model);
        bomb->draw(*shader);
    }
    void moveParabola(Terrain* terrain, float t);
    void Explode(Shader& shader, float deltaTime);
private:
};


// 定义树的基类，包括可破坏和不可破坏的树（特别大的那种）
class TreeUnbreakable {
public:
    Model* tree = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float angle = 0.0f;

    TreeUnbreakable() = default;
    virtual ~TreeUnbreakable() = default;
    void draw(Shader& shader, const int unit_offset = 0) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, scale);
        float rotation_angle = glm::radians(90.0f + angle);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = model * rotationMatrix;
        shader.use();
        shader.setMat4("model", model);
        tree->draw(shader, unit_offset);
    }
};

class TreeBreakable {
public:
    Model* tree = nullptr;
    Model *stump = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float angle = 0.0f;
    bool breakable;
    bool breaked = false;

    TreeBreakable() = default;
    virtual ~TreeBreakable() = default;
    void draw(Shader& shader, const int unit_offset = 0) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, scale);
        float rotation_angle = glm::radians(90.0f + angle);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = model * rotationMatrix;
        shader.use();
        shader.setMat4("model", model);
        if (breaked) {
            stump->draw(shader, unit_offset);
        } else {
            tree->draw(shader, unit_offset);
        }
    }
};

// 定义不同类型的树，对可以破坏的那些，另外定义一个树桩
// 白桦树
class WhiteBirch : public TreeBreakable {
public:
    WhiteBirch() {
        tree = new Model("resources/model/TreeWhiteBirch/WhiteBirch_A4.obj");
        stump = new Model("resources/model/TreeWhiteBirch/stump.obj");
        breakable = true;
        breaked = false;
    }
    ~WhiteBirch() {
        delete tree;
        delete stump;
    }

    void breakTree() {
        breaked = true;
    }
};

// 苹果树
class TreeApple : public TreeBreakable {
public:
    TreeApple() {
        tree = new Model("resources/model/TreeApple/TreeApple_L.obj");
        stump = new Model("resources/model/TreeApple/stump.obj");
        breakable = true;
        breaked = false;
    }
    ~TreeApple() {
        delete tree;
        delete stump;
    }

    void breakTree() {
        breaked = true;
    }
};

// 橡树
class BroadLeaf : public TreeUnbreakable {
public:
    BroadLeaf() {
        tree = new Model("resources/model/TreeBroadLeaf/BroadLeaf_LL.obj");
    }
    ~BroadLeaf() {
        delete tree;
    }
};


// 定义游戏中箱子的类
class BoxUnbreakable {
public:
    Model* box = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float angle = 0.0f;

    BoxUnbreakable() = default;
    virtual ~BoxUnbreakable() = default;
    void draw(Shader& shader, const int unit_offset = 0) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, scale);
        float rotation_angle = glm::radians(90.0f + angle);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = model * rotationMatrix;
        shader.use();
        shader.setMat4("model", model);
        box->draw(shader, unit_offset);
    }
};

class BoxBreakable {
public:
    Model* box = nullptr;
    Model *breaked_box = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float angle = 0.0f;
    bool breakable;
    bool breaked = false;

    BoxBreakable() = default;
    virtual ~BoxBreakable() = default;
    void draw(Shader& shader, const int unit_offset = 0) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, scale);
        float rotation_angle = glm::radians(90.0f + angle);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = model * rotationMatrix;
        shader.use();
        shader.setMat4("model", model);
        if (breaked) {
            if(breaked_box != nullptr){
                model = glm::scale(model, glm::vec3(2.0f));
                shader.setMat4("model", model);
                breaked_box->draw(shader, unit_offset);
            }
        }else{
            box->draw(shader, unit_offset);
        }
    }
};

class WoodBox : public BoxBreakable {
public:
    WoodBox() {
        box = new Model("resources/model/Box/BoxWood.obj");
        breakable = true;
        breaked = false;
    }
    ~WoodBox() {
        delete box;
    }

    void breakBox() {
        breaked = true;
    }
};

class MetalBox_breakable : public BoxBreakable {
public:
    MetalBox_breakable() {
        box = new Model("resources/model/Box/BoxIron_A.obj");
        breaked_box = new Model("resources/model/Box/BoxIron_Breakable.obj");
        breakable = true;
        breaked = false;

    }
    ~MetalBox_breakable() {
        delete box;
    }
    void breakBox() {
        breaked = true;
    }
};

class MetalBox_B : public BoxUnbreakable {
public:
    MetalBox_B() {
        box = new Model("resources/model/Box/BoxIron_B_2x2x2.obj");
    }
    ~MetalBox_B() {
        delete box;
    }
};

class MetalBox_C : public BoxUnbreakable {
public:
    MetalBox_C() {
        box = new Model("resources/model/Box/BoxIron_B_4x4x4.obj");
    }
    ~MetalBox_C() {
        delete box;
    }
};

