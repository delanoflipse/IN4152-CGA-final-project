#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lights
{
  class SpotLight
  {
  public:
    glm::vec4 color;
    glm::vec3 position;
    glm::vec3 direction;
    float distance;
    float size;

    SpotLight(glm::vec4 col, glm::vec3 pos, glm::vec3 dir, float s, float dist)
    {
      color = col;
      position = pos;
      direction = dir;
      size = s;
      distance = dist;
    }
  }
}