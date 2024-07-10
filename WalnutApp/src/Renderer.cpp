#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils {

  static uint32_t ConvertToRGBA(glm::vec4& color) {
    uint8_t r = (uint8_t)(color.r * 255.0f);
    uint8_t g = (uint8_t)(color.g * 255.0f);
    uint8_t b = (uint8_t)(color.b * 255.0f);
    uint8_t a = (uint8_t)(color.a * 255.0f);

    uint32_t outColor = (a << 24) | (b << 16) | (g << 8) | r;
    return outColor;
  }
}

/// @brief Trigger when the image is resizing. Call at least once at the
/// beginning of the program to initialize the image.
/// @param w width of the image
/// @param h height of the image 
void Renderer::OnResize(uint32_t w, uint32_t h)
{
  if (mFinalImage) 
  {
    // no resize necessary
    if (mFinalImage->GetWidth() == w && mFinalImage->GetHeight() == h)
      return;
    mFinalImage->Resize(w, h);
  } 
  else 
  {
    mFinalImage = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA);
  }

  delete[] mImageData;                // deleting a null is a noop
  mImageData = new uint32_t[w * h];
}

/// @brief Call to render the image
void Renderer::Render(glm::vec4 albido, glm::vec3 lightDir, glm::vec3 sphereOrigin)
{
  const uint32_t w = mFinalImage->GetWidth();
  const uint32_t h = mFinalImage->GetHeight();
  // render every pixel
  for (uint32_t y = 0; y < h; y++) {
    for (uint32_t x = 0; x < w; x++) {
      glm::vec2 coord = { (float)x / (float)w, (float)y / (float)h };
      coord = coord * 2.0f - 1.0f;  // remap so -1,-1 is lower left and 1,1 is upper right
      
      glm::vec4 color = PerPixel(coord, albido, lightDir, sphereOrigin);
      color = glm::clamp(color, 0.0f, 1.0f);
      mImageData[x + y * w] = Utils::ConvertToRGBA(color);
    }
  }

  mFinalImage->SetData(mImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord, glm::vec4 albido, glm::vec3 lightDir, glm::vec3 sphereOrigin)
{

  // (bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 - r^2) = 0
  //
  // a = ray origin
  // b = ray direction
  // r = radius of sphere
  // t = hit distance
  glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
  glm::vec3 rayOrigin(-sphereOrigin.x, -sphereOrigin.y, 2.0f - sphereOrigin.z);
  
  float radius = 0.5f;
  float a = glm::dot(rayDirection, rayDirection);
  float b = 2.0f * glm::dot(rayOrigin, rayDirection);
  float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

  
  // discriminant
  float discrim = b * b - 4.0f * a * c;
  if (discrim < 0.0f)
    return glm::vec4(0,0,0,1);

  // quadriatic formula 
  // b^2 - 4ac
  //-----------
  // (-b +/- sqrt(discriminant)) / (2.0f * a)
  float t0 = (-b + glm::sqrt(discrim)) / (2.0f * a);
  float closestT = (-b - glm::sqrt(discrim)) / (2.0f * a);

  glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;

  glm::vec3 normal = glm::normalize(hitPoint);  

  glm::vec3 lightDirN = glm::normalize(lightDir);

  float d = glm::dot(-lightDirN, normal);

  glm::vec3 sphereColor(normal * 0.5f + 0.5f);
  sphereColor = albido * d;

  return glm::vec4(sphereColor, 1.0f);
}

