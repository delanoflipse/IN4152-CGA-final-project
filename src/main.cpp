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
DISABLE_WARNINGS_POP()
#include <array>
#include <framework/mesh.h>
#include <framework/shader.h>
#include <framework/window.h>
#include <iostream>
#include <span>
#include <vector>
#include <string>

#include "shapes/uv_sphere.cpp"
#include "util/drawMesh.cpp"
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

    const lights::DirectionalLight sunlight(glm::vec4(.9922f, .9843f, .8275f, 1.0f), glm::vec3(0.f, 1.f, 0.f));
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
    int genericDirLightDirection = glGetUniformLocation(genericShader.m_program, "directionalLightDirections");
    int genericDirLightColor = glGetUniformLocation(genericShader.m_program, "directionalLightColors");

    // === Load a texture for exercise 5 ===
    // Create Texture
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load("resources/smiley.png", &texWidth, &texHeight, &texChannels, 3);

    GLuint texLight;
    glCreateTextures(GL_TEXTURE_2D, 1, &texLight);
    glTextureStorage2D(texLight, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(texLight, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texLight, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texLight, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texLight, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texLight, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load mesh from disk.
    Mesh sphere1 = shapes::uv_unit_sphere(32, 32);
    sphere1.material.kd = glm::vec3(1);
    sphere1.material.ks = glm::vec3(1);
    MeshDrawer sphere1Drawer(&sphere1);

    std::vector<MeshDrawer *> meshes{&sphere1Drawer};

    // const Mesh mesh = mergeMeshes(loadMesh("resources/sceneWithBox.obj"));

    // === Create Shadow Texture ===
    GLuint texShadow;
    const int SHADOWTEX_WIDTH = 1024;
    const int SHADOWTEX_HEIGHT = 1024;
    glCreateTextures(GL_TEXTURE_2D, 1, &texShadow);
    glTextureStorage2D(texShadow, 1, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texShadow, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texShadow, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texShadow, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texShadow, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // === Create framebuffer for extra texture ===
    GLuint framebuffer;
    glCreateFramebuffers(1, &framebuffer);
    glNamedFramebufferTexture(framebuffer, GL_DEPTH_ATTACHMENT, texShadow, 0);

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

        const glm::mat4 mvp = mainProjectionMatrix * viewCamera.viewMatrix(); // Assume model matrix is identity.
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

        // Set view position
        const glm::vec3 cameraPos = viewCamera.cameraPos();
        glUniform3fv(1, 1, glm::value_ptr(cameraPos));

        int lights = directionalLights.size();
        glm::vec3 directions[8];
        glm::vec4 colors[8];

        for (int i = 0; i < lights; i++) {
            directions[i] = directionalLights[i]->direction;
            colors[i] = directionalLights[i]->color;
        }

        glUniform1i(4, lights);
        glUniform3fv(genericDirLightDirection, 8, glm::value_ptr(directions[0]));
        glUniform4fv(genericDirLightColor, 8, glm::value_ptr(colors[0]));

        for (auto meshdrawer : meshes) {
            // set material properties
            auto ks = meshdrawer->workingMesh->material.ks;
            auto shine = meshdrawer->workingMesh->material.shininess;

            glUniform3fv(2, 1, glm::value_ptr(ks));
            glUniform1f(3, shine);

            meshdrawer->draw();
        }

        // Present result to the screen.
        window.swapBuffers();
    }

    // Be a nice citizen and clean up after yourself.
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texShadow);
    glDeleteTextures(1, &texLight);

    return 0;
}
