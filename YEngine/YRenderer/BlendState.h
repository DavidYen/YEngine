#ifndef YENGINE_YRENDERER_BLENDSTATE_H
#define YENGINE_YRENDERER_BLENDSTATE_H

#include <YEngine/YRenderDevice/RenderDevice.h>

#include "RendererCommon.h"

namespace YEngine { namespace YRenderer {

class BlendState {
 public:
  BlendState(const YRenderDevice::RenderBlendState& blend_state);
  void Release();

  void SetBlendState(const YRenderDevice::RenderBlendState& blend_state);
  void Activate();

 private:
  YRenderDevice::RenderBlendState mBlendState;
  bool mDirty;
  uint8_t mActiveBlendStateIndex;
  YRenderDevice::RenderBlendStateID mBlendStateIDs[2];
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_BLENDSTATE_H
