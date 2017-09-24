#ifndef YENGINE_RENDERER_VIEW_PORT_H
#define YENGINE_RENDERER_VIEW_PORT_H

#include "yengine/render_device/render_device.h"
#include "yengine/renderer/renderer_common.h"

namespace yengine { namespace renderer {

struct RenderDeviceState;

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
  void Activate(RenderDeviceState& render_device_state);

 private:
  float mTop, mLeft, mWidth, mHeight, mMinZ, mMaxZ;
  DimensionType mTopType, mLeftType, mWidthType, mHeightType;
  bool mDirty;
  uint8_t mActiveViewPortIndex;
  render_device::ViewPortID mViewPortIDs[2];
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_VIEW_PORT_H
