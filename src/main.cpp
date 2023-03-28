#include "camera.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
// Include GLEW before GLFW
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
// Library for loading an image
#include <stb/stb_image.h>
#include "imgui/imgui.h"
DISABLE_WARNINGS_POP()
#include <array>
#include <framework/mesh.h>
#include <framework/shader.h>
#include <framework/window.h>
#include <iostream>
#include <span>
#include <vector>
#include <string>

#include "shapes/shapes.cpp"
#include "util/drawMesh.cpp"
#include "util/texture2D.cpp"
#include "lights/directionalLight.cpp"

// Configuration
const int WIDTH = 1080;
const int HEIGHT = 720;

const std::string WINDOW_TITLE = "Group 8";
const std::string shaderDirectory = "../shaders/";

int main()
{
    Window window{WINDOW_TITLE, glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45};

    Camera viewCamera{&window, glm::vec3(0.2f, 0.5f, 1.5f), -glm::vec3(1.2f, 1.1f, 0.9f)};
    // Camera shadowCamera{&window, glm::vec3(0.2f, 0.5f, 1.5f), -glm::vec3(1.2f, 1.1f, 0.9f)};

    const lights::DirectionalLight sunlight(glm::vec4(1.0f), glm::vec3(0.f, 1.f, 0.f));
    // const lights::DirectionalLight sunlight(glm::vec4(.9922f, .9843f, .8275f, 1.0f), glm::vec3(0.f, 1.f, 0.f));
    std::vector directionalLights = {&sunlight};

    constexpr float fov = glm::pi<float>() / 4.0f;

    // Key handle function
    window.registerKeyCallback([&](
                                   int key, int /* scancode */, int action, int /* mods */)
                               {
        switch (key) {
        case GLFW_KEY_1: {
            break;
        }
        default:
            break;
        }
    });

    const Shader mainShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, shaderDirectory + "shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, shaderDirectory + "shader_frag.glsl").build();
    const Shader peelShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, shaderDirectory + "shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, shaderDirectory + "peel_frag.glsl").build();
    const Shader shadowShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, shaderDirectory + "shader_vert.glsl").build();

    const Shader genericShader = ShaderBuilder()
        .addStage(GL_VERTEX_SHADER, shaderDirectory + "shader_vert.glsl")
        .addStage(GL_FRAGMENT_SHADER, shaderDirectory + "generic_frag.glsl")
        .build();

    int uniformCameraPos = genericShader.getUniformIndex("cameraPosition");
    int uniformNumDirLights = genericShader.getUniformIndex("directionalLights");
    int uniformDirLightDirs = genericShader.getUniformIndex("directionalLightDirections");
    int uniformDirLightsColors = genericShader.getUniformIndex("directionalLightColors");
    int uniformDiffuseCol = genericShader.getUniformIndex("diffuseColor");
    int uniformSpecCol = genericShader.getUniformIndex("specularColor");
    int uniformShine = genericShader.getUniformIndex("shininess");
    int uniformUseTex = genericShader.getUniformIndex("useTexture");
    int uniformUvTex = genericShader.getUniformIndex("uvTexture");

    // === Load a texture for exercise 5 ===
    // Create Texture
    util::Textured2D texture1("resources/smiley.png");

    // Load mesh from disk.
    Mesh plane1 = shapes::plane();
    MeshMaterial planeMaterial;
    planeMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    planeMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    planeMaterial.texture = &texture1;
    MeshDrawer plane1Drawer(&plane1, &planeMaterial);
    plane1Drawer.transformation *= glm::rotate(glm::mat4(1.0f), glm::radians(85.0f), glm::vec3(0, 0, 1));
    plane1Drawer.transformation *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0, 1.0, 1.5));

    Mesh sphere1 = shapes::uv_unit_sphere(32, 32);
    sphere1.material.kd = glm::vec3(1);
    sphere1.material.ks = glm::vec3(1);

    MeshMaterial sphereMaterial;
    sphereMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    sphereMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    MeshDrawer sphere1Drawer(&sphere1, &sphereMaterial);

    std::vector<MeshDrawer *> meshes{&sphere1Drawer, &plane1Drawer};

    // const Mesh mesh = mergeMeshes(loadMesh("resources/sceneWithBox.obj"));

    // === Create framebuffer for extra texture ===

    // Main loop
    while (!window.shouldClose())
    {
        window.updateInput();
        viewCamera.updateInput();
        auto windowSize = window.getWindowSize();

        // const glm::mat4 model { 1.0f };
        float aspectRatio = window.getAspectRatio();
        const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspectRatio, 0.1f, 30.0f);

        // === Stub code for you to fill in order to render the shadow map ===
        // {
        //     // Bind the off-screen framebuffer
        //     glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        //     // Clear the shadow map and set needed options
        //     glClearDepth(1.0f);
        //     glClear(GL_DEPTH_BUFFER_BIT);
        //     glEnable(GL_DEPTH_TEST);

        //     // Bind the shader
        //     shadowShader.bind();

        //     // Set viewport size
        //     glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

        //     const glm::mat4 shadowMvp = mainProjectionMatrix * shadowCamera.viewMatrix(); // Assume model matrix is identity.
        //     glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(shadowMvp));

        //     // const glm::vec3 lightLocation = shadowCamera.cameraPos();
        //     // glUniform3fv(1, 1, glm::value_ptr(lightLocation));

        //     // Bind vertex data
        //     for (auto meshDrawer : meshes)
        //     {
        //         meshDrawer->draw();
        //     }

        //     // Unbind the off-screen framebuffer
        //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // }

        // Clear the framebuffer to black and depth to maximum value
        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // Set viewport size
        glViewport(0, 0, windowSize.x, windowSize.y);

        // Bind the shader
        genericShader.bind();

        // Assume model matrix is identity.
        const glm::mat4 mvp = mainProjectionMatrix * viewCamera.viewMatrix(); 

        // Set view position
        const glm::vec3 cameraPos = viewCamera.cameraPos();
        glUniform3fv(uniformCameraPos, 1, glm::value_ptr(cameraPos));

        int lights = directionalLights.size();
        glm::vec3 directions[2];
        glm::vec4 colors[2];

        for (int i = 0; i < lights; i++) {
            directions[i] = directionalLights[i]->direction;
            colors[i] = directionalLights[i]->color;
        }

        glUniform1i(uniformNumDirLights, lights);
        glUniform3fv(uniformDirLightDirs, 8, glm::value_ptr(directions[0]));
        glUniform4fv(uniformDirLightsColors, 8, glm::value_ptr(colors[0]));

        for (auto meshdrawer : meshes) {
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(meshdrawer->transformation));

            // set material properties
            auto diffuse = meshdrawer->material->diffuseColor;
            auto specular = meshdrawer->material->specularColor;
            auto shine = meshdrawer->material->shininess;

            glUniform3fv(uniformDiffuseCol, 1, glm::value_ptr(diffuse));
            glUniform3fv(uniformSpecCol, 1, glm::value_ptr(specular));
            glUniform1f(uniformShine, shine);

            if (meshdrawer->material->texture != NULL) {
                glUniform1i(uniformUseTex, 1);
                meshdrawer->material->texture->bindUniform(uniformUvTex);
            } else {
                glUniform1i(uniformUseTex, 0);
            }

            meshdrawer->draw();
        }

        // ImGui::ShowMetricsWindow();
        window.swapBuffers();
        
    }

    return 0;
}
