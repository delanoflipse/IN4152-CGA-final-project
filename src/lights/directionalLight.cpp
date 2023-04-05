#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


#include "shadowMap.cpp"
#include "lights.cpp"

namespace lights
{

  class DirectionalLight : public Light
  {
  public:
    glm::vec4 color;
    glm::vec3 direction;

    DirectionalLight(glm::vec4 col, glm::vec3 dir)
    {
      color = col;
      direction = dir;
      float maxDim = 50.0f;
      
      glm::mat4 view = glm::lookAt(glm::vec3(0), dir, glm::vec3(0, 1, 0));
      glm::mat4 projection = glm::ortho(-maxDim, maxDim, -maxDim, maxDim, -maxDim, maxDim);
      shadowMap = new ShadowMap(4096);
      
      mvp = view * projection;
    }

    void updateMvp(){}
  };
}
