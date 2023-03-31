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
      float maxDim = 100.0f;
      glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 0),
        dir,
        glm::vec3(0, 1, 0)
      );

      glm::mat4 projection = glm::ortho(-maxDim, maxDim, -maxDim, maxDim, -maxDim, maxDim);

      mvp = view * projection;
      // mvp = orthoProjection * directionView;
      // mvp = directionView;
      // mvp = projection;
    }
  };
}
