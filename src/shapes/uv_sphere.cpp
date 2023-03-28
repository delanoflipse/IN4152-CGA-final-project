#include <framework/mesh.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace shapes {
  // Adopted from https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html
  Mesh uv_unit_sphere(int rings, int segmentsPerRing)
  {
    Mesh mesh;

    Vertex topVertex{
        .position = glm::vec3(0, 1, 0),
        .normal = glm::vec3(0, 1, 0),
        .texCoord = glm::vec2(0),
    };

    double pi = glm::pi<double>();

    mesh.vertices.push_back(topVertex);
    int topVertexIndex = 0;

    // add vertices based on latitude (rings) and longitude (segments per ring) coordinates
    for (int i = 0; i < rings; i++)
    {
      auto phi = double(i + 1) * (pi / double(rings));
      auto y = glm::cos(phi);

      for (int j = 0; j < segmentsPerRing; j++)
      {

        auto theta = 2.0 * pi * double(j) / double(segmentsPerRing);
        auto x = std::sin(phi) * std::cos(theta);
        auto z = std::sin(phi) * std::sin(theta);

        Vertex newVertex{
            .position = glm::vec3(x, y, z),
            .normal = glm::vec3(x, y, z),
            .texCoord = glm::vec2(0),
        };

        mesh.vertices.push_back(newVertex);
      }
    }

    // add polar triangles
    for (int i = 0; i < segmentsPerRing; ++i)
    {
      auto i0 = i + 1;
      auto i1 = (i + 1) % segmentsPerRing + 1;
      mesh.triangles.push_back(glm::uvec3(topVertexIndex, i0, i1));
    }

    // add quads (two triangels) per ring segment
    for (int i = 1; i < rings; i++)
    {
      auto i0 = (i - 1) * segmentsPerRing + 1;
      auto i1 = (i)*segmentsPerRing + 1;

      for (int j = 0; j < segmentsPerRing; j++)
      {
        auto vi0 = i0 + j;
        auto vi1 = i0 + (j + 1) % segmentsPerRing;
        auto vi2 = i1 + (j + 1) % segmentsPerRing;
        auto vi3 = i1 + j;
        mesh.triangles.push_back(glm::uvec3(vi0, vi1, vi2));
        mesh.triangles.push_back(glm::uvec3(vi2, vi0, vi3));
      }
    }

    return mesh;
  }
}
