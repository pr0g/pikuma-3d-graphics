#include "lighting.h"

uint32_t apply_light_intensity(const uint32_t color, const float t) {
  const uint32_t a = color & 0xff000000;
  const uint32_t r = (color & 0x00ff0000) * t;
  const uint32_t g = (color & 0x0000ff00) * t;
  const uint32_t b = (color & 0x000000ff) * t;
  return a | (r & 0x00ff0000) | (g & 0x0000ff00) | (b & 0x000000ff);
}
