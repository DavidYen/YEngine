#include <YCommon/Headers/stdincludes.h>
#include "ViewPort.h"

#define INVALID_VIEWPORT static_cast<YRenderDevice::ViewPortID>(-1)

namespace YEngine { namespace YRenderer {

ViewPort::ViewPort(DimensionType top_type, float top,
                   DimensionType left_type, float left,
                   DimensionType width_type, float width,
                   DimensionType height_type, float height,
                   float min_z, float max_z)
  : mTop(top),
    mLeft(left),
    mWidth(width),
    mHeight(height),
    mMinZ(min_z),
    mMaxZ(max_z),
    mTopType(top_type),
    mLeftType(left_type),
    mWidthType(width_type),
    mHeightType(height_type),
    mActivationState(kActivationState_Unactivated),
    mActiveViewPortIndex(0) {
  mViewPortIDs[0] = INVALID_VIEWPORT;
  mViewPortIDs[1] = INVALID_VIEWPORT;
}

void ViewPort::Release() {
  mActivationState = kActivationState_Unactivated;
  for (int i = 0; i < ARRAY_SIZE(mViewPortIDs); ++i) {
    if (mViewPortIDs[i] != INVALID_VIEWPORT) {
      YRenderDevice::RenderDevice::ReleaseViewPort(mViewPortIDs[i]);
    }
  }
}

void ViewPort::SetViewPort(DimensionType top_type, float top,
                           DimensionType left_type, float left,
                           DimensionType width_type, float width,
                           DimensionType height_type, float height,
                           float min_z, float max_z) {
  mTop = top;
  mLeft = left;
  mWidth = width;
  mHeight = height;
  mMinZ = min_z;
  mMaxZ = max_z;
  mTopType = top_type;
  mLeftType = left_type;
  mWidthType = width_type;
  mHeightType = height_type;

  mActivationState = kActivationState_Unactivated;
}

void ViewPort::Activate() {
  if (mActivationState != kActivationState_Activated) {
    uint32_t frame_width, frame_height;
    YRenderDevice::RenderDevice::GetFrameBufferDimensions(frame_width,
                                                          frame_height);

    const uint32_t t = GetDimensionValue(frame_height, mTopType, mTop);
    const uint32_t l = GetDimensionValue(frame_width, mLeftType, mLeft);
    const uint32_t w = GetDimensionValue(frame_width, mWidthType, mWidth);
    const uint32_t h = GetDimensionValue(frame_height, mHeightType, mHeight);
    mActiveViewPortIndex = !mActiveViewPortIndex;
    if (mViewPortIDs[mActiveViewPortIndex] == INVALID_VIEWPORT) {
      mViewPortIDs[mActiveViewPortIndex] =
          YRenderDevice::RenderDevice::CreateViewPort(t, l, w, h, mMinZ, mMaxZ);
    } else {
      YRenderDevice::RenderDevice::SetViewPort(
          mViewPortIDs[mActiveViewPortIndex], t, l, w, h, mMinZ, mMaxZ);
    }

    mActivationState = kActivationState_Activated;
  }

  YRenderDevice::RenderDevice::ActivateViewPort(
      mViewPortIDs[mActiveViewPortIndex]);
}

}} // namespace YEngine { namespace YRenderer {
