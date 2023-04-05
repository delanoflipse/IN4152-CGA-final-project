#pragma once

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
#include "util/constants.cpp"
#include "lights/directionalLight.cpp"
#include "lights/spotLight.cpp"
#include "lights/shadowMap.cpp"
#include "entities/asteroid.cpp"
#include "entities/AsteroidManager.cpp"
#include "materials/genericMaterial.cpp"
#include "shaders/shaders.cpp"

#include "camera.h"
#include "camera.cpp"

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

    Camera viewCamera{&window, glm::vec3(0.f, .0f, .0f), glm::vec3(0.0f, 0.0f, -1.0f)};
    viewCamera.initialInput();

    lights::DirectionalLight sunlight(glm::vec4(1.0f), glm::normalize(glm::vec3(-1.f, 0.2f, 0.f)));
    std::vector directionalLights = {&sunlight};

    lights::SpotLight platformSpotlight(glm::vec4(1.0f), glm::vec3(0.01, 2, 0),  glm::vec3(-0.01, -1, 0), glm::pi<float>() / 8.0f, 4.0f);

    lights::SpotLight shipSpotlight(glm::vec4(1.0f), glm::vec3(0),  glm::vec3(0), glm::pi<float>() / 8.0f, 10.0f);

    std::vector spotLights = {&platformSpotlight, &shipSpotlight};

    std::vector<lights::Light *> lights = {&shipSpotlight, &platformSpotlight, &sunlight};

    const float pi = glm::pi<float>();
    const float pi2 = pi / 2;
    constexpr float fov = glm::pi<float>() / 4.0f;

    shaders::loadShaders();

    // === Load textures ===
    util::Textured2D rockTexture("resources/rocks.jpg");
    // util::Textured2D earthDayTexture("resources/textures/2k_earth_daymap.jpg");
    util::Textured2D earthDayTexture("resources/textures/2k_earth_daymap_with_clouds.jpg");
    util::Textured2D earthNightTexture("resources/textures/2k_earth_nightmap.jpg");
    util::Textured2D moonTexture("resources/textures/2k_moon.jpg");
    // https://www.vecteezy.com/vector-art/19783578-pattern-with-geometric-elements-in-golden-yellow-tones-abstract-gradient-background
    util::Textured2D spaceshipTexture("resources/textures/gold.jpg"); 
    // util::Textured2D skyMap("resources/textures/8k_stars.jpg");
    // util::Textured2D skyMap("resources/textures/8k_stars_milky_way.jpg");
    util::Textured2D skyMap("resources/textures/8k_stars_milky_way_darker.jpg");
    util::Textured2D toonMap("resources/textures/toon_map.png");
    // util::Textured2D sunTexture("resources/textures/2k_sun.jpg");

    // === Load meshes  ===
    Mesh asteroidMesh = mergeMeshes(loadMesh("resources/asteroid.obj"));
    Mesh sphere1 = mergeMeshes(loadMesh("resources/unit_uv_sphere.obj"));
    Mesh spaceshipMesh = mergeMeshes(loadMesh("resources/spaceship.obj"));
    Mesh platformMesh = mergeMeshes(loadMesh("resources/platform.obj"));
    // Mesh sphere1 = shapes::uv_unit_sphere(64, 64);
    // Mesh mesh = mergeMeshes(loadMesh("resources/sceneWithBox.obj"));

    materials::GenericMaterial asteroidMaterial;
    asteroidMaterial.toonTexture = &toonMap;
    asteroidMaterial.diffuseTexture = &rockTexture;
    asteroidMaterial.shininess = 64;
    // asteroidMaterial.toonUsage = 0.5f;

    materials::GenericMaterial shipMaterial;
    shipMaterial.toonTexture = &toonMap;
    shipMaterial.diffuseTexture = &spaceshipTexture;
    shipMaterial.diffuseColor = glm::vec3(0.1f, 0.1f, 0.1f);
    shipMaterial.shininess = 256;
    shipMaterial.toonUsage = 0.0f;

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

    materials::GenericMaterial platformMaterial;
    platformMaterial.diffuseColor = glm::vec4(.8f, .8f, .8f, 1.0f);
    platformMaterial.shininess = 256;

    MeshDrawer earthDrawer (&sphere1, &earthMaterial);
    MeshDrawer moonDrawer (&sphere1, &moonMaterial);
    MeshDrawer sunDrawer (&sphere1, &sunMaterial);
    MeshDrawer skyboxDrawer (&sphere1, &skyboxMaterial);

    MeshDrawer asteroidDrawer (&asteroidMesh, &asteroidMaterial);
    MeshDrawer spaceshipDrawer(&spaceshipMesh, &shipMaterial);
    MeshDrawer platformDrawer(&platformMesh, &platformMaterial);
    platformDrawer.transformation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, 0));

    // MeshDrawer debugSphereDrawer(&sphere1, &platformMaterial);
    
    entities::AsteroidManager asteroidManager;
    
    std::vector meshes {&spaceshipDrawer, &platformDrawer};

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

        shipSpotlight.direction = viewCamera.m_forward + viewCamera.m_up * 0.2f;
        shipSpotlight.position = viewCamera.m_position + viewCamera.m_forward * 0.05f;

        // const glm::mat4 model { 1.0f };
        const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspectRatio, 0.1f, 100.0f);

        // === Render shadow maps ===
        for (auto light : lights) {
            light->shadowMap.enablePass();
            // Clear the shadow map and set needed options
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Bind the shader
            shaders::shadow.shader.bind();

            // Set viewport size
            glViewport(0, 0, light->shadowMap.resolution, light->shadowMap.resolution);
            
            light->updateMvp();
            const glm::mat4 shadowMvp = light->mvp;
            glUniformMatrix4fv(shaders::shadow.vars["mvp"], 1, GL_FALSE, glm::value_ptr(shadowMvp));

            for (auto asteroid : asteroidManager.asteroids) {
                asteroidDrawer.transformation = asteroid->currentTransformation * asteroid->baseTransformation;
                asteroidDrawer.shadow();
            }

            for (auto drawer : meshes) {
                if (!drawer->castShadow) {
                    continue;
                }

                drawer->shadow();
            }
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

        int dirLightOffset = 0;
        int spotLightOffset = directionalLights.size();
        int lightCount = int(directionalLights.size()) + int(spotLights.size());

        materials::MaterialContext context{
            .mvp = &mvp,
            .cameraPosition = &cameraPos,
            .lightCount = lightCount,
        };
        
        int currentOffset = 0;
        for (int i = 0; i < directionalLights.size(); i++) {
            context.lightDirections[currentOffset] = directionalLights[i]->direction;
            context.lightColors[currentOffset] = directionalLights[i]->color;
            context.lightMvps[currentOffset] = directionalLights[i]->mvp;
            context.lightShadowMaps[currentOffset] = &(directionalLights[i]->shadowMap);
            context.lightTypes[currentOffset] = 0;
            context.lightEnabled[currentOffset] = directionalLights[i]->active;
            currentOffset++;
        }

        for (int i = 0; i < spotLights.size(); i++) {
            context.lightDirections[currentOffset] = spotLights[i]->direction;
            context.lightPositions[currentOffset] = spotLights[i]->position;
            context.lightColors[currentOffset] = spotLights[i]->color;
            context.lightMvps[currentOffset] = spotLights[i]->mvp;
            context.lightShadowMaps[currentOffset] = &(spotLights[i]->shadowMap);
            context.lightTypes[currentOffset] = 1;
            context.lightEnabled[currentOffset] = spotLights[i]->active;

            currentOffset++;
        }

        for (int i = 0; i < spotLights.size(); i++) {
            context.lightEnabled[spotLightOffset + i] = 0;
        }

        // draw earth, moon and sun
        earthDrawer.transformation = glm::rotate(glm::translate(glm::mat4(1.0f), cameraPos + glm::vec3(0, 0, -10.0f)), pi, glm::vec3(1, 0, 0));

        earthDrawer.draw(&context);

        moonDrawer.transformation =  glm::scale(glm::translate(glm::mat4(1.0f), cameraPos + glm::vec3(-1.3f, 1.8f, -10.0f)), glm::vec3(0.27));
        moonDrawer.draw(&context);

        sunDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos + sunlight.direction), glm::vec3(0.02));
        sunDrawer.draw(&context);

        skyboxDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos), glm::vec3(20.0f));
        skyboxDrawer.draw(&context);

        for (int i = 0; i < spotLights.size(); i++) {
            context.lightEnabled[spotLightOffset + i] = 1;
        }

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
        

        // render asteroids
        for (auto asteroid : asteroidManager.asteroids) {
            asteroidDrawer.transformation = asteroid->currentTransformation * asteroid->baseTransformation;
            asteroidDrawer.draw(&context);
        }

        for (auto drawer : meshes) {
            drawer->draw(&context);
        }

        // ImGui::ShowMetricsWindow();
        window.swapBuffers();
    }

    return 0;
}