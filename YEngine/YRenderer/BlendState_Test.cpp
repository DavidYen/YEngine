#include <YCommon/Headers/stdincludes.h>
#include "BlendState.h"

#include <gtest/gtest.h>

#include <YEngine/YRenderDevice/RenderDevice_Mock.h>

#include "BasicRendererTest.h"

namespace YEngine { namespace YRenderer {

class BlendStateTest : public BasicRendererTest {};

TEST_F(BlendStateTest, BlendStateEmptyRelease) {
  YRenderDevice::RenderBlendState render_blend_state;
  BlendState blend_state(render_blend_state);

  blend_state.Release();
}

TEST_F(BlendStateTest, BlendStateActivationTest) {
  YRenderDevice::RenderBlendState render_blend_state;
  BlendState blend_state(render_blend_state);

  // First Activation should create a blend state id.
  const YRenderDevice::BlendStateID blendstate_id = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateBlendState(blendstate_id,
                                                          render_blend_state);
  YRenderDevice::RenderDeviceMock::ExpectActivateBlendState(blendstate_id);
  blend_state.Activate();

  // Second activation should reuse the same viewport id.
  YRenderDevice::RenderDeviceMock::ExpectActivateBlendState(blendstate_id);
  blend_state.Activate();

  // Third activation (when changed data) should allocate a new blend state id.
  blend_state.SetBlendState(render_blend_state);
  const YRenderDevice::BlendStateID blendstate_id2 = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateBlendState(blendstate_id2,
                                                          render_blend_state);
  YRenderDevice::RenderDeviceMock::ExpectActivateBlendState(blendstate_id2);
  blend_state.Activate();

  // Fourth activation should re-use the first viewport id.
  blend_state.SetBlendState(render_blend_state);
  YRenderDevice::RenderDeviceMock::ExpectSetBlendState(blendstate_id,
                                                       render_blend_state);
  YRenderDevice::RenderDeviceMock::ExpectActivateBlendState(blendstate_id);
  blend_state.Activate();

  // Fifth activation should reuse the second viewport id.
  blend_state.SetBlendState(render_blend_state);
  YRenderDevice::RenderDeviceMock::ExpectSetBlendState(blendstate_id2,
                                                       render_blend_state);
  YRenderDevice::RenderDeviceMock::ExpectActivateBlendState(blendstate_id2);
  blend_state.Activate();

  // Release should release both.
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(blendstate_id);
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(blendstate_id2);
  blend_state.Release();
}

}} // namespace YEngine { namespace YRenderer {
