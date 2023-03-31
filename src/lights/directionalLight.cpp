#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


#include "shadowMap.cpp"

namespace lights
{

  class DirectionalLight
  {
  public:
    glm::vec4 color;
    glm::vec3 direction;
    glm::mat4 mvp;
    ShadowMap shadowMap;

    DirectionalLight(glm::vec4 col, glm::vec3 dir)
    {
      color = col;
      direction = dir;
      mvp = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);
    }
  };

}
