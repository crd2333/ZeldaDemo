#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "GameWindow.h"
#include "UBO.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"
#include "Def.h"

int main() {
    GLFWwindow* window = Create_glfw_Window();
    ImGui::InitImGui(window);

    // textures
    // ...

    // skybox
    // Skybox* skybox = new Skybox("resources/textures/skybox");

    // terrain
    Terrain terrain(glm::vec2(250.0f), 300.0f, 2, 3);
    camera.terrain = &terrain;

    // shaders
    Shader terrain_shader("resources/terrain.vs", "resources/terrain.fs");
    Shader player_shader("resources/player.vs", "resources/player.fs");
    terrain_shader.use();
    terrain_shader.setInt("terrain_texture1", 0);

    while (!glfwWindowShouldClose(window)) {
        RenderLoopPreProcess(window);

        GLenum terrain_mode;
        const char* terrain_modes[] = { "GL_LINE_STRIP", "GL_TRIANGLES" };
        static int current_mode = 1;

        // todo: ImGui windows
        if (mainMenu) {
            ImGui::Begin("Main Menu: left ALT to focus", &mainMenu);
            if (ImGui::Button("Quit")) { // quit button
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::SliderFloat("Camera Speed", &speedUp, 0.0f, 10.0f);
            glm::vec3 cameraPos = camera.Position;
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
            glm::vec3 cameraFront = camera.Front;
            ImGui::Text("Camera Look At: (%.1f, %.1f, %.1f)", cameraFront.x, cameraFront.y, cameraFront.z);
            ImGui::ColorEdit3("Clear Color", clearColor);
            ImGui::Combo("Terrain Mode", &current_mode, terrain_modes, IM_ARRAYSIZE(terrain_modes));
            // ImGui::Checkbox("Sub Menu", &SubMenu);
            ImGui::End();
        }
        terrain_mode = (current_mode == 0) ? GL_LINE_STRIP : GL_TRIANGLES;

        // set the shared shader properties todo: move to UBO
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        terrain_shader.use();
        terrain_shader.setMat4("view", view);
        terrain_shader.setMat4("projection", projection);

        float time = glfwGetTime();

        // render the terrain
        terrain.draw(terrain_shader, terrain_mode);
        // terrain.draw(terrain_shader);

        // skybox->draw(view, projection);

        RenderLoopPostProcess(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    ImGui::EndImGui();

    glfwTerminate();
    return 0;
}
