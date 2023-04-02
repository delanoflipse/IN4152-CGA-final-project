#pragma once

#include <framework/disable_all_warnings.h>
#include <framework/opengl_includes.h>
DISABLE_WARNINGS_PUSH()
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "glm/ext.hpp"
DISABLE_WARNINGS_POP()

#include <framework/mesh.h>

#include "../util/texture2D.cpp"
#include "../materials/material.cpp"
#include "../shaders/shaders.cpp"


class MeshDrawer
{
private:
  GLuint vertexBuffer;
  GLuint indexBuffer;
  GLuint vertexArray;

public:
  const Mesh *workingMesh;
  materials::Material *material;
  glm::mat4 transformation = glm::mat4(1.0f);

  MeshDrawer(const Mesh *mesh, materials::Material * mat)
  {
    workingMesh = mesh;
    material = mat;
    init();
  }

  void init()
  {
    // Create Vertex Buffer Object and Index Buffer Objects.
    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferStorage(vertexBuffer, static_cast<GLsizeiptr>(workingMesh->vertices.size() * sizeof(Vertex)), workingMesh->vertices.data(), 0);

    glCreateBuffers(1, &indexBuffer);
    glNamedBufferStorage(indexBuffer, static_cast<GLsizeiptr>(workingMesh->triangles.size() * sizeof(decltype(Mesh::triangles)::value_type)), workingMesh->triangles.data(), 0);

    // Bind vertex data to shader inputs using their index (location).
    // These bindings are stored in the Vertex Array Object.
    glCreateVertexArrays(1, &vertexArray);

    // The indices (pointing to vertices) should be read from the index buffer.
    glVertexArrayElementBuffer(vertexArray, indexBuffer);

    // The position and normal vectors should be retrieved from the specified Vertex Buffer Object.
    // The stride is the distance in bytes between vertices. We use the offset to point to the normals
    // instead of the positions.
    glVertexArrayVertexBuffer(vertexArray, 0, vertexBuffer, offsetof(Vertex, position), sizeof(Vertex));
    glVertexArrayVertexBuffer(vertexArray, 1, vertexBuffer, offsetof(Vertex, normal), sizeof(Vertex));
    glVertexArrayVertexBuffer(vertexArray, 2, vertexBuffer, offsetof(Vertex, texCoord), sizeof(Vertex));
    glEnableVertexArrayAttrib(vertexArray, 0);
    glEnableVertexArrayAttrib(vertexArray, 1);
    glEnableVertexArrayAttrib(vertexArray, 2);
  }


  void renderTriangles()
  {

    // Bind vertex data.
    glBindVertexArray(vertexArray);
    // Execute draw command.
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(workingMesh->triangles.size()) * 3, GL_UNSIGNED_INT, nullptr);
  }
  
  void draw(materials::MaterialContext * context)
  {
    material->activate(context, &transformation);
    renderTriangles();
  }

  void shadow()
  {
    int uniTransform = shaders::shadow.vars["transformation"];
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, glm::value_ptr(transformation));
    renderTriangles();
  }

  ~MeshDrawer()
  {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
  }
};
