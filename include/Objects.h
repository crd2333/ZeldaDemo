#include "Def.h"
#include "Shader.h"
#include "Model.h"

class Bomb {
public:
    Shader* shader = nullptr;
    MeshVC* bomb = nullptr;
    glm::vec3 position = glm::vec3(0.0f);

    Bomb() : shader(new Shader("resources/vertex_color.vs", "resources/vertex_color.fs")) {
        bomb = new MeshVC("resources/model/bomb/bomb_vc.txt");
    }
    ~Bomb() {
        delete shader;
        delete bomb;
    }
    void draw() {
        shader->use();
        shader->setMat4("model", glm::translate(glm::mat4(1.0f), position));
        bomb->draw(*shader);
    }

private:
    // 根据时间 t 在抛物线上移动（调整 position），直到碰撞
    void moveParabola(float t); // TODO
};

// 定义树的基类，包括可破坏和不可破坏的树（特别大的那种）
class TreeUnbreakable {
public:
    Shader* shader = nullptr;
    Model* tree = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    TreeUnbreakable() : shader(new Shader("resources/model.vs", "resources/model.fs")) {}
    virtual ~TreeUnbreakable() {
        delete shader;
    }
};

class TreeBreakable {
public:
    Shader* shader = nullptr;
    Model* tree = nullptr;
    Model *stump = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool breakable;
    bool breaked;

    TreeBreakable() : shader(new Shader("resources/model.vs", "resources/model.fs")) {}
    virtual ~TreeBreakable() {
        delete shader;
    }
};

// 定义不同类型的树，对可以破坏的那些，另外定义一个树桩
// 白桦树
class WhiteBirch : public TreeBreakable {
public:
    WhiteBirch() {
        tree = new Model("resources/model/TreeWhiteBirch/WhiteBirch_A1.obj");
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
        tree = new Model("resources/model/BroadLeaf/BroadLeaf_LL.obj");
    }
    ~BroadLeaf() {
        delete tree;
    }
};
