#pragma once
#include <framework/mesh.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace shapes
{
  Mesh plane()
  {
    Mesh mesh;
    Vertex v1{
        .position = glm::vec3(0, 0, 1),
        .normal = glm::vec3(0, 1, 0),
        .texCoord = glm::vec2(0, 1),
    };
    Vertex v2{
        .position = glm::vec3(1, 0, 1),
        .normal = glm::vec3(0, 1, 0),
        .texCoord = glm::vec2(1, 1),
    };
    Vertex v3{
        .position = glm::vec3(1, 0, 0),
        .normal = glm::vec3(0, 1, 0),
        .texCoord = glm::vec2(1, 0),
    };
    Vertex v4{
        .position = glm::vec3(0, 0, 0),
        .normal = glm::vec3(0, 1, 0),
        .texCoord = glm::vec2(0, 0),
    };

    mesh.vertices.push_back(v1);
    mesh.vertices.push_back(v2);
    mesh.vertices.push_back(v3);
    mesh.vertices.push_back(v4);

    mesh.triangles.push_back(glm::uvec3(0, 1, 2));
    mesh.triangles.push_back(glm::uvec3(2, 3, 0));

    return mesh;
  }
}
