#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Camera.h"
#include "Ray.h"

class Renderer
{
public:
  Renderer() = default;

  void OnResize(uint32_t w, uint32_t h);
  
  void Render(const Camera& camera, glm::vec4 albido, glm::vec3 lightDir, glm::vec3 sphereOrigin);

  std::shared_ptr<Walnut::Image> getFinalImage() const { return mFinalImage; }

private:
  glm::vec4 TraceRay(const Ray& ray, glm::vec4 albido, glm::vec3 lightDir, glm::vec3 sphereOrigin);

private:
  std::shared_ptr<Walnut::Image> mFinalImage;
  uint32_t mWidth, mHeight = 0;
  uint32_t* mImageData = nullptr;
};

