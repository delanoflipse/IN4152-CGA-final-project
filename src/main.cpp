#include "camera.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
// Include GLEW before GLFW
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include "glm/gtx/string_cast.hpp"
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
#include "util/clock.cpp"
#include "lights/directionalLight.cpp"
#include "lights/shadowMap.cpp"
#include "entities/asteroid.cpp"
#include "entities/AsteroidManager.cpp"
#include "materials/genericMaterial.cpp"
#include "shaders/shaders.cpp"

// Configuration
const int WIDTH = 1080;
const int HEIGHT = 720;

const std::string WINDOW_TITLE = "Group 8";

int main()
{
    Window window{WINDOW_TITLE, glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45};
    window.setMouseCapture(true);

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

    Camera viewCamera{&window, glm::vec3(0.20f, 2.0f, 9.0f), glm::vec3(0.0f, 0.0f, -1.0f)};
    lights::DirectionalLight sunlight(glm::vec4(1.0f), glm::normalize(glm::vec3(-1.f, 0.2f, 0.f)));
    // lights::DirectionalLight moonlight(glm::vec4(1.0f), glm::vec3(0.f, 1.f, 0.f));
    // const lights::DirectionalLight sunlight(glm::vec4(.9922f, .9843f, .8275f, 1.0f), glm::vec3(0.f, 1.f, 0.f));
    std::vector directionalLights = {&sunlight};

    constexpr float fov = glm::pi<float>() / 4.0f;

    shaders::loadShaders();

    // === Load textures ===
    util::Textured2D texture1("resources/smiley.png");
    util::Textured2D rockTexture("resources/rocks.jpg");
    // util::Textured2D earthDayTexture("resources/textures/2k_earth_daymap.jpg");
    util::Textured2D earthDayTexture("resources/textures/2k_earth_daymap_with_clouds.jpg");
    util::Textured2D earthNightTexture("resources/textures/2k_earth_nightmap.jpg");
    util::Textured2D moonTexture("resources/textures/2k_moon.jpg");
    // util::Textured2D skyMap("resources/textures/8k_stars.jpg");
    // util::Textured2D skyMap("resources/textures/8k_stars_milky_way.jpg");
    util::Textured2D skyMap("resources/textures/8k_stars_milky_way_darker.jpg");
    util::Textured2D toonMap("resources/textures/toon_map.png");
    // util::Textured2D sunTexture("resources/textures/2k_sun.jpg");

    // === Load meshes  ===
    Mesh asteroidMesh = mergeMeshes(loadMesh("resources/asteroid.obj"));
    Mesh sphere1 = mergeMeshes(loadMesh("resources/unit_uv_sphere.obj"));
    Mesh spaceshipMesh = mergeMeshes(loadMesh("resources/spaceship.obj"));
    // Mesh sphere1 = shapes::uv_unit_sphere(64, 64);
    // Mesh mesh = mergeMeshes(loadMesh("resources/sceneWithBox.obj"));

    materials::GenericMaterial asteroidMaterial;
    asteroidMaterial.toonTexture = &toonMap;
    asteroidMaterial.diffuseTexture = &rockTexture;
    asteroidMaterial.shininess = 64;
    // asteroidMaterial.toonUsage = 0.5f;

    materials::GenericMaterial shipMaterial;
    shipMaterial.toonTexture = &toonMap;
    shipMaterial.diffuseTexture = &rockTexture;
    shipMaterial.shininess = 64;
    // asteroidMaterial.toonUsage = 0.5f;

    materials::GenericMaterial earthMaterial;
    earthMaterial.diffuseTexture = &earthDayTexture;
    earthMaterial.shadowTexture = &earthNightTexture;
    earthMaterial.shininess = 0;
    earthMaterial.useShadows = false;

    materials::GenericMaterial moonMaterial;
    moonMaterial.diffuseTexture = &moonTexture;
    moonMaterial.shininess = 0;
    moonMaterial.useShadows = false;

    materials::GenericMaterial sunMaterial;
    sunMaterial.diffuseColor = glm::vec4(.9922f, .9843f, .8275f, 1.0f);
    sunMaterial.shininess = 0;
    sunMaterial.useLights = false;
    sunMaterial.useShadows = false;

    materials::GenericMaterial skyboxMaterial;
    skyboxMaterial.diffuseTexture = &skyMap;
    skyboxMaterial.shininess = 0;
    skyboxMaterial.useLights = false;
    skyboxMaterial.useShadows = false;

    MeshDrawer earthDrawer (&sphere1, &earthMaterial);
    MeshDrawer moonDrawer (&sphere1, &moonMaterial);
    MeshDrawer sunDrawer (&sphere1, &sunMaterial);
    MeshDrawer skyboxDrawer (&sphere1, &skyboxMaterial);

    MeshDrawer asteroidDrawer (&asteroidMesh, &asteroidMaterial);
    MeshDrawer spaceshipDrawer(&spaceshipMesh, &shipMaterial);

    entities::AsteroidManager asteroidManager;

    glm::mat4 shipRotationMatrix{0};

    timing::start();

    // Main loop
    while (!window.shouldClose())
    {
        window.updateInput();
        viewCamera.updateInput();
        timing::update();

        asteroidManager.update();

        glm::ivec2 windowSize = window.getWindowSize();
        float aspectRatio = window.getAspectRatio();

        // const glm::mat4 model { 1.0f };
        const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspectRatio, 0.1f, 100.0f);

        // === Render shadow maps ===
        for (auto dirLight : directionalLights) {
            dirLight->shadowMap.enablePass();
            // Clear the shadow map and set needed options
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Bind the shader
            shaders::shadow.shader.bind();

            // Set viewport size
            glViewport(0, 0, dirLight->shadowMap.resolution, dirLight->shadowMap.resolution);

            const glm::mat4 shadowMvp = dirLight->mvp;
            glUniformMatrix4fv(shaders::shadow.vars["mvp"], 1, GL_FALSE, glm::value_ptr(shadowMvp));

            for (auto asteroid : asteroidManager.asteroids) {
                asteroidDrawer.transformation = asteroid->currentTransformation * asteroid->baseTransformation;
                asteroidDrawer.shadow();
            }
            spaceshipDrawer.shadow();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        // window.swapBuffers();
        // continue;

        // === Normal render ===
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, windowSize.x, windowSize.y);

        glm::mat4 mvp = mainProjectionMatrix * viewCamera.viewMatrix(); 

        glm::vec3 cameraPos = viewCamera.cameraPos();

        int lights = directionalLights.size();
        glm::vec3 directions[2];
        glm::vec4 colors[2];
        glm::mat4 mvps[2];
        lights::ShadowMap * shadows[2];

        for (int i = 0; i < lights; i++) {
            directions[i] = directionalLights[i]->direction;
            colors[i] = directionalLights[i]->color;
            mvps[i] = directionalLights[i]->mvp;
            shadows[i] = &(directionalLights[i]->shadowMap);
        }

        materials::MaterialContext context {
            .mvp = &mvp,
            .cameraPosition = &cameraPos,
            .directionalLights = lights,
            .directionLightDirections = directions,
            .directionLightColors = colors,
            .directionLightMvps = mvps,
            .directionLightShadows = shadows,
        };

        // draw earth, moon and sun
        earthDrawer.transformation = glm::translate(glm::mat4(1.0f), cameraPos + glm::vec3(0, 0, -10.0f));

        earthDrawer.draw(&context);

        moonDrawer.transformation =  glm::scale(glm::translate(glm::mat4(1.0f), cameraPos + glm::vec3(-1.3f, 1.8f, -10.0f)), glm::vec3(0.27));
        moonDrawer.draw(&context);

        sunDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos + sunlight.direction), glm::vec3(0.02));
        sunDrawer.draw(&context);

        skyboxDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos), glm::vec3(20.0f));
        skyboxDrawer.draw(&context);

        // clear depth from skybox drawing
        glClear(GL_DEPTH_BUFFER_BIT);


        // render ship
        glm::mat4x4 shipTransform = glm::mat4(1.0f);

        shipTransform = glm::translate(shipTransform, viewCamera.m_position - viewCamera.m_up);
        shipTransform = glm::rotate(shipTransform, glm::half_pi<float>(), viewCamera.m_up);

        glm::vec3 f = viewCamera.m_forward;
        glm::vec3 u = viewCamera.m_up;
        glm::vec3 r = viewCamera.m_right;
        glm::mat4 rotation{ r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0, 0, 0, 0, 1 };
        shipTransform *= rotation;
        
        spaceshipDrawer.transformation = shipTransform;
        spaceshipDrawer.draw(&context);

        // render asteroids
        for (auto asteroid : asteroidManager.asteroids) {
            asteroidDrawer.transformation = asteroid->currentTransformation * asteroid->baseTransformation;
            asteroidDrawer.draw(&context);
        }

        // ImGui::ShowMetricsWindow();
        window.swapBuffers();
    }

    return 0;
}