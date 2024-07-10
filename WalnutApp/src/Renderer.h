#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Renderer
{
public:
  Renderer() = default;

  void OnResize(uint32_t w, uint32_t h);
  
  void Render(glm::vec4 albido, glm::vec3 lightDir, glm::vec3 sphereOrigin);

  std::shared_ptr<Walnut::Image> getFinalImage() const { return mFinalImage; }

private:
  glm::vec4 PerPixel(glm::vec2 coord, glm::vec4 albido, glm::vec3 lightDir, glm::vec3 sphereOrigin);

private:
  std::shared_ptr<Walnut::Image> mFinalImage;
  uint32_t mWidth, mHeight = 0;
  uint32_t* mImageData = nullptr;
};

