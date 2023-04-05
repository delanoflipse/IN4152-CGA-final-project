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
#include "util/intersection.cpp"
#include "gamestate.cpp"
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
const int WIDTH = 1920;
const int HEIGHT = 1080;

const int FRAMES = 200;
const int SKIP_FRAMES = 8;
const int ANIMATION_FPS = 30;
const float ANIMATION_DURATION = 0.833f;
const float SHOOT_COOLDOWN = 0.8f;

const float powerUpTransitionSpeed = 1.25f;
const float powerUpDuration = 5.0f;

const std::string WINDOW_TITLE = "Group 8";

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

int main()
{
    Window window{WINDOW_TITLE, glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45};

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
        } });

    Camera viewCamera{&window, glm::vec3(0.f, .0f, .0f), glm::vec3(0.0f, 0.0f, -1.0f)};

    lights::DirectionalLight sunlight(glm::vec4(1.0f), glm::normalize(glm::vec3(-1.f, 0.2f, 0.f)));
    // lights::DirectionalLight earthLight(glm::vec4(0.47451f, 0.52549f, 0.61176f, 1.0f), glm::normalize(glm::vec3(0.f, 0.f, -1.f)));
    std::vector directionalLights = {&sunlight};

    float platformScale = 2.0f;

    lights::SpotLight platformSpotlight1(
        glm::vec4(0.2f, 0.2f, 1.0f, 1.0f),
        platformScale * glm::vec3(-0.8575f, 0.9289f, 0.0f),
        glm::vec3(0.8575f, -0.9289f, 0.0f),
        glm::pi<float>() / 4.0f, 8.0f);

    lights::SpotLight platformSpotlight2(
        glm::vec4(0.2f, 1.0f, 0.2f, 1.0f),
        platformScale * glm::vec3(0.4284f, 0.9289f, -0.7424f),
        glm::vec3(-0.4284f, -0.9289f, 0.7424f),
        glm::pi<float>() / 4.0f, 8.0f);

    lights::SpotLight platformSpotlight3(
        glm::vec4(1.0f, 0.2f, 0.2f, 1.0f),
        platformScale * glm::vec3(0.4284f, 0.9289f, 0.7424f),
        glm::vec3(-0.4284f, -0.9289f, -0.7424f),
        glm::pi<float>() / 4.0f, 8.0f);

    lights::SpotLight shipSpotlight(glm::vec4(1.0f), glm::vec3(0), glm::vec3(0), glm::pi<float>() / 8.0f, 32.0f);

    std::vector spotLights = {&platformSpotlight1, &platformSpotlight2, &platformSpotlight3, &shipSpotlight};
    std::vector rechargeLights = {&platformSpotlight1, &platformSpotlight2, &platformSpotlight3};

    std::vector<lights::Light *> lights = {&shipSpotlight, &platformSpotlight1, &platformSpotlight2, &platformSpotlight3, &sunlight};

    const float pi = glm::pi<float>();
    const float pi2 = pi / 2;
    constexpr float fov = glm::pi<float>() / 4.0f;

    shaders::loadShaders();

    // === Load textures ===
    util::Textured2D rockTexture("resources/textures/rocks.jpg", true);
    // util::Textured2D earthDayTexture("resources/textures/2k_earth_daymap.jpg");
    util::Textured2D earthDayTexture("resources/textures/2k_earth_daymap_with_clouds.jpg", true);
    util::Textured2D earthNightTexture("resources/textures/2k_earth_nightmap.jpg", true);
    util::Textured2D moonTexture("resources/textures/2k_moon.jpg", true);
    util::Textured2D earthNormalTexture("resources/textures/2k_earth_normal_map.png");
    util::Textured2D earthSpecularTexture("resources/textures/2k_earth_specular_map_2.jpg");
    // https://www.vecteezy.com/vector-art/19783578-pattern-with-geometric-elements-in-golden-yellow-tones-abstract-gradient-background
    util::Textured2D spaceshipTexture("resources/textures/gold.jpg", true);
    // util::Textured2D skyMap("resources/textures/8k_stars.jpg");
    // util::Textured2D skyMap("resources/textures/8k_stars_milky_way.jpg");
    util::Textured2D skyMap("resources/textures/8k_stars_milky_way_darker.jpg", true);

    util::Textured2D toonMapShip("resources/textures/toon_map_ship.png");
    util::Textured2D toonMapEarth("resources/textures/toon_map_earth.png");
    util::Textured2D toonMapAsteroid("resources/textures/toon_map_asteroid.png");
    util::Textured2D toonMapPlatform("resources/textures/toon_map_platform.png");

    util::Textured2D metalNormalMap("resources/textures/metal_norm.jpg");
    util::Textured2D metalSpecularMap("resources/textures/metal_spec.jpg");
    util::Textured2D metalDiffuseMap("resources/textures/metal_tex.jpg", true);

    // === Load meshes  ===
    Mesh asteroidMesh = mergeMeshes(loadMesh("resources/models/asteroid.obj"));
    Mesh sphere1 = mergeMeshes(loadMesh("resources/models/unit_uv_sphere.obj"));
    Mesh spaceshipMesh = mergeMeshes(loadMesh("resources/models/spaceship.obj"));
    Mesh platformMesh = mergeMeshes(loadMesh("resources/models/platform.obj"));
    Mesh houseMesh = mergeMeshes(loadMesh("resources/models/house.obj"));
    Mesh spaceshipAnimation[FRAMES];

    for (int i = 0; i < FRAMES; i += SKIP_FRAMES)
    {
        spaceshipAnimation[i] = mergeMeshes(loadMesh("resources/spaceship_animation/spaceshipanim" + std::to_string(i + 1) + ".obj"));
    }

    // Mesh sphere1 = shapes::uv_unit_sphere(64, 64);
    // Mesh mesh = mergeMeshes(loadMesh("resources/sceneWithBox.obj"));

    materials::GenericMaterial asteroidMaterial;
    asteroidMaterial.toonTexture = &toonMapAsteroid;
    asteroidMaterial.diffuseTexture = &rockTexture;
    asteroidMaterial.shininess = 64;

    materials::GenericMaterial shipMaterial;
    shipMaterial.toonTexture = &toonMapShip;
    shipMaterial.diffuseTexture = &spaceshipTexture;
    shipMaterial.diffuseColor = glm::vec3(0.1f, 0.1f, 0.1f);
    shipMaterial.shininess = 256;
    shipMaterial.toonUsage = 0.0f;

    materials::GenericMaterial earthMaterial;
    earthMaterial.toonTexture = &toonMapEarth;
    earthMaterial.diffuseTexture = &earthDayTexture;
    earthMaterial.shadowTexture = &earthNightTexture;
    earthMaterial.normalTexture = &earthNormalTexture;
    earthMaterial.specularTexture = &earthSpecularTexture;
    earthMaterial.shininess = 8.f;
    earthMaterial.useShadows = false;

    materials::GenericMaterial moonMaterial;
    moonMaterial.toonTexture = &toonMapAsteroid;
    moonMaterial.shininess = 2.0f;
    moonMaterial.ambient = 0.0f;
    moonMaterial.specularTexture = &moonTexture;
    moonMaterial.diffuseTexture = &moonTexture;
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
    platformMaterial.toonTexture = &toonMapPlatform;
    platformMaterial.diffuseTexture = &metalDiffuseMap;
    platformMaterial.normalTexture = &metalNormalMap;
    platformMaterial.specularTexture = &metalSpecularMap;
    platformMaterial.shininess = 256;

    MeshDrawer earthDrawer(&sphere1, &earthMaterial);
    MeshDrawer moonDrawer(&sphere1, &moonMaterial);
    MeshDrawer sunDrawer(&sphere1, &sunMaterial);
    MeshDrawer skyboxDrawer(&sphere1, &skyboxMaterial);

    MeshDrawer asteroidDrawer(&asteroidMesh, &asteroidMaterial);
    MeshDrawer asteroidEasterEggDrawer(&houseMesh, &asteroidMaterial);
    MeshDrawer platformDrawer(&platformMesh, &platformMaterial);
    platformDrawer.transformation = glm::scale(
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, -2.0f, 0.0f)),
        glm::vec3(platformScale));

    MeshDrawer *spaceshipAnimationDrawers[FRAMES];

    for (int i = 0; i < FRAMES; i += SKIP_FRAMES)
    {
        spaceshipAnimationDrawers[i] = new MeshDrawer(&spaceshipAnimation[i], &shipMaterial);
    }

    entities::AsteroidManager asteroidManager;
    std::vector meshes{spaceshipAnimationDrawers[0], &platformDrawer};

    timing::start();
    int updateFrame = 0, animationFrame = 0;

    // Main loop
    while (!window.shouldClose())
    {
        window.updateInput();
        viewCamera.updateInput();

        timing::update();

        float powerUpTimeLeft = gamestate::powerUpActiveUntil - timing::time_s;
        bool poweredUp = powerUpTimeLeft > 0.0f;

        if (!poweredUp && !gamestate::paused)
        {
            for (auto recharger : rechargeLights)
            {
                glm::vec3 inlightPos = math::vec4toVec3(recharger->mvp * glm::vec4(viewCamera.m_position, 1.0f));
                if (inlightPos.x > -1 && inlightPos.x < 1 && inlightPos.y > -1 && inlightPos.y < 1 && inlightPos.z < 1 && inlightPos.z > -1)
                {
                    gamestate::powerUpAvailable = true;
                    break;
                }
            }
        }

        if (!poweredUp && !gamestate::paused && gamestate::powerUpAvailable && window.isKeyPressed(GLFW_KEY_E))
        {
            poweredUp = true;
            gamestate::powerUpAvailable = false;
            gamestate::powerUpActiveUntil = timing::time_s + powerUpDuration;
        }

        float powerUpDelta = poweredUp ? 1.0f : -1.0f;
        gamestate::powerUp = glm::clamp(gamestate::powerUp + timing::delta_s * powerUpDelta * powerUpTransitionSpeed, 0.0f, 1.0f);

        platformMaterial.toonUsage = gamestate::powerUp;
        shipMaterial.toonUsage = gamestate::powerUp;
        asteroidMaterial.toonUsage = gamestate::powerUp;
        moonMaterial.toonUsage = gamestate::powerUp;
        earthMaterial.toonUsage = gamestate::powerUp;

        if (!gamestate::paused)
        {
            asteroidManager.update(viewCamera.m_position);
        }

        float shootCooldown = gamestate::shootCooldown - timing::time_s;
        bool canshoot = shootCooldown < 0.0f;
        float shootCooldownTime = SHOOT_COOLDOWN * (1.0f - gamestate::powerUp * 0.5f);
        bool showShoot = shootCooldown / shootCooldownTime > 0.25f;

        bool shooting = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        if (shooting && canshoot && !gamestate::paused)
        {
            asteroidManager.shootAt(viewCamera.m_position, viewCamera.m_forward);
            gamestate::shootCooldown = timing::time_s + shootCooldownTime;
        }

        bool hitByAsteroid = asteroidManager.hitBy(viewCamera.m_position, 1.0f);
        if (hitByAsteroid)
        {
            viewCamera.reset();
            gamestate::hits++;
        }

        // float sunRotation = timing::time_s * 0.05f;
        float sunRotation = 0.0f;
        glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), sunRotation, glm::vec3(0.0, 0.0, 1.0));
        glm::vec4 baseDirection = glm::vec4(-1.f, 0.2f, 0.f, 1.0);
        glm::vec4 rotated = rotationMat * baseDirection;
        glm::vec3 normalRotated = glm::normalize(glm::vec3(rotated.x, rotated.y, rotated.z) / rotated.w);
        sunlight.direction = normalRotated;

        float animationProgress = 1.0f - gamestate::powerUp;
        int animationFrame = SKIP_FRAMES * int(animationProgress * (FRAMES - 1) / SKIP_FRAMES);
        meshes[0] = spaceshipAnimationDrawers[animationFrame];

        glm::ivec2 windowSize = window.getWindowSize();
        float aspectRatio = window.getAspectRatio();

        shipSpotlight.direction = viewCamera.m_forward;
        shipSpotlight.position = viewCamera.m_position + viewCamera.m_forward * 0.05f;

        // render ship
        glm::mat4x4 shipTransform = glm::mat4(1.0f);

        shipTransform = glm::translate(shipTransform, viewCamera.m_position - viewCamera.m_up);
        shipTransform = glm::rotate(shipTransform, glm::half_pi<float>(), viewCamera.m_up);

        glm::vec3 f = viewCamera.m_forward;
        glm::vec3 u = viewCamera.m_up;
        glm::vec3 r = viewCamera.m_right;
        glm::mat4 rotation{r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0, 0, 0, 0, 1};
        shipTransform *= rotation;

        meshes[0]->transformation = shipTransform;
        // const glm::mat4 model { 1.0f };
        const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspectRatio, 0.1f, 100.0f);

        // === Render shadow maps ===
        for (auto light : lights)
        {
            light->shadowMap->enablePass();
            // Clear the shadow map and set needed options
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Bind the shader
            shaders::shadow.shader.bind();

            // Set viewport size
            glViewport(0, 0, light->shadowMap->resolution, light->shadowMap->resolution);

            light->updateMvp();
            const glm::mat4 shadowMvp = light->mvp;
            glUniformMatrix4fv(shaders::shadow.vars["mvp"], 1, GL_FALSE, glm::value_ptr(shadowMvp));

            for (auto asteroid : asteroidManager.asteroids)
            {
                MeshDrawer *drawer = asteroid->isEasterEgg ? &asteroidEasterEggDrawer : &asteroidDrawer;

                drawer->transformation = asteroid->currentTransformation * asteroid->baseTransformation;
                drawer->shadow();
            }

            for (auto drawer : meshes)
            {
                if (!drawer->castShadow)
                {
                    continue;
                }

                drawer->shadow();
            }
            light->shadowMap->disablePass();
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
        for (int i = 0; i < directionalLights.size(); i++)
        {
            context.lightDirections[currentOffset] = directionalLights[i]->direction;
            context.lightColors[currentOffset] = directionalLights[i]->color;
            context.lightMvps[currentOffset] = directionalLights[i]->mvp;
            context.lightShadowMaps[currentOffset] = directionalLights[i]->shadowMap;
            context.lightTypes[currentOffset] = 0;
            context.lightEnabled[currentOffset] = directionalLights[i]->active;
            currentOffset++;
        }

        for (int i = 0; i < spotLights.size(); i++)
        {
            context.lightDirections[currentOffset] = spotLights[i]->direction;
            context.lightPositions[currentOffset] = spotLights[i]->position;
            context.lightColors[currentOffset] = spotLights[i]->color;
            context.lightMvps[currentOffset] = spotLights[i]->mvp;
            context.lightShadowMaps[currentOffset] = spotLights[i]->shadowMap;
            context.lightTypes[currentOffset] = 1;
            context.lightEnabled[currentOffset] = spotLights[i]->active;
            context.lightDistances[currentOffset] = spotLights[i]->distance;

            currentOffset++;
        }

        for (int i = 0; i < spotLights.size(); i++)
        {
            context.lightEnabled[spotLightOffset + i] = 0;
        }

        // draw earth, moon and sun
        float earthRotation = timing::time_s * 0.08f;
        glm::mat4 earthTranslation = glm::translate(glm::mat4(1.0f), cameraPos + glm::vec3(0, 0, -10.0f));
        glm::mat4 earthScaled = glm::scale(earthTranslation, glm::vec3(2.0f));
        glm::mat4 earthCorrectRotation = glm::rotate(earthScaled, pi, glm::vec3(1, 0, 0));
        earthDrawer.transformation = glm::rotate(earthCorrectRotation, earthRotation, glm::vec3(0, 1, 0));

        earthDrawer.draw(&context);

        moonDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos + glm::vec3(-2.6f, 3.2f, -10.0f)), glm::vec3(0.54));
        moonDrawer.draw(&context);

        sunDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos + sunlight.direction), glm::vec3(0.02));
        sunDrawer.draw(&context);

        skyboxDrawer.transformation = glm::scale(glm::translate(glm::mat4(1.0f), cameraPos), glm::vec3(20.0f));
        skyboxDrawer.draw(&context);

        for (int i = 0; i < spotLights.size(); i++)
        {
            context.lightEnabled[spotLightOffset + i] = 1;
        }

        // clear depth from skybox drawing
        glClear(GL_DEPTH_BUFFER_BIT);

        // render asteroids
        for (auto asteroid : asteroidManager.asteroids)
        {
            MeshDrawer *drawer = asteroid->isEasterEgg ? &asteroidEasterEggDrawer : &asteroidDrawer;

            drawer->transformation = asteroid->currentTransformation * asteroid->baseTransformation;
            drawer->draw(&context);
        }

        for (auto drawer : meshes)
        {
            drawer->draw(&context);
        }

        // === DRAW GUI ===
        ImDrawList *drawList = ImGui::GetForegroundDrawList();

        drawList->AddCircle(
            ImVec2(windowSize.x / 2.0f, windowSize.y / 2.0f),
            4,
            showShoot ? IM_COL32_WHITE : IM_COL32(255, 255, 255, 128),
            16);

        std::string scorestring = std::string("Score: ") + std::to_string(gamestate::score);

        std::string missedstring = std::string("Missed: ") + std::to_string(gamestate::missed);

        std::string powerupString = std::string("Power up available: ") + (gamestate::powerUpAvailable ? std::string("Yes!") : std::string("No."));

        std::string powerupDurationString = gamestate::paused ? std::string("<Paused>")
                                            :

                                            poweredUp
                                                ? std::string("Powered up for: ") + to_string_with_precision(powerUpTimeLeft, 1) + std::string("s")
                                            : canshoot
                                                ? std::string("")
                                                : std::string("Cooldown...");

        drawList->AddText(
            ImGui::GetFont(),
            16,
            ImVec2(50, 50),
            IM_COL32_WHITE,
            scorestring.c_str());

        drawList->AddText(
            ImGui::GetFont(),
            16,
            ImVec2(50, 70),
            IM_COL32_WHITE,
            missedstring.c_str());

        drawList->AddText(
            ImGui::GetFont(),
            16,
            ImVec2(50, 90),
            IM_COL32_WHITE,
            powerupString.c_str());

        drawList->AddText(
            ImGui::GetFont(),
            16,
            ImVec2(50, 110),
            IM_COL32_WHITE,
            powerupDurationString.c_str());

        for (auto asteroid : asteroidManager.asteroids)
        {
            glm::vec4 toScreenSpace = mvp * glm::vec4(asteroid->currentPosition, 1.0f);
            glm::vec3 convertReal = math::vec4toVec3(toScreenSpace);
            glm::vec3 screenPosition = convertReal * 0.5f + 0.5f;
            glm::vec2 pos = glm::vec2(
                std::clamp(screenPosition.x, 0.0f, 1.0f),
                1.0f - std::clamp(screenPosition.y, 0.0f, 1.0f)
            );
            
            if (screenPosition.z < 0)
            {
                // clip to edge if outside of viewport
                pos.x = pos.x > 0.5f ? 1.0f : 0.0f;
                pos.y = pos.y > 0.5f ? 1.0f : 0.0f;
            }

            drawList->AddCircle(
                ImVec2(windowSize.x * pos.x, windowSize.y * pos.y),
                8,
                IM_COL32(255, 0, 0, 128),
                6);
        }

        // ImGui::ShowMetricsWindow();
        window.swapBuffers();
    }

    return 0;
}