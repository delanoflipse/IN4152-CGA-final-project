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

    glm::vec3 tPos = rayPosition + t * rayDir;
    glm::vec3 fromCenter = sphereCenter - tPos;
    float dist2 = glm::dot(fromCenter, fromCenter);

    return dist2 < sphereRadius * sphereRadius;
  };

  bool sphereSphereIntersect(glm::vec3 center1, float radius1, glm::vec3 center2, float radius2)
  {
    glm::vec3 fromCenter = center1 - center2;
    float dist2 = glm::dot(fromCenter, fromCenter);
    float intersectDistance = radius1 + radius2;
    return dist2 < intersectDistance * intersectDistance;
  };
};