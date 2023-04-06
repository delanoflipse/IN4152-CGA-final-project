#pragma once

#include <random>

#include <framework/disable_all_warnings.h>
#include <framework/opengl_includes.h>
DISABLE_WARNINGS_PUSH()
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "glm/ext.hpp"
#include <random>
DISABLE_WARNINGS_POP()

#include <framework/mesh.h>

#include "../util/texture2D.cpp"
#include "../util/vector.cpp"
#include "../util/clock.cpp"
#include "../materials/material.cpp"

class Particle
{
public:

    int lifetime;
    int age;
    float scale;
    float cameradistance;
    float speed;
    float initialSpeed;
    glm::vec3 localPosition;
    glm::vec3 direction;

    Particle(int initialage, int maxlifetime = 30, float particlescale = 0.05f)
    {
        cameradistance = 500.0f;
        age = initialage;
        scale = particlescale;
        lifetime = maxlifetime;
        direction = glm::vec3(0);
        localPosition = glm::vec3(0);
    }

    // void update(glm::vec3 position, glm::vec3 back, glm::vec3 up, glm::vec3 right, glm::mat4 mvp)
    // {
    //     age++;
    //     if (age % lifetime == 0) {
    //         float yoffset = random::randomRange(-0.1f, 0.1f);
    //         float xoffset = random::randomRange(-0.1f, 0.1f);
    //         localPosition = position + yoffset*up + xoffset * right;
    //         direction = back;
    //         age = 1;
    //     }

    //     localPosition += direction * speed * timing::delta_s;
    //     glm::vec3 camspace = math::vec4toVec3(mvp * glm::vec4(position, 1.0f));
    //     cameradistance = camspace.z;
    // }
    
    void update(glm::vec3 position, glm::vec3 back, glm::vec3 up, glm::vec3 right, glm::vec3 camerapos)
    {
        age++;
        if (age % lifetime == 0) {
            float yoffset = random::randomRange(-0.1f, 0.1f);
            float xoffset = random::randomRange(-0.1f, 0.1f);
            localPosition = position + yoffset*up + xoffset * right;
            direction = back;
            // speed = initialSpeed;
            age = 1;
        }

        localPosition += direction * speed * timing::delta_s;
        cameradistance = glm::length(camerapos - localPosition);
    }

    bool operator<(Particle& that) {
        return this->cameradistance < that.cameradistance;
    }
};
