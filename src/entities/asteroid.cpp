#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <random>

#include "../util/bezier.cpp"

// https://www.geeksforgeeks.org/generate-a-random-float-number-in-cpp/
float randomFloat()
{
  return float(rand()) / float(RAND_MAX);
}

float randomRange(float start, float end)
{
  return start + randomFloat() * (end - start);
}

glm::vec3 randomDirection()
{
  float rx = randomRange(-1.0, 1.0) + 0.00000000001f; // prevent divide by zero
  float ry = randomRange(-1.0, 1.0);
  float rz = randomRange(-1.0, 1.0);
  return glm::normalize(glm::vec3(rx, ry, rz));
}

namespace entities
{
  const float MIN_SCALE = 0.8f;
  const float MAX_SCALE = 1.0f / MIN_SCALE;
  class Asteroid
  {
  public:
    glm::mat4 baseTransformation;
    glm::mat4 currentTransformation;

    float speed;
    glm::vec3 spawnDirection;
    glm::vec3 targetDirection;
    float progress = 0.0f;

    Asteroid()
    {
      speed = randomRange(0.075, 0.125);

      float scaleX = randomRange(MIN_SCALE, MAX_SCALE);
      float scaleY = randomRange(MIN_SCALE, MAX_SCALE);
      float scaleZ = randomRange(MIN_SCALE, MAX_SCALE);

      float pi2 = 2 * glm::pi<float>();
      glm::vec3 rotAxis = randomDirection();
      float rotation = randomRange(0, pi2);

      glm::mat4 model = glm::mat4(1.0f);
      baseTransformation = glm::mat4(1.0f);
      baseTransformation *= glm::rotate(glm::mat4(1.0f), rotation, rotAxis);
      baseTransformation *= glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));

      spawnDirection = randomDirection();
      targetDirection = randomDirection();
      update();
    }

    void update()
    {
      float spawnRadius = 50.0f;
      // glm::vec3 direction = targetLocation - spawnLocation;
      glm::vec3 position = math::cubicBezier(
        spawnDirection * spawnRadius,
        spawnDirection * spawnRadius * 0.15f,
        targetDirection * spawnRadius * 0.15f,
        targetDirection * spawnRadius,
        progress
      );
      currentTransformation = glm::translate(glm::mat4(1.0f), position);
    }
  };
};