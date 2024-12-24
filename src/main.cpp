#include "Def.h"
#include "GameWindow.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Water.h"
#include "Skybox.h"
#include "Player.h"
#include "Objects.h"


int main() {
    GLFWwindow* window = Create_glfw_Window();
    ImGui::InitImGui(window);

    // textures
    // ...

    // skybox
    Skybox skybox("resources/textures/skybox/sun");

    // terrain
    Terrain terrain(MAP_SZIE, HEIGHT_SCALE, RESOLUTION_RATIO, SAMPLE_NUM, SMOOTH_TIMES);
    camera.terrain = &terrain;

    // water
    Water water(MAP_SZIE, HEIGHT_SCALE, river_of_dead_height, river_of_dead, river_of_dead_num);

    // player
    Player player(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(1.0f, 2.0f, 1.0f), &terrain);
    Bomb playerBomb;

    // tree
    BroadLeaf broadLeaf[4];
    WhiteBirch whiteBirch[4];
    TreeApple treeApple[4];

    broadLeaf[0].position = glm::vec3(-35.0f, 0.0f, 21.0f);
    broadLeaf[0].position.y = terrain.getHeight(broadLeaf[0].position.x, broadLeaf[0].position.z);
    broadLeaf[0].angle = acos(terrain.getNormal(broadLeaf[0].position.x, broadLeaf[0].position.z).y) * 180.0f / glm::pi<float>();
    broadLeaf[1].position = glm::vec3(60.0f, 0.0f, 60.0f);
    broadLeaf[1].scale = glm::vec3(1.5f);
    broadLeaf[1].position.y = terrain.getHeight(broadLeaf[1].position.x, broadLeaf[1].position.z);
    broadLeaf[2].position = glm::vec3(-9.0f, 0.0f, 112.0f);
    broadLeaf[2].scale = glm::vec3(2.5f);
    broadLeaf[2].position.y = terrain.getHeight(broadLeaf[2].position.x, broadLeaf[2].position.z);
    broadLeaf[3].position = glm::vec3(28.0f, 0.0f, -83.0f);
    broadLeaf[3].scale = glm::vec3(1.5f);
    broadLeaf[3].position.y = terrain.getHeight(broadLeaf[3].position.x, broadLeaf[3].position.z);

    whiteBirch[0].position = glm::vec3(63.0f, 0.0f, -113.0f);
    whiteBirch[0].position.y = terrain.getHeight(whiteBirch[0].position.x, whiteBirch[0].position.z);
    whiteBirch[1].position = glm::vec3(133.0f, 0.0f, -7.0f);
    whiteBirch[1].position.y = terrain.getHeight(whiteBirch[1].position.x, whiteBirch[1].position.z);
    whiteBirch[2].position = glm::vec3(13.0f, 0.0f, 178.0f);
    whiteBirch[2].position.y = terrain.getHeight(whiteBirch[2].position.x, whiteBirch[2].position.z);
    whiteBirch[3].position = glm::vec3(-10.0f, 0.0f, 126.0f);
    whiteBirch[3].position.y = terrain.getHeight(whiteBirch[3].position.x, whiteBirch[3].position.z);

    treeApple[0].position = glm::vec3(-80.0f, 0.0f, 99.0f);
    treeApple[0].position.y = terrain.getHeight(treeApple[0].position.x, treeApple[0].position.z);
    treeApple[0].scale = glm::vec3(0.7f);
    treeApple[1].position = glm::vec3(-67.0f, 0.0f, -102.0f);
    treeApple[1].position.y = terrain.getHeight(treeApple[1].position.x, treeApple[1].position.z);
    treeApple[1].scale = glm::vec3(0.7f);
    treeApple[2].position = glm::vec3(68.0f, 0.0f, -71.0f);
    treeApple[2].position.y = terrain.getHeight(treeApple[2].position.x, treeApple[2].position.z);
    treeApple[2].scale = glm::vec3(0.7f);
    treeApple[3].position = glm::vec3(13.0f, 0.0f, -162.0f);
    treeApple[3].position.y = terrain.getHeight(treeApple[3].position.x, treeApple[3].position.z);
    treeApple[3].scale = glm::vec3(0.7f);
    // sun.load(); // 这个必须在 Create_glfw_Window() 之后，不然会 segfault（但是不画太阳了，算了）

    // shaders
    Shader terrain_shader("resources/terrain.vs", "resources/terrain.fs");
    terrain_shader.use();
    terrain_shader.setInt("shadowMap", 0);
    terrain_shader.setInt("terrain_texture1", 1);
    terrain_shader.setInt("terrain_texture2", 2);
    terrain_shader.setInt("terrain_texture3", 3);
    terrain_shader.setInt("terrain_texture4", 4);
    terrain_shader.setInt("normalMap1", 5);
    terrain_shader.setInt("normalMap2", 6);
    Shader water_shader("resources/water.vs", "resources/water.fs");
    water_shader.use();
    water_shader.setInt("shadowMap", 0);
    water_shader.setInt("dudvMap", 1);
    water_shader.setInt("normalMap", 2);
    water_shader.setInt("refractionMap", 3); // 折射
    water_shader.setInt("reflectionMap", 4); // 反射
    Shader player_shader("resources/player.vs", "resources/player.fs");
    player_shader.use();
    player_shader.setInt("shadowMap", 0);
    Shader shadow_shader("resources/shadow_depth.vs", "resources/shadow_depth.fs");
    Shader line_shader("resources/line.vs", "resources/line.fs");
    Shader explosion_shader("resources/explosion.vs", "resources/explosion.fs");

    // Uniform Buffer Objects
    UBO* UBO_Matrices = new UBO(2 * sizeof(glm::mat4), 0, "Matrices"); // proj_view, shadowMat
    UBO_Matrices->Bind(terrain_shader);
    UBO_Matrices->Bind(water_shader);
    UBO_Matrices->Bind(player_shader);
    UBO_Matrices->Bind(line_shader);
    UBO_Matrices->Bind(explosion_shader);
    // UBO_Matrices->Bind(*sun.sun_shader); // 不画太阳了
    UBO* UBO_Light = new UBO(5 * sizeof(glm::vec4), 1, "Light"); // lightColor, lightPos, lightAmbient, lightDiffuse, lightSpecular
    sun.uboLight = UBO_Light; // attach to the sun，之后直接 setUBO() 代替外面的复杂操作
    UBO_Light->Bind(terrain_shader);
    UBO_Light->Bind(water_shader);
    UBO_Light->Bind(player_shader);
    UBO_Light->Bind(line_shader);
    UBO_Light->Bind(explosion_shader);
    UBO* UBO_viewPos = new UBO(sizeof(glm::vec4), 2, "View"); // viewPos
    UBO_viewPos->Bind(terrain_shader);
    UBO_viewPos->Bind(water_shader);
    UBO_viewPos->Bind(player_shader);
    UBO_viewPos->Bind(line_shader);
    UBO_viewPos->Bind(explosion_shader);

    // frame buffer
    FrameBuffer depthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT, FBO_DEPTH_TEXT_ONLY);
    WaterFrameBuffer waterFBO(1024, 1024);
    water.waterFBO = &waterFBO;

    float symNear = SYM_NEAR;
    float symFar = SYM_FAR;
    float shadowNear = SHADOW_NEAR;
    float shadowFar = SHADOW_FAR;

    while (!glfwWindowShouldClose(window)) {
        RenderLoopPreProcess(window, &player, &terrain, &playerBomb, broadLeaf, whiteBirch, treeApple);
        
        // ImGui windows
        if (mainMenu) {
            ImGui::Begin("Main Menu: left ALT to focus", &mainMenu);
            if (ImGui::Button("Quit")) { // quit button
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::Text("Time: %.1f", glfwGetTime());
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::SliderFloatWithDefault("Camera Speed", &camera.MovementSpeed, 0.0f, 30.0f, 20.0f);
            glm::vec3 cameraPos = camera.Position;
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
            glm::vec3 cameraFront = camera.Front;
            ImGui::Text("Camera Look At: (%.1f, %.1f, %.1f)", cameraFront.x, cameraFront.y, cameraFront.z);
            ImGui::SliderFloatWithDefault("Perspective Near", &camera.Near, 0.01f, 1.0f, NEAR);
            ImGui::SliderFloatWithDefault("Perspective Far", &camera.Far, 100.0f, 1000.0f, FAR);
            ImGui::SliderFloatWithDefault("Symmetric Perspective Near", &symNear, 0.01f, 50.f, SYM_NEAR);
            ImGui::SliderFloatWithDefault("Symmetric Perspective Far", &symFar, 100.0f, 1000.f, SYM_FAR);
            ImGui::SliderFloatWithDefault("Shadow Near", &shadowNear, 50.0f, 400.0f, SHADOW_NEAR);
            ImGui::SliderFloatWithDefault("Shadow Far", &shadowFar, 100.0f, 1500.0f, SHADOW_FAR);
            ImGui::Text("Water Height: %.1f", checkHeight(player.getPosition().x, player.getPosition().z));
            ImGui::Text("Player height: %.1f", player.getPosition().y);
            ImGui::Separator();
            ImGui::Text("Player");
            glm::vec3 direction = player.getDirection();
            if (ImGui::InputFloat3("Direction", glm::value_ptr(direction))) {
                player.setDirection(direction);
            }
            glm::vec3 upVector = player.getUpVector();
            if (ImGui::InputFloat3("Up Vector", glm::value_ptr(upVector))) {
                player.setUpVector(upVector);
            }
            glm::vec3 length = player.getLength();
            if (ImGui::InputFloat3("Length", glm::value_ptr(length))) {
                player.setLength(length);
            }
            glm::vec3 color = player.getColor();
            float colorArray[3] = { color.r, color.g, color.b };
            if (ImGui::ColorEdit3("Color", colorArray)) {
                player.setColor(glm::vec3(colorArray[0], colorArray[1], colorArray[2]));
            }
            float speed = player.getSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 0.0f, 100.0f)) {
                player.setSpeed(speed);
            }
            float walkSpeed = player.getWalkSpeed();
            if (ImGui::SliderFloat("Walk Speed", &walkSpeed, 0.0f, 50.0f)) {
                player.setWalkSpeed(walkSpeed);
            }
            float runSpeed = player.getRunSpeed();
            if (ImGui::SliderFloat("Run Speed", &runSpeed, 0.0f, 100.0f)) {
                player.setRunSpeed(runSpeed);
            }
            float swimSpeed = player.getSwimSpeed();
            if (ImGui::SliderFloat("Swim Speed", &swimSpeed, 0.0f, 30.0f)) {
                player.setSwimSpeed(swimSpeed);
            }
            float fastSwimSpeed = player.getFastSwimSpeed();
            if (ImGui::SliderFloat("Fast Swim Speed", &fastSwimSpeed, 0.0f, 60.0f)) {
                player.setFastSwimSpeed(fastSwimSpeed);
            }
            float climbSpeed = player.getClimbSpeed();
            if (ImGui::SliderFloat("Climb Speed", &climbSpeed, 0.0f, 20.0f)) {
                player.setClimbSpeed(climbSpeed);
            }
            float jumpHorizenSpeed = player.getJumpHorizenSpeed();
            if (ImGui::SliderFloat("Jump Horizontal Speed", &jumpHorizenSpeed, 0.0f, 50.0f)) {
                player.setJumpHorizenSpeed(jumpHorizenSpeed);
            }
            float jumpUpSpeed = player.getJumpUpSpeed();
            if (ImGui::SliderFloat("Jump Up Speed", &jumpUpSpeed, 0.0f, 50.0f)) {
                player.setJumpUpSpeed(jumpUpSpeed);
            }
            float jumpHeight = player.getJumpHeight();
            if (ImGui::SliderFloat("Jump Height", &jumpHeight, 0.0f, 100.0f)) {
                player.setJumpHeight(jumpHeight);
            }
            ImGui::End();
        }

        // update the camera with third person view
        camera.UpdateThirdPerson(&terrain, &player, 10.0f, 1.0f);

        // ---------- shadow pass ----------
        // GLfloat orthoWidth = 100.0f;
        glm::mat4 lightProjection = camera.GetOrthoMatrix(100.0f, shadowNear, shadowFar);
        glm::mat4 lightView = glm::lookAt(sun.LightPos, player.getPosition(), glm::vec3(0.0f, 1.0f, 0.0f)); // look at the camera
        glm::mat4 shadowMat = lightProjection * lightView;
        depthMapFBO.Bind();
        depthMapFBO.Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), GL_DEPTH_BUFFER_BIT);
        shadow_shader.use();
        shadow_shader.setMat4("shadowMat", shadowMat);

        // render the terrain
        terrain.draw(shadow_shader);

        // render the water
        water.dudvMap->Bind(1);
        water.normalMap->Bind(2);
        water.draw(shadow_shader);

        // render the player
        player.draw(shadow_shader);

        depthMapFBO.UnBind();
        // ---------- shadow pass ----------

        // ---------- refraction pass ----------
        water.RefractionPreProcess(); // 开启裁剪平面，并且渲染到 waterFBO 上

        // firstly, set the shared shader properties
        // set Matrices uniforms: proj_view, shadowMat
        glm::mat4 projection = camera.GetPerspectiveMatrix();
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj_view = projection * view;
        UBO_Matrices->setData(0, sizeof(glm::mat4), glm::value_ptr(proj_view));
        UBO_Matrices->setData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shadowMat));
        // set View uniform: viewPos
        UBO_viewPos->setData(0, sizeof(glm::vec3), glm::value_ptr(camera.Position));
        terrain_shader.use();
        terrain_shader.setBool("doRefraction", GL_TRUE);
        terrain_shader.setFloat("waterHeight", water.getHeight());
        depthMapFBO.BindTextureBuffer(0);
        terrain.draw(terrain_shader);
        terrain_shader.setBool("doRefraction", GL_FALSE);

        water.RefractionPostProcess();
        // ---------- refraction pass ----------

        // ---------- reflection pass ----------
        water.ReflectionPreProcess();

        // firstly, set the shared shader properties
        glm::mat4 symProjection = camera.GetPerspectiveMatrix(45, SCR_SCALE, symNear, symFar);
        glm::mat4 symView = camera.GetSymmetricViewMatrix_y(water.getHeight());
        proj_view = symProjection * symView;
        UBO_Matrices->setData(0, sizeof(glm::mat4), glm::value_ptr(proj_view));

        // render the terrain
        terrain_shader.use();
        terrain_shader.setBool("doReflection", GL_TRUE);
        terrain.draw(terrain_shader);
        terrain_shader.setBool("doReflection", GL_FALSE);

        // 其它水上物体的渲染
        // ...

        // render the skybox
        skybox.draw(symProjection, symView);

        water.ReflectionPostProcess();
        // ---------- reflection pass ----------

        // ---------- normal pass ----------
        // firstly, set the shared shader properties
        // set Light uniforms: lightColor, lightPos, lightAmbient, lightDiffuse, lightSpecular
        sun.SetUBO();
        // set Matrices uniforms: proj_view, shadowMat
        projection = camera.GetPerspectiveMatrix();
        proj_view = projection * view;
        UBO_Matrices->setData(0, sizeof(glm::mat4), glm::value_ptr(proj_view));

        // render the terrain
        depthMapFBO.BindTextureBuffer(0);
        terrain.draw(terrain_shader);

        // render the water
        depthMapFBO.BindTextureBuffer(0);
        water.dudvMap->Bind(1);
        water.normalMap->Bind(2);
        water.waterFBO->BindTextureBuffer(3); // 绑定 refractionMap 到纹理单元 3
        water.waterFBO->BindTextureBuffer2(4); // 绑定 reflectionMap 到纹理单元 4
        water.draw(water_shader, GL_TRIANGLE_FAN);

        if (playerBomb.active == 1 || playerBomb.active == 2) {
            playerBomb.draw(proj_view);
        }

        // render the trees
        for (int i = 0; i < 4; i++) {
            broadLeaf[i].draw(proj_view);
            whiteBirch[i].draw(proj_view);
            treeApple[i].draw(proj_view);
        }

        // sun.draw(); // 这个太阳太难看了，用天空盒的太阳吧（（

        // render the skybox
        skybox.draw(projection, view);
        // render the player
        depthMapFBO.BindTextureBuffer(0);
        glDepthMask(GL_FALSE);
        if (playerBomb.explode) {
            playerBomb.Explode(player_shader, deltaTime);
        }
        player.draw(player_shader);
        glDepthMask(GL_TRUE);

        // ---------- normal pass ----------

        RenderLoopPostProcess(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    ImGui::EndImGui();

    glfwTerminate();
    return 0;
}
