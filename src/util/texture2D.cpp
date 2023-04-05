#pragma once

#include <stb/stb_image.h>
#include "../util/TextureUnitCounter.cpp"

namespace util
{
  class Textured2D
  {
  private:
    int textureUnit;

  public:
    GLuint texture;
    std::string pathname;
    int texWidth, texHeight, texChannels;

    Textured2D(std::string path, bool srgb = false)
    {
      pathname = path;
      textureUnit = util::getTextureUnitId();
      stbi_uc *pixels = stbi_load(pathname.c_str(), &texWidth, &texHeight, &texChannels, 3);

      glCreateTextures(GL_TEXTURE_2D, 1, &texture);
      GLint storage = srgb ? GL_SRGB8 : GL_RGB8;
      glTextureStorage2D(texture, 1, storage, texWidth, texHeight);
      glTextureSubImage2D(texture, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

      // Set behaviour for when texture coordinates are outside the [0, 1] range.
      glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
      glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void bindUniform(int uniformId)
    {
      glActiveTexture(GL_TEXTURE0 + textureUnit);
      glBindTexture(GL_TEXTURE_2D, texture);
      glUniform1i(uniformId, textureUnit);
    }

    ~Textured2D()
    {
      glDeleteTextures(1, &texture);
    }
  };

};