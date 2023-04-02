#pragma once
#include <glm/vec3.hpp>

namespace math {
  glm::mat3 asNormalTransform(glm::mat4 transform) {
    glm::mat3 asMat3 = glm::mat3(transform);
    return glm::transpose(glm::inverse(asMat3));
  }
}