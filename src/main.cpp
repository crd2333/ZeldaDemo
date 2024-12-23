#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include "GameWindow.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Water.h"
#include "Skybox.h"
#include "Def.h"
#include "Player.h"
#include "Bomb.h"
#include "ExplosionEffect.h"

int main() {
    GLFWwindow* window = Create_glfw_Window();
    ImGui::InitImGui(window);

    // textures
    // ...

    // skybox
    Skybox* skybox = new Skybox("resources/textures/skybox/sun");

    // terrain
    Terrain terrain(MAP_SZIE, HEIGHT_SCALE, 2, 7);
    camera.terrain = &terrain;

    // water
    Water water(MAP_SZIE, HEIGHT_SCALE, river_of_dead_height, river_of_dead, river_of_dead_num);

    // player
    Player player(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(1.0f, 2.0f, 1.0f), &terrain);

    // sun.load(); // 这个必须在 Create_glfw_Window() 之后，不然会 segfault（但是不画太阳了，算了）

    // shaders
    Shader terrain_shader("resources/terrain.vs", "resources/terrain.fs");
    terrain_shader.use();
    terrain_shader.setInt("shadowMap", 0);
    terrain_shader.setInt("terrain_texture1", 1);
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

    while (!glfwWindowShouldClose(window)) {
        RenderLoopPreProcess(window, &player, &terrain);

        // float time = glfwGetTime();

        GLenum terrain_mode;
        const char* terrain_modes[] = { "GL_LINE_STRIP", "GL_TRIANGLES" };
        static int current_mode = 1;

        // ImGui windows
        if (mainMenu) {
            ImGui::Begin("Main Menu: left ALT to focus", &mainMenu);
            if (ImGui::Button("Quit")) { // quit button
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::SliderFloatWithDefault("Camera Speed", &camera.MovementSpeed, 0.0f, 30.0f, 20.0f);
            glm::vec3 cameraPos = camera.Position;
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
            glm::vec3 cameraFront = camera.Front;
            ImGui::Text("Camera Look At: (%.1f, %.1f, %.1f)", cameraFront.x, cameraFront.y, cameraFront.z);
            ImGui::SliderFloatWithDefault("Perspective Near", &camera.Near, 0.01f, 1.0f, 0.1f);
            ImGui::SliderFloatWithDefault("Perspective Far", &camera.Far, 100.0f, 1000.0f, 400.0f);
            ImGui::Combo("Terrain Mode", &current_mode, terrain_modes, IM_ARRAYSIZE(terrain_modes));
            ImGui::Text("Water Height: %.1f", checkHeight(player.getPosition().x, player.getPosition().z));
            ImGui::Text("Player height: %.1f", player.getPosition().y);
            // ImGui::Checkbox("Sub Menu", &SubMenu);
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
        terrain_mode = (current_mode == 0) ? GL_LINE_STRIP : GL_TRIANGLES;

        // update the camera with third person view
        camera.UpdateThirdPerson(&terrain, &player, 10.0f, 1.0f);

        // ---------- shadow pass ----------
        GLfloat orthoWidth = 150.0f;
        glm::mat4 lightProjection = camera.GetOrthoMatrix(-orthoWidth, orthoWidth, -orthoWidth, orthoWidth);
        glm::mat4 lightView = glm::lookAt(sun.LightPos, camera.Position, glm::vec3(0.0f, 1.0f, 0.0f)); // look at the camera
        glm::mat4 shadowMat = lightProjection * lightView;
        depthMapFBO.Bind();
        depthMapFBO.Clear(GL_DEPTH_BUFFER_BIT);
        shadow_shader.use();
        shadow_shader.setMat4("shadowMat", shadowMat);

        // render the terrain
        terrain.draw(shadow_shader, terrain_mode);

        // render the water
        water.dudvMap->Bind(1);
        water.normalMap->Bind(2);
        water.draw(shadow_shader, GL_TRIANGLE_FAN);

        // render the player
        player.draw(shadow_shader);

        for (auto &bomb : bombs) {
            bomb.draw(shadow_shader);
        }
        // for (auto &explosion : explosions) {
        //     explosion.draw(shadow_shader);
        // }

        depthMapFBO.UnBind();
        // ---------- shadow pass ----------

        // ---------- normal pass ----------
        // firstly, set the shared shader properties
        // set Light uniforms: lightColor, lightPos, lightAmbient, lightDiffuse, lightSpecular
        sun.SetUBO();
        // set Matrices uniforms: proj_view, shadowMat
        glm::mat4 proj_view = camera.GetPerspectiveMatrix() * camera.GetViewMatrix();
        UBO_Matrices->setData(0, sizeof(glm::mat4), glm::value_ptr(proj_view));
        UBO_Matrices->setData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shadowMat));
        // set View uniform: viewPos
        UBO_viewPos->setData(0, sizeof(glm::vec3), glm::value_ptr(camera.Position));

        water_shader.use();
        water_shader.setFloat("time", glfwGetTime());

        // render the terrain
        depthMapFBO.BindTextureBuffer(0);
        terrain.draw(terrain_shader, terrain_mode);

        // render the water
        depthMapFBO.BindTextureBuffer(0);
        water.dudvMap->Bind(1);
        water.normalMap->Bind(2);
        water.draw(water_shader, GL_TRIANGLE_FAN);

        for (auto &bomb : bombs) {
            bomb.draw(player_shader);
        }
        // for (auto &explosion : explosions) {
        //     explosion.draw(explosion_shader);
        // }

        skybox->draw(camera.GetPerspectiveMatrix(), camera.GetViewMatrix());

        // render the player
        depthMapFBO.BindTextureBuffer(0);
        glDepthMask(GL_FALSE);
        player.draw(player_shader);
        glDepthMask(GL_TRUE);

        // sun.draw(); // 这个太阳太难看了，用天空盒的太阳吧（（

        
        // ---------- normal pass ----------


        RenderLoopPostProcess(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    ImGui::EndImGui();

    glfwTerminate();
    return 0;
}
