#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


#include "shadowMap.cpp"

namespace lights
{

  class Light
  {
  public:
    glm::mat4 mvp;
    ShadowMap shadowMap;
    bool active = true;

    virtual void updateMvp() = 0;
  };
}
