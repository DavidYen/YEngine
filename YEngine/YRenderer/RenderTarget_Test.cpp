#include <YCommon/Headers/stdincludes.h>
#include "RenderTarget.h"

#include <gtest/gtest.h>

#include <YEngine/YRenderDevice/RenderDevice_Mock.h>

#include "BasicRendererTest.h"

namespace YEngine { namespace YRenderer {

class RenderTargetTest : public BasicRendererTest {};

TEST_F(RenderTargetTest, RenderTargetEmptyRelease) {
  RenderTarget render_target(YRenderDevice::kPixelFormat_A8R8G8B8,
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f);
  render_target.Release();
}

TEST_F(RenderTargetTest, RenderTargetActivationTest) {
  const YRenderDevice::PixelFormat kPixelFormat =
      YRenderDevice::kPixelFormat_A8R8G8B8;
  RenderTarget render_target(kPixelFormat,
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f);

  // First Activation should create a render target.
  const YRenderDevice::RenderTargetID rendertarget_id = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderTarget(
      rendertarget_id, 1, 2, kPixelFormat);
  YRenderDevice::RenderDeviceMock::ExpectActivateRenderTarget(
      0, rendertarget_id);
  render_target.Activate(0);

  // Second activation should reuse the same render_target id.
  YRenderDevice::RenderDeviceMock::ExpectActivateRenderTarget(
      1, rendertarget_id);
  render_target.Activate(1);

  // Third activation (when changed data) should allocate a new render target.
  render_target.SetRenderTarget(kPixelFormat,
                                kDimensionType_Percentage, 2.0f,
                                kDimensionType_Percentage, 3.0f);
  const YRenderDevice::RenderTargetID rendertarget_id2 = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderTarget(
      rendertarget_id2,
      BASIC_RENDERER_WIDTH * 2, BASIC_RENDERER_HEIGHT * 3, kPixelFormat);
  YRenderDevice::RenderDeviceMock::ExpectActivateRenderTarget(
      3, rendertarget_id2);
  render_target.Activate(3);

  // Fourth activation should delete the first id and create a new one.
  render_target.SetRenderTarget(kPixelFormat,
                                kDimensionType_Absolute, 2.0f,
                                kDimensionType_Absolute, 4.0f);
  const YRenderDevice::RenderTargetID rendertarget_id3 = 246;
  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderTarget(rendertarget_id);
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderTarget(
      rendertarget_id3,2, 4, kPixelFormat);
  YRenderDevice::RenderDeviceMock::ExpectActivateRenderTarget(
      0, rendertarget_id3);
  render_target.Activate(0);

  // Release should release both.
  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderTarget(rendertarget_id3);
  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderTarget(rendertarget_id2);
  render_target.Release();
}

}} // namespace YEngine { namespace YRenderer {
