#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lights
{

  class DirectionalLight
  {
  public:
    glm::vec4 color;
    glm::vec3 direction;

    DirectionalLight(glm::vec4 col, glm::vec3 dir)
    {
      color = col;
      direction = dir;
    }
  };

}
