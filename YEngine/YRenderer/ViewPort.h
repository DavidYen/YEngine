#ifndef YENGINE_YRENDERER_VIEWPORT_H
#define YENGINE_YRENDERER_VIEWPORT_H

#include <YEngine/YRenderDevice/RenderDevice.h>

#include "RendererCommon.h"

namespace YEngine { namespace YRenderer {

class ViewPort {
 public:
  ViewPort(DimensionType top_type, float top,
           DimensionType left_type, float left,
           DimensionType width_type, float width,
           DimensionType height_type, float height,
           float min_z, float max_z);

  void Release();

  void SetViewPort(DimensionType top_type, float top,
                   DimensionType left_type, float left,
                   DimensionType width_type, float width,
                   DimensionType height_type, float height,
                   float min_z, float max_z);
  void Activate();

 private:
  float mTop, mLeft, mWidth, mHeight, mMinZ, mMaxZ;
  DimensionType mTopType, mLeftType, mWidthType, mHeightType;
  bool mDirty;
  uint8_t mActiveViewPortIndex;
  YRenderDevice::ViewPortID mViewPortIDs[2];
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_VIEWPORT_H
