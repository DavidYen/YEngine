#ifndef YENGINE_YRENDERDEVICE_PIXELFORMAT_H
#define YENGINE_YRENDERDEVICE_PIXELFORMAT_H

namespace YEngine { namespace YRenderDevice {

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

const size_t kPixelFormatSize[] = {
  sizeof(Pixel_A8R8G8B8), // kPixelFormat_A8R8G8B8,
  sizeof(Pixel_F32),      // kPixelFormat_F32,
}
static_assert(ARRAY_SIZE(kPixelFormatSize) == NUM_PIXEL_FORMATS,
              "kPixelFormatSize must be defined for every pixel format.");

}} // namespace YEngine { namespace YRenderDevice {

#endif // YENGINE_YRENDERDEVICE_PIXELFORMAT_H
