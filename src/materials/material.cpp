#pragma once

#include "../lights/shadowMap.cpp"

namespace materials
{
  struct MaterialContext {
    glm::mat4 * mvp;
    glm::vec3 * cameraPosition;
    int directionalLights;
    // note: these are arrays
    glm::vec3 * directionLightDirections;
    glm::vec4 * directionLightColors;
    glm::mat4 * directionLightMvps;
    lights::ShadowMap ** directionLightShadows;
  };

  class Material
  {
  public:
    virtual void activate(MaterialContext context, glm::mat4 * transformation) = 0;
  };

};