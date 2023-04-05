#pragma once
#include <glm/vec3.hpp>

namespace math
{
  bool raySphereIntersect(glm::vec3 rayPosition, glm::vec3 rayDirection, glm::vec3 sphereCenter, float sphereRadius)
  {
    glm::vec3 rayDir = glm::normalize(rayDirection);
    glm::vec3 fromRayToSphere = sphereCenter - rayPosition;
    float t = glm::dot(fromRayToSphere, rayDir);

    if (t < 0)
    {
      return false;
    }

    glm::vec3 tPos = rayPosition + t * rayDirection;
    glm::vec3 fromCenter = sphereCenter - tPos;
    float dist2 = glm::dot(fromCenter, fromCenter);

    return dist2 < sphereRadius * sphereRadius;
  };

};