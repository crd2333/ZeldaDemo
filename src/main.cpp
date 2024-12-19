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

int main() {
    GLFWwindow* window = Create_glfw_Window();
    ImGui::InitImGui(window);

    // textures
    // ...

    // skybox
    Skybox* skybox = new Skybox("resources/textures/skybox/sun");

    // terrain
    Terrain terrain(MAP_SZIE, HEIGHT_SCALE, RESOLUTION_RATIO, SAMPLE_NUM);
    camera.terrain = &terrain;

    // water
    Water water(MAP_SZIE, HEIGHT_SCALE, river_of_dead_height, river_of_dead, river_of_dead_num);

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
    Shader shadow_shader("resources/shadow_depth.vs", "resources/shadow_depth.fs");

    // Uniform Buffer Objects
    UBO* UBO_Matrices = new UBO(2 * sizeof(glm::mat4), 0, "Matrices");
    UBO_Matrices->Bind(terrain_shader);
    UBO_Matrices->Bind(water_shader);
    // UBO_Matrices->Bind(*sun.sun_shader); // 不画太阳了
    UBO* UBO_Light = new UBO(5 * sizeof(glm::vec4), 1, "Light");
    sun.uboLight = UBO_Light; // attach to the sun，之后直接 setUBO() 代替外面的复杂操作
    UBO_Light->Bind(terrain_shader);
    UBO_Light->Bind(water_shader);
    UBO* UBO_viewPos = new UBO(sizeof(glm::vec4), 2, "View");
    UBO_viewPos->Bind(terrain_shader);
    UBO_viewPos->Bind(water_shader);

    // frame buffer
    FrameBuffer depthMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT, FBO_DEPTH_TEXT_ONLY);

    while (!glfwWindowShouldClose(window)) {
        RenderLoopPreProcess(window);

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
            // ImGui::Checkbox("Sub Menu", &SubMenu);
            ImGui::End();
        }
        terrain_mode = (current_mode == 0) ? GL_LINE_STRIP : GL_TRIANGLES;

        // ---------- shadow pass ----------
        GLfloat orthoWidth = 300.0f;
        glm::mat4 lightProjection = camera.GetOrthoMatrix(-orthoWidth, orthoWidth, -orthoWidth, orthoWidth);
        glm::mat4 lightView = glm::lookAt(sun.LightPos, glm::vec3(camera.Position.x, 0.0f, camera.Position.z), glm::vec3(0.0f, 1.0f, 0.0f)); // look at the camera
        glm::mat4 shadowMat = lightProjection * lightView;
        depthMapFBO.Bind();
        depthMapFBO.Clear(GL_DEPTH_BUFFER_BIT);
        shadow_shader.use();
        shadow_shader.setMat4("shadowMat", shadowMat);

        terrain.draw(shadow_shader, terrain_mode);
        water.draw(shadow_shader, GL_TRIANGLE_FAN);

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

        // sun.draw(); // 这个太阳太难看了，用天空盒的太阳吧（（

        skybox->draw(camera.GetPerspectiveMatrix(), camera.GetViewMatrix());
        // ---------- normal pass ----------

        RenderLoopPostProcess(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    ImGui::EndImGui();

    glfwTerminate();
    return 0;
}
