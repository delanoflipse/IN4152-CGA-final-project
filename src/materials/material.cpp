#pragma once

#include "../lights/shadowMap.cpp"
#include "../util/constants.cpp"

namespace materials
{
  struct MaterialContext {
    glm::mat4 * mvp;
    glm::vec3 * cameraPosition;
    int lightCount;
    // note: these are arrays
    int lightTypes[MAX_LIGHTS];
    int lightEnabled[MAX_LIGHTS];
    glm::vec3 lightDirections[MAX_LIGHTS];
    glm::vec3 lightPositions[MAX_LIGHTS];
    glm::vec4 lightColors[MAX_LIGHTS];
    glm::mat4 lightMvps[MAX_LIGHTS];
    lights::ShadowMap * lightShadowMaps[MAX_LIGHTS];
  };

  class Material
  {
  public:
    virtual void activate(MaterialContext *context, glm::mat4 * transformation) = 0;
  };

};