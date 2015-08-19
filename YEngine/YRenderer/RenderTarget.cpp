#include <YCommon/Headers/stdincludes.h>
#include "RenderTarget.h"

#define INVALID_RENDER_TARGET static_cast<YRenderDevice::RenderTargetID>(-1)

namespace YEngine { namespace YRenderer {

RenderTarget::RenderTarget(YRenderDevice::PixelFormat format,
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
      YRenderDevice::RenderDevice::ReleaseRenderTarget(mRenderTargetIDs[i]);
    }
  }
}

void RenderTarget::SetRenderTarget(YRenderDevice::PixelFormat format,
                                   DimensionType width_type, float width,
                                   DimensionType height_type, float height) {
  mFormat = format;
  mWidth = width;
  mHeight = height;
  mWidthType = width_type;
  mHeightType = height_type;

  mDirty = true;
}

void RenderTarget::Activate(int target) {
  if (mDirty) {
    uint32_t frame_width, frame_height;
    YRenderDevice::RenderDevice::GetFrameBufferDimensions(frame_width,
                                                          frame_height);

    const uint32_t w = GetDimensionValue(frame_width, mWidthType, mWidth);
    const uint32_t h = GetDimensionValue(frame_height, mHeightType, mHeight);
    mActiveRenderTargetIndex = !mActiveRenderTargetIndex;
    if (mRenderTargetIDs[mActiveRenderTargetIndex] != INVALID_RENDER_TARGET) {
      YRenderDevice::RenderDevice::ReleaseRenderTarget(
          mRenderTargetIDs[mActiveRenderTargetIndex]);
    }

    mRenderTargetIDs[mActiveRenderTargetIndex] =
        YRenderDevice::RenderDevice::CreateRenderTarget(w, h, mFormat);
    mDirty = false;
  }

  YRenderDevice::RenderDevice::ActivateRenderTarget(
      target, mRenderTargetIDs[mActiveRenderTargetIndex]);
}

}} // namespace YEngine { namespace YRenderer {
