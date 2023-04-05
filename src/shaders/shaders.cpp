
#pragma once

#include <framework/shader.h>
#include "../util/constants.cpp"

#include <map>

namespace shaders
{
  struct ShaderProgram {
    Shader shader;
    std::map<std::string , int> vars;
  };

  ShaderProgram generic;
  ShaderProgram shadow;

  void saveShaderVar(ShaderProgram * program, const char * var)
  {
    std::string str = var;
    program->vars[str] = program->shader.getUniformIndex(var);
  }

  void loadShaders()
  {
    generic.shader = ShaderBuilder()
                        .addStage(GL_VERTEX_SHADER, constants::SHADER_DIR + "shader_vert.glsl")
                        .addStage(GL_GEOMETRY_SHADER, constants::SHADER_DIR + "generic_geom.glsl")
                        .addStage(GL_FRAGMENT_SHADER, constants::SHADER_DIR + "generic_frag.glsl")
                        .build();

    saveShaderVar(&generic, "cameraPosition");

    saveShaderVar(&generic, "diffuseColor");
    saveShaderVar(&generic, "specularColor");
    saveShaderVar(&generic, "shininess");
    saveShaderVar(&generic, "ambient");

    saveShaderVar(&generic, "lightCount");
    saveShaderVar(&generic, "lightTypes");
    saveShaderVar(&generic, "lightDirections");
    saveShaderVar(&generic, "lightPosition");
    saveShaderVar(&generic, "lightColors");
    saveShaderVar(&generic, "lightEnabled");
    saveShaderVar(&generic, "lightDistances");
    saveShaderVar(&generic, "lightMVPs");
    saveShaderVar(&generic, "lightShadowMaps");
    
    saveShaderVar(&generic, "useDiffuseTexture");
    saveShaderVar(&generic, "diffuseTexture");
    saveShaderVar(&generic, "useShadowTexture");
    saveShaderVar(&generic, "shadowTexture");
    saveShaderVar(&generic, "useOverlayTexture");
    saveShaderVar(&generic, "overlayTexture");
    saveShaderVar(&generic, "useNormalTexture");
    saveShaderVar(&generic, "normalTexture");
    saveShaderVar(&generic, "useSpecularTexture");
    saveShaderVar(&generic, "specularTexture");
    saveShaderVar(&generic, "useShadows");
    saveShaderVar(&generic, "useLights");
    saveShaderVar(&generic, "toonUsage");
    saveShaderVar(&generic, "toonTexture");

    saveShaderVar(&generic, "mvp");
    saveShaderVar(&generic, "transformation");
    saveShaderVar(&generic, "normalTransformation");
    saveShaderVar(&generic, "lightShadowPixelSize");

    shadow.shader = ShaderBuilder()
                       .addStage(GL_VERTEX_SHADER, constants::SHADER_DIR + "shader_vert.glsl")
                       .build();

    saveShaderVar(&shadow, "mvp");
    saveShaderVar(&shadow, "transformation");
    saveShaderVar(&shadow, "normalTransformation");
  };

};
