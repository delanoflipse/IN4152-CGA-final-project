#pragma once

#include <glm/vec3.hpp>
#include <framework/shader.h>
#include "material.cpp"
#include "../util/constants.cpp"
#include "../util/texture2D.cpp"
#include "../shaders/shaders.cpp"

namespace materials
{
  class GenericMaterial : public Material
  {

  public:
    glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambientColor = glm::vec3(0.0f);
    glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float shininess = 1.0f;

    util::Textured2D *texture = NULL;

    GenericMaterial()
    {
    }

    void activate(MaterialContext context, glm::mat4 *transformation)
    {
      // https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals.html
      glm::mat3 normalTransformation = glm::transpose(glm::inverse(glm::mat3(*transformation)));

      // Bind the shader
      shaders::generic.shader.bind();

      // set context bindings
      glUniformMatrix4fv(shaders::generic.vars["mvp"], 1, GL_FALSE, glm::value_ptr(*context.mvp));
      glUniformMatrix4fv(shaders::generic.vars["transformation"], 1, GL_FALSE, glm::value_ptr(*transformation));
      glUniformMatrix3fv(shaders::generic.vars["normalTransformation"], 1, GL_FALSE, glm::value_ptr(normalTransformation));
      glUniform3fv(shaders::generic.vars["cameraPosition"], 1, glm::value_ptr(*context.cameraPosition));
      glUniform1i(shaders::generic.vars["directionalLights"], context.directionalLights);
      glUniform3fv(shaders::generic.vars["directionalLightDirections"], 2, glm::value_ptr(context.directionLightDirections[0]));
      glUniform4fv(shaders::generic.vars["directionalLightColors"], 2, glm::value_ptr(context.directionLightColors[0]));
      glUniformMatrix4fv(shaders::generic.vars["directionalLightMVPs"], 2, GL_FALSE, glm::value_ptr(context.directionLightMvps[0]));

      // set material values
      glUniform3fv(shaders::generic.vars["diffuseColor"], 1, glm::value_ptr(diffuseColor));
      glUniform3fv(shaders::generic.vars["specularColor"], 1, glm::value_ptr(specularColor));
      glUniform1f(shaders::generic.vars["shininess"], shininess);

      if (texture != NULL)
      {
        glUniform1i(shaders::generic.vars["useTexture"], 1);
        texture->bindUniform(shaders::generic.vars["uvTexture"]);
      }
      else
      {
        glUniform1i(shaders::generic.vars["useTexture"], 0);
      }
    }
  };
};