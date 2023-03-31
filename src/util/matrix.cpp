#pragma once
#include <glm/vec3.hpp>

namespace math {
  glm::mat3 asNormalTransform(glm::mat4 transform) {
    return glm::transpose(glm::inverse(glm::mat3(transform)));
  }
}