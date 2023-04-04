#pragma once
#include "../util/TextureUnitCounter.cpp"

namespace lights
{

  class ShadowMap
  {
  private:
    int textureUnit;

  public:
    int resolution;
    GLuint shadowMapTexture;
    GLuint framebuffer;

    ShadowMap(int res = 1024)
    {
      resolution = res;
      textureUnit = util::getTextureUnitId();
      init();
    }

    void init()
    {
      glCreateTextures(GL_TEXTURE_2D, 1, &shadowMapTexture);
      glTextureStorage2D(shadowMapTexture, 1, GL_DEPTH_COMPONENT32F, resolution, resolution);
      glTextureParameteri(shadowMapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTextureParameteri(shadowMapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glTextureParameteri(shadowMapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTextureParameteri(shadowMapTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glCreateFramebuffers(1, &framebuffer);
      glNamedFramebufferTexture(framebuffer, GL_DEPTH_ATTACHMENT, shadowMapTexture, 0);
    }

    void enablePass()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void disablePass()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bindUniform(int uniformId)
    {
      glActiveTexture(GL_TEXTURE0 + textureUnit);
      glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
      glUniform1i(uniformId, textureUnit);
    }

    ~ShadowMap()
    {
      glDeleteFramebuffers(1, &framebuffer);
      glDeleteTextures(1, &shadowMapTexture);
    }
  };
};