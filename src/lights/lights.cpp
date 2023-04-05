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
    ShadowMap * shadowMap = NULL;
    bool active = true;
    float distance = 0;

    virtual void updateMvp() = 0;
  };
}
