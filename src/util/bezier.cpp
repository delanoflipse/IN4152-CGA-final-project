#pragma once
#include <glm/vec3.hpp>

namespace math
{
  glm::vec3 quadraticBezier(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
  {
    float t1 = 1 - t;
    return t1 * t1 * p1 + 2 * t1 * p2 + t * t * p3;
  };

  glm::vec3 cubicBezier(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, float t)
  {
    float t1 = 1 - t;

    return t1 * t1 * t1 * p1 + 3 * t1 * t1 * t * p2 + 3 * t1 * t * t * p3 + t * t * t * p4;
  };

};