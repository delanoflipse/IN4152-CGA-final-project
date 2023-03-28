#pragma once

#include <exception>

int textureCounter;

namespace util
{
  int getTextureUnitId()
  {
    int currentId = textureCounter;
    int max;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max);
    if (currentId >= max)
    {
      throw std::exception("Exceeded texture unit threshold");
    }
    textureCounter += 1;
    return currentId;
  }
}