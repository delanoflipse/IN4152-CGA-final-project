#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <random>

#include "../util/bezier.cpp"
#include "../util/easing.cpp"
#include "../util/random.cpp"

glm::vec3 randomDirection()
{
  float rx = random::randomRange(-1.0, 1.0) + 0.00000000001f; // prevent divide by zero
  float ry = random::randomRange(-1.0, 1.0);
  float rz = random::randomRange(-1.0, 1.0);
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
    glm::mat4 inverseBaseTransform;
    glm::mat4 currentTransformation;

    float speed;
    glm::vec3 spawnDirection;
    glm::vec3 targetDirection;
    glm::vec3 centerPosition;
    glm::vec3 currentPosition;
    float progress = 0.0f;
    bool isEasterEgg = false;

    Asteroid(glm::vec3 centerpos)
    {
      centerPosition = centerpos;
      speed = random::randomRange(0.075, 0.1);
      isEasterEgg = random::randomFloat() > 0.95;

      float scaleX = random::randomRange(MIN_SCALE, MAX_SCALE);
      float scaleY = random::randomRange(MIN_SCALE, MAX_SCALE);
      float scaleZ = random::randomRange(MIN_SCALE, MAX_SCALE);

      float pi2 = 2 * glm::pi<float>();
      glm::vec3 rotAxis = randomDirection();
      float rotation = random::randomRange(0, pi2);

      glm::mat4 model = glm::mat4(1.0f);
      baseTransformation = glm::mat4(1.0f);
      baseTransformation *= glm::rotate(glm::mat4(1.0f), rotation, rotAxis);
      baseTransformation *= glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));
      
      inverseBaseTransform = glm::inverse(baseTransformation);

      spawnDirection = randomDirection();
      targetDirection = randomDirection();
      update();
    }

    void update()
    {
      float spawnRadius = 50.0f;
      float scaleNorm = std::min(1.0f, 10.0f * easing::triangle(progress));
      float scale = easing::easeInOutCubic(scaleNorm);

      // glm::vec3 direction = targetLocation - spawnLocation;
      currentPosition = math::cubicBezier(
        centerPosition + spawnDirection * spawnRadius,
        centerPosition + spawnDirection * spawnRadius * 0.15f,
        centerPosition + targetDirection * spawnRadius * 0.15f,
        centerPosition + targetDirection * spawnRadius,
        progress
      );
      
      currentTransformation = glm::translate(glm::mat4(1.0f), currentPosition);
      currentTransformation = glm::scale(currentTransformation, glm::vec3(scale));
    }
  };
};