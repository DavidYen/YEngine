#include "yengine/renderer/view_port.h"

#include <gtest/gtest.h>

#include "yengine/render_device/render_device_mock.h"
#include "yengine/renderer/basic_renderer_test.h"
#include "yengine/renderer/render_device_state.h"

namespace yengine { namespace renderer {

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

  RenderDeviceState device_state;

  // First Activation should create the viewport.
  const render_device::ViewPortID viewport_id = 123;
  render_device::RenderDeviceMock::ExpectCreateViewPort(viewport_id,
                                                        1, 2, 3, 4,
                                                        0.1f, 1.0f);
  render_device::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  viewport.Activate(device_state);

  // Second activation should reuse the same viewport id.
  render_device::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  device_state.Reset();
  viewport.Activate(device_state);

  // Third activation (when changed data) should allocate a new viewport.
  viewport.SetViewPort(kDimensionType_Absolute, 4.0f,
                       kDimensionType_Percentage, 3.0f,
                       kDimensionType_Absolute, 2.0f,
                       kDimensionType_Percentage, 1.0f,
                       0.2f, 0.9f);
  const render_device::ViewPortID viewport_id2 = 234;
  render_device::RenderDeviceMock::ExpectCreateViewPort(
      viewport_id2, 4, BASIC_RENDERER_WIDTH * 3,
      2, BASIC_RENDERER_HEIGHT * 1, 0.2f, 0.9f);
  render_device::RenderDeviceMock::ExpectActivateViewPort(viewport_id2);
  device_state.Reset();
  viewport.Activate(device_state);

  // Fourth activation should re-use the first viewport id.
  viewport.SetViewPort(kDimensionType_Absolute, 2.0f,
                       kDimensionType_Percentage, 4.0f,
                       kDimensionType_Absolute, 6.0f,
                       kDimensionType_Percentage, 8.0f,
                       0.3f, 0.8f);
  render_device::RenderDeviceMock::ExpectSetViewPort(
      viewport_id, 2, BASIC_RENDERER_WIDTH * 4,
      6, BASIC_RENDERER_HEIGHT * 8, 0.3f, 0.8f);
  render_device::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  device_state.Reset();
  viewport.Activate(device_state);

  // Fifth activation should reuse the second viewport id.
  viewport.SetViewPort(kDimensionType_Absolute, 3.0f,
                       kDimensionType_Percentage, 6.0f,
                       kDimensionType_Absolute, 9.0f,
                       kDimensionType_Percentage, 12.0f,
                       0.3f, 0.8f);
  render_device::RenderDeviceMock::ExpectSetViewPort(
      viewport_id2, 3, BASIC_RENDERER_WIDTH * 6,
      9, BASIC_RENDERER_HEIGHT * 12, 0.3f, 0.8f);
  render_device::RenderDeviceMock::ExpectActivateViewPort(viewport_id2);
  device_state.Reset();
  viewport.Activate(device_state);

  // Release should release both.
  render_device::RenderDeviceMock::ExpectReleaseViewPort(viewport_id);
  render_device::RenderDeviceMock::ExpectReleaseViewPort(viewport_id2);
  viewport.Release();
}

}} // namespace yengine { namespace renderer {
