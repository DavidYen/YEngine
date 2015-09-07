#ifndef YENGINE_YRENDERER_RENDERTARGET_H
#define YENGINE_YRENDERER_RENDERTARGET_H

#include <YEngine/YRenderDevice/RenderDevice.h>

#include "RendererCommon.h"

namespace YEngine { namespace YRenderer {

struct RenderDeviceState;

class RenderTarget {
 public:
  RenderTarget(YRenderDevice::PixelFormat format,
               DimensionType width_type, float width,
               DimensionType height_type, float height);
  void Release();

  void SetRenderTarget(YRenderDevice::PixelFormat format,
                       DimensionType width_type, float width,
                       DimensionType height_type, float height);
  void Activate(RenderDeviceState& device_state, uint8_t target);

 private:
  float mWidth, mHeight;
  DimensionType mWidthType, mHeightType;
  YRenderDevice::PixelFormat mFormat;
  bool mDirty;
  uint8_t mActiveRenderTargetIndex;
  YRenderDevice::RenderTargetID mRenderTargetIDs[2];
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERTARGET_H
