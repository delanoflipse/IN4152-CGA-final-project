#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "shadowMap.cpp"
#include "lights.cpp"

namespace lights
{
  class SpotLight : public Light
  {
  public:
    glm::vec4 color;
    glm::vec3 position;
    glm::vec3 direction;
    float distance;
    float fieldOfView;

    SpotLight(glm::vec4 col, glm::vec3 pos, glm::vec3 dir, float fov, float dist)
    {
      color = col;
      position = pos;
      direction = glm::normalize(dir);
      fieldOfView = fov;
      distance = dist;
    }

    void updateMvp()
    {
      glm::mat4 view = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
      glm::mat4 projection = glm::perspective(fieldOfView, 1.0f, 0.1f, distance);

      mvp = projection * view;
    }
  };
};
