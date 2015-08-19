#include <YCommon/Headers/stdincludes.h>
#include "BlendState.h"

#define INVALID_BLEND_STATE static_cast<YRenderDevice::RenderBlendStateID>(-1)

namespace YEngine { namespace YRenderer {

BlendState::BlendState(const YRenderDevice::RenderBlendState& blend_state)
  : mBlendState(blend_state),
    mDirty(true),
    mActiveBlendStateIndex(0) {
  mBlendStateIDs[0] = INVALID_BLEND_STATE;
  mBlendStateIDs[1] = INVALID_BLEND_STATE;
}

void BlendState::Release() {
  mDirty = true;
  for (int i = 0; i < ARRAY_SIZE(mBlendStateIDs); ++i) {
    if (mBlendStateIDs[i] != INVALID_BLEND_STATE) {
      YRenderDevice::RenderDevice::ReleaseRenderBlendState(mBlendStateIDs[i]);
    }
  }
}

void BlendState::SetBlendState(
    const YRenderDevice::RenderBlendState& blend_state) {
  mBlendState = blend_state;
  mDirty = true;
}

void BlendState::Activate() {
  if (mDirty) {
    mActiveBlendStateIndex = !mActiveBlendStateIndex;
    if (mBlendStateIDs[mActiveBlendStateIndex] == INVALID_BLEND_STATE) {
      mBlendStateIDs[mActiveBlendStateIndex] =
          YRenderDevice::CreateRenderBlendState(mBlendState);
    } else {
      YRenderDevice::SetRenderBlendState(mBlendStateIDs[mActiveBlendStateIndex],
                                         mBlendState);
    }
    mDirty = false;
  }

  YRenderDevice::RenderDevice::ActivateRenderBlendState(
      target, mBlendStateIDs[mActiveBlendStateIndex]);
}

}} // namespace YEngine { namespace YRenderer {
