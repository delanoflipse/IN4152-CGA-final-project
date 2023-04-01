#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <random>

// https://www.geeksforgeeks.org/generate-a-random-float-number-in-cpp/
float randomFloat()
{
    return float(rand()) / float(RAND_MAX);
}

float randomRange(float start, float end) {
  return start + randomFloat() * (end - start);
}

namespace entities
{
  class Asteroid
  {
public:
  glm::mat4 baseTransformation;
  glm::mat4 currentTransformation;


  Asteroid() {
    float scaleX = randomRange(0.75, 1.25);
    float scaleY = randomRange(0.75, 1.25);
    float scaleZ = randomRange(0.75, 1.25);

    float pi2 = 2*glm::pi<float>();
    float rx =randomRange(-1.0, 1.0);
    float ry =randomRange(-1.0, 1.0);
    float rz =randomRange(-1.0, 1.0);
    glm::vec3 rotAxis = glm::normalize(glm::vec3(rx, ry, rz));
    float rotation = randomRange(0, pi2);

    glm::mat4 model = glm::mat4(1.0f);
    baseTransformation = model;
    baseTransformation *= glm::rotate(model, rotation, rotAxis);
    baseTransformation *= glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));

    currentTransformation = glm::mat4(1.0f);
  }

  // void update() {

  // }
  };
};