#include "yengine/renderer/render_target.h"

#include <gtest/gtest.h>

#include "yengine/render_device/render_device_mock.h"
#include "yengine/renderer/basic_renderer_test.h"
#include "yengine/renderer/render_device_state.h"

namespace yengine { namespace renderer {

class RenderTargetTest : public BasicRendererTest {};

TEST_F(RenderTargetTest, RenderTargetEmptyRelease) {
  RenderTarget render_target(render_device::kPixelFormat_A8R8G8B8,
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f);
  render_target.Release();
}

TEST_F(RenderTargetTest, RenderTargetActivationTest) {
  const render_device::PixelFormat kPixelFormat =
      render_device::kPixelFormat_A8R8G8B8;
  RenderTarget render_target(kPixelFormat,
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f);

  RenderDeviceState device_state;

  // First Activation should create a render target.
  const render_device::RenderTargetID rendertarget_id = 123;
  render_device::RenderDeviceMock::ExpectCreateRenderTarget(
      rendertarget_id, 1, 2, kPixelFormat);
  render_device::RenderDeviceMock::ExpectActivateRenderTarget(
      0, rendertarget_id);
  device_state.Reset();
  render_target.Activate(device_state, 0);

  // Second activation should reuse the same render_target id.
  render_device::RenderDeviceMock::ExpectActivateRenderTarget(
      1, rendertarget_id);
  device_state.Reset();
  render_target.Activate(device_state, 1);

  // Third activation (when changed data) should allocate a new render target.
  render_target.SetRenderTarget(kPixelFormat,
                                kDimensionType_Percentage, 2.0f,
                                kDimensionType_Percentage, 3.0f);
  const render_device::RenderTargetID rendertarget_id2 = 234;
  render_device::RenderDeviceMock::ExpectCreateRenderTarget(
      rendertarget_id2,
      BASIC_RENDERER_WIDTH * 2, BASIC_RENDERER_HEIGHT * 3, kPixelFormat);
  render_device::RenderDeviceMock::ExpectActivateRenderTarget(
      3, rendertarget_id2);
  device_state.Reset();
  render_target.Activate(device_state, 3);

  // Fourth activation should delete the first id and create a new one.
  render_target.SetRenderTarget(kPixelFormat,
                                kDimensionType_Absolute, 2.0f,
                                kDimensionType_Absolute, 4.0f);
  const render_device::RenderTargetID rendertarget_id3 = 246;
  render_device::RenderDeviceMock::ExpectReleaseRenderTarget(rendertarget_id);
  render_device::RenderDeviceMock::ExpectCreateRenderTarget(
      rendertarget_id3,2, 4, kPixelFormat);
  render_device::RenderDeviceMock::ExpectActivateRenderTarget(
      0, rendertarget_id3);
  device_state.Reset();
  render_target.Activate(device_state, 0);

  // Release should release both.
  render_device::RenderDeviceMock::ExpectReleaseRenderTarget(rendertarget_id3);
  render_device::RenderDeviceMock::ExpectReleaseRenderTarget(rendertarget_id2);
  render_target.Release();
}

}} // namespace yengine { namespace renderer {
