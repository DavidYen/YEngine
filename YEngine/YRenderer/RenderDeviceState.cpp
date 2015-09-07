#include <YCommon/Headers/stdincludes.h>
#include "RenderDeviceState.h"

#include <string.h>

namespace YEngine { namespace YRenderer {

void RenderDeviceState::Reset() {
  memset(this, -1, sizeof(*this));
}

void RenderDeviceState::ActivateViewPort(YRenderDevice::ViewPortID id) {
  if (mSetViewPort != id) {
    YRenderDevice::RenderDevice::ActivateViewPort(id);
    mSetViewPort = id;
  }
}

void RenderDeviceState::ActivateBlendState(
    YRenderDevice::RenderBlendStateID id) {
  if (mSetBlendStateID != id) {
    YRenderDevice::RenderDevice::ActivateRenderBlendState(id);
    mSetBlendStateID = id;
  }
}

void RenderDeviceState::ActivateRenderTarget(uint8_t target,
                                             YRenderDevice::RenderTargetID id) {
  if (mRenderTargetIDs[target] != id) {
    YRenderDevice::RenderDevice::ActivateRenderTarget(target, id);
    mRenderTargetIDs[target] = id;
  }
}

}} // namespace YEngine { namespace YRenderer {
