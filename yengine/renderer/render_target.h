#ifndef YENGINE_RENDERER_RENDER_TARGET_H
#define YENGINE_RENDERER_RENDER_TARGET_H

#include "yengine/render_device/render_device.h"
#include "yengine/renderer/renderer_common.h"

namespace yengine { namespace renderer {

struct RenderDeviceState;

class RenderTarget {
 public:
  RenderTarget(render_device::PixelFormat format,
               DimensionType width_type, float width,
               DimensionType height_type, float height);
  void Release();

  void SetRenderTarget(render_device::PixelFormat format,
                       DimensionType width_type, float width,
                       DimensionType height_type, float height);
  void Activate(RenderDeviceState& device_state, uint8_t target);

 private:
  float mWidth, mHeight;
  DimensionType mWidthType, mHeightType;
  render_device::PixelFormat mFormat;
  bool mDirty;
  uint8_t mActiveRenderTargetIndex;
  render_device::RenderTargetID mRenderTargetIDs[2];
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_RENDER_TARGET_H
