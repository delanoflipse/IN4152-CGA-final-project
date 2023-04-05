#pragma once
#include <glm/glm.hpp>

namespace random
{
  // https://www.geeksforgeeks.org/generate-a-random-float-number-in-cpp/
  float randomFloat()
  {
    return float(rand()) / float(RAND_MAX);
  }

  float randomRange(float start, float end)
  {
    return start + randomFloat() * (end - start);
  }

}
