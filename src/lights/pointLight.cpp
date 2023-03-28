#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lights
{
  class PointLight
  {
  public:
    glm::vec4 color;
    glm::vec3 position;
    float distance;

    PointLight(glm::vec4 col, glm::vec3 pos, float dist)
    {
      color = col;
      position = pos;
      distance = dist;
    }
  }
}