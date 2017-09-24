#ifndef YENGINE_RENDER_DEVICE_PIXEL_FORMAT_H
#define YENGINE_RENDER_DEVICE_PIXEL_FORMAT_H

#include <stdint.h>

#include "ycommon/headers/macros.h"

namespace yengine { namespace render_device {

enum PixelFormat {
  kPixelFormat_A8R8G8B8,
  kPixelFormat_F32,

  NUM_PIXEL_FORMATS
};

// Format Definitions
struct Pixel_A8R8G8B8 {
  uint8_t a;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct Pixel_F32 {
  float data;
};

static const uint32_t kPixelFormatSize[] = {
  sizeof(Pixel_A8R8G8B8), // kPixelFormat_A8R8G8B8,
  sizeof(Pixel_F32),      // kPixelFormat_F32,
};
static_assert(ARRAY_SIZE(kPixelFormatSize) == NUM_PIXEL_FORMATS,
              "kPixelFormatSize must be defined for every pixel format.");

static const char* kPixelFormatNames[] = {
  "A8R8G8B8", // kPixelFormat_A8R8G8B8,
  "F32",      // kPixelFormat_F32,
};
static_assert(ARRAY_SIZE(kPixelFormatNames) == NUM_PIXEL_FORMATS,
              "kPixelFormatNames must be defined for every pixel format.");

}} // namespace yengine { namespace render_device {

#endif // YENGINE_RENDER_DEVICE_PIXEL_FORMAT_H
