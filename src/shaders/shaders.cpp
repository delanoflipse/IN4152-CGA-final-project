
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
                        .addStage(GL_FRAGMENT_SHADER, constants::SHADER_DIR + "generic_frag.glsl")
                        .build();

    saveShaderVar(&generic, "cameraPosition");
    saveShaderVar(&generic, "directionalLights");
    saveShaderVar(&generic, "directionalLightDirections");
    saveShaderVar(&generic, "directionalLightColors");
    saveShaderVar(&generic, "directionalLightMVPs");
    saveShaderVar(&generic, "directionalLightShadowMaps");
    saveShaderVar(&generic, "diffuseColor");
    saveShaderVar(&generic, "specularColor");
    saveShaderVar(&generic, "shininess");
    saveShaderVar(&generic, "useTexture");
    saveShaderVar(&generic, "uvTexture");

    saveShaderVar(&generic, "mvp");
    saveShaderVar(&generic, "transformation");
    saveShaderVar(&generic, "normalTransformation");

    shadow.shader = ShaderBuilder()
                       .addStage(GL_VERTEX_SHADER, constants::SHADER_DIR + "shader_vert.glsl")
                       .build();

    saveShaderVar(&shadow, "mvp");
    saveShaderVar(&shadow, "transformation");
    saveShaderVar(&shadow, "normalTransformation");
  };

};
