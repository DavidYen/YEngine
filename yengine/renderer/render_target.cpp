#include "yengine/renderer/render_target.h"

#include "yengine/renderer/render_device_state.h"

#define INVALID_RENDER_TARGET static_cast<render_device::RenderTargetID>(-1)

namespace yengine { namespace renderer {

RenderTarget::RenderTarget(render_device::PixelFormat format,
                           DimensionType width_type, float width,
                           DimensionType height_type, float height)
  : mWidth(width),
    mHeight(height),
    mWidthType(width_type),
    mHeightType(height_type),
    mFormat(format),
    mDirty(true),
    mActiveRenderTargetIndex(0) {
  mRenderTargetIDs[0] = INVALID_RENDER_TARGET;
  mRenderTargetIDs[1] = INVALID_RENDER_TARGET;
}

void RenderTarget::Release() {
  mDirty = true;
  for (int i = 0; i < ARRAY_SIZE(mRenderTargetIDs); ++i) {
    if (mRenderTargetIDs[i] != INVALID_RENDER_TARGET) {
      render_device::RenderDevice::ReleaseRenderTarget(mRenderTargetIDs[i]);
    }
  }
}

void RenderTarget::SetRenderTarget(render_device::PixelFormat format,
                                   DimensionType width_type, float width,
                                   DimensionType height_type, float height) {
  mFormat = format;
  mWidth = width;
  mHeight = height;
  mWidthType = width_type;
  mHeightType = height_type;

  mDirty = true;
}

void RenderTarget::Activate(RenderDeviceState& device_state, uint8_t target) {
  if (mDirty) {
    uint32_t frame_width, frame_height;
    render_device::RenderDevice::GetFrameBufferDimensions(frame_width,
                                                          frame_height);

    const uint32_t w = GetDimensionValue(frame_width, mWidthType, mWidth);
    const uint32_t h = GetDimensionValue(frame_height, mHeightType, mHeight);
    mActiveRenderTargetIndex = !mActiveRenderTargetIndex;
    if (mRenderTargetIDs[mActiveRenderTargetIndex] != INVALID_RENDER_TARGET) {
      render_device::RenderDevice::ReleaseRenderTarget(
          mRenderTargetIDs[mActiveRenderTargetIndex]);
    }

    mRenderTargetIDs[mActiveRenderTargetIndex] =
        render_device::RenderDevice::CreateRenderTarget(w, h, mFormat);
    mDirty = false;
  }

  device_state.ActivateRenderTarget(target,
                                    mRenderTargetIDs[mActiveRenderTargetIndex]);
}

}} // namespace yengine { namespace renderer {
