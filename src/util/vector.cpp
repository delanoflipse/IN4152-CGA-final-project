#pragma once
#include <glm/vec3.hpp>

namespace math {
  glm::vec3 vec4toVec3(glm::vec4 vec) {
    return glm::vec3(
      vec.x / vec.w,
      vec.y / vec.w,
      vec.z / vec.w
    );
  };

};