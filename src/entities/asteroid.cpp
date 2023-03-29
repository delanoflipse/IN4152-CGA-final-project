#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <random>

// https://www.geeksforgeeks.org/generate-a-random-float-number-in-cpp/
float randomFloat()
{
    return (float)(rand()) / (float)(rand());
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
    float scaleX = randomRange(0.9, 1.1);
    float scaleY = randomRange(0.9, 1.1);
    float scaleZ = randomRange(0.9, 1.1);

    float pi2 = 2*glm::pi<float>();
    float rotateX = randomRange(0, pi2);
    float rotateY = randomRange(0, pi2);
    float rotateZ = randomRange(0, pi2);

    glm::mat4 model = glm::mat4(1.0f);
    baseTransformation = model;
    baseTransformation *= glm::rotate(model, rotateX, glm::vec3(1, 0, 0));
    baseTransformation *= glm::rotate(model, rotateY, glm::vec3(0, 1, 0));
    baseTransformation *= glm::rotate(model, rotateZ, glm::vec3(0, 0, 1));
    baseTransformation *= glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));

    currentTransformation = glm::mat4(1.0f);
  }

  // void update() {

  // }
  };
};