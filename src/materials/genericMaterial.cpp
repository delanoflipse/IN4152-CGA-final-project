#pragma once

#include <glm/vec3.hpp>
#include <framework/shader.h>
#include "material.cpp"
#include "../util/constants.cpp"
#include "../util/texture2D.cpp"
#include "../util/matrix.cpp"
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
    float ambient = 0.01f;
    float toonUsage = 0.0f;

    bool useShadows = true;
    bool useLights = true;

    util::Textured2D *overlayTexture = NULL;
    util::Textured2D *diffuseTexture = NULL;
    util::Textured2D *shadowTexture = NULL;
    util::Textured2D *toonTexture = NULL;

    GenericMaterial()
    {
    }

    void activate(MaterialContext *context, glm::mat4 *transformation)
    {
      // Bind the shader
      shaders::generic.shader.bind();

      // set context bindings
      glUniformMatrix4fv(shaders::generic.vars["mvp"], 1, GL_FALSE, glm::value_ptr(*context->mvp));
      glUniformMatrix4fv(shaders::generic.vars["transformation"], 1, GL_FALSE, glm::value_ptr(*transformation));
      glUniform3fv(shaders::generic.vars["cameraPosition"], 1, glm::value_ptr(*context->cameraPosition));

      // https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals.html
      glm::mat3 normalTransformation = math::asNormalTransform(*transformation);
      glUniformMatrix3fv(shaders::generic.vars["normalTransformation"], 1, GL_FALSE, glm::value_ptr(normalTransformation));

      // === Lights ===
      glUniform1i(shaders::generic.vars["lightCount"], context->lightCount);
      glUniform1iv(shaders::generic.vars["lightTypes"], MAX_LIGHTS, context->lightTypes);
      glUniform1iv(shaders::generic.vars["lightEnabled"], MAX_LIGHTS, context->lightEnabled);
      glUniform3fv(shaders::generic.vars["lightDirections"], MAX_LIGHTS, glm::value_ptr(context->lightDirections[0]));
      glUniform3fv(shaders::generic.vars["lightPosition"], MAX_LIGHTS, glm::value_ptr(context->lightPositions[0]));
      glUniform4fv(shaders::generic.vars["lightColors"], MAX_LIGHTS, glm::value_ptr(context->lightColors[0]));

      int uniformShadowMap = shaders::generic.vars["lightShadowMaps"];
      for (int i = 0; i < context->lightCount; i++)
      {
        context->lightShadowMaps[i]->bindUniform(uniformShadowMap + i);
        glUniformMatrix4fv(shaders::generic.vars["lightMVPs"] + i, 1, GL_FALSE, glm::value_ptr(context->lightMvps[i]));
      }

      // set material values
      glUniform3fv(shaders::generic.vars["diffuseColor"], 1, glm::value_ptr(diffuseColor));
      glUniform3fv(shaders::generic.vars["specularColor"], 1, glm::value_ptr(specularColor));
      glUniform1f(shaders::generic.vars["shininess"], shininess);
      glUniform1f(shaders::generic.vars["ambient"], ambient);

      glUniform1i(shaders::generic.vars["useShadows"], useShadows ? 1 : 0);
      glUniform1i(shaders::generic.vars["useLights"], useLights ? 1 : 0);
      if (diffuseTexture != NULL)
      {
        glUniform1i(shaders::generic.vars["useDiffuseTexture"], 1);
        diffuseTexture->bindUniform(shaders::generic.vars["diffuseTexture"]);
      }
      else
      {
        glUniform1i(shaders::generic.vars["useDiffuseTexture"], 0);
      }

      if (overlayTexture != NULL)
      {
        glUniform1i(shaders::generic.vars["useOverlayTexture"], 1);
        overlayTexture->bindUniform(shaders::generic.vars["overlayTexture"]);
      }
      else
      {
        glUniform1i(shaders::generic.vars["useOverlayTexture"], 0);
      }

      if (shadowTexture != NULL)
      {
        glUniform1i(shaders::generic.vars["useShadowTexture"], 1);
        shadowTexture->bindUniform(shaders::generic.vars["shadowTexture"]);
      }
      else
      {
        glUniform1i(shaders::generic.vars["useShadowTexture"], 0);
      }

      if (toonTexture != NULL)
      {
        glUniform1f(shaders::generic.vars["toonUsage"], toonUsage);
        toonTexture->bindUniform(shaders::generic.vars["toonTexture"]);
      }
      else
      {
        glUniform1f(shaders::generic.vars["toonUsage"], 0.0f);
      }
    }
  };
};