#pragma once
#include <glm/glm.hpp>

namespace easing
{
  float triangle (float x) {
    if (x > 0.5) {
      return 2.0f - 2.0f * x;
    }
    return 2.0f * x;
  }

  float easeInOutCubic(float x)
  {
    return x < 0.5 ? 4 * x * x * x : 1 - glm::pow(-2 * x + 2, 3) / 2;
  }
}
