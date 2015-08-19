#include <YCommon/Headers/stdincludes.h>
#include "ViewPort.h"

#include <gtest/gtest.h>

#include <YEngine/YRenderDevice/RenderDevice_Mock.h>

#include "BasicRendererTest.h"

namespace YEngine { namespace YRenderer {

class ViewPortTest : public BasicRendererTest {};

TEST_F(ViewPortTest, ViewPortEmptyRelease) {
  ViewPort viewport(kDimensionType_Absolute, 1.0f,
                    kDimensionType_Absolute, 2.0f,
                    kDimensionType_Absolute, 3.0f,
                    kDimensionType_Absolute, 4.0f,
                    0.1f, 1.0f);

  viewport.Release();
}

TEST_F(ViewPortTest, ViewPortActivationTest) {
  ViewPort viewport(kDimensionType_Absolute, 1.0f,
                    kDimensionType_Absolute, 2.0f,
                    kDimensionType_Absolute, 3.0f,
                    kDimensionType_Absolute, 4.0f,
                    0.1f, 1.0f);

  // First Activation should create the viewport.
  const YRenderDevice::ViewPortID viewport_id = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateViewPort(viewport_id,
                                                        1, 2, 3, 4,
                                                        0.1f, 1.0f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  viewport.Activate();

  // Second activation should reuse the same viewport id.
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  viewport.Activate();

  // Third activation (when changed data) should allocate a new viewport.
  viewport.SetViewPort(kDimensionType_Absolute, 4.0f,
                       kDimensionType_Percentage, 3.0f,
                       kDimensionType_Absolute, 2.0f,
                       kDimensionType_Percentage, 1.0f,
                       0.2f, 0.9f);
  const YRenderDevice::ViewPortID viewport_id2 = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateViewPort(
      viewport_id2, 4, BASIC_RENDERER_WIDTH * 3,
      2, BASIC_RENDERER_HEIGHT * 1, 0.2f, 0.9f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id2);
  viewport.Activate();

  // Fourth activation should re-use the first viewport id.
  viewport.SetViewPort(kDimensionType_Absolute, 2.0f,
                       kDimensionType_Percentage, 4.0f,
                       kDimensionType_Absolute, 6.0f,
                       kDimensionType_Percentage, 8.0f,
                       0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectSetViewPort(
      viewport_id, 2, BASIC_RENDERER_WIDTH * 4,
      6, BASIC_RENDERER_HEIGHT * 8, 0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  viewport.Activate();

  // Fifth activation should reuse the second viewport id.
  viewport.SetViewPort(kDimensionType_Absolute, 3.0f,
                       kDimensionType_Percentage, 6.0f,
                       kDimensionType_Absolute, 9.0f,
                       kDimensionType_Percentage, 12.0f,
                       0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectSetViewPort(
      viewport_id2, 3, BASIC_RENDERER_WIDTH * 6,
      9, BASIC_RENDERER_HEIGHT * 12, 0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id2);
  viewport.Activate();

  // Release should release both.
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(viewport_id);
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(viewport_id2);
  viewport.Release();
}

}} // namespace YEngine { namespace YRenderer {
