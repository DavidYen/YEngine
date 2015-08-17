#include <YCommon/Headers/stdincludes.h>
#include "ViewPort.h"

#include <gtest/gtest.h>

#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YEngine/YRenderDevice/RenderDevice_Mock.h>
#include <YEngine/YRenderDevice/RenderDevice.h>

namespace YEngine { namespace YRenderer {

namespace {
  const uint32_t gTestWidth = 128;
  const uint32_t gTestHeight = 128;
};

class ViewPortTest : public ::testing::Test {
 protected:
  ViewPortTest()
    : mBuffer(nullptr) {
    memset(&mHandle, 0, sizeof(mHandle));
  }

  ~ViewPortTest() override {
    delete [] mBuffer;
  }

  void SetUp() override {
    delete [] mBuffer;
    mBuffer = new uint8_t[1024 * 10];
    mMemBuffer.Init(mBuffer, 10240);
    YRenderDevice::RenderDevice::Initialize(mHandle, gTestWidth, gTestHeight,
                                            mMemBuffer.Allocate(10240), 10240);
  }

  void TearDown() override {
    YRenderDevice::RenderDevice::Terminate();

    mMemBuffer.Reset();

    delete [] mBuffer;
    mBuffer = nullptr;
  }

  void* mBuffer;
  YCommon::YContainers::MemBuffer mMemBuffer;
  YCommon::YPlatform::PlatformHandle mHandle;
};

TEST_F(ViewPortTest, EmptyRelease) {
  ViewPort viewport(kDimensionType_Absolute, 1.0f,
                    kDimensionType_Absolute, 2.0f,
                    kDimensionType_Absolute, 3.0f,
                    kDimensionType_Absolute, 4.0f,
                    0.1f, 1.0f);

  viewport.Release();
}

TEST_F(ViewPortTest, ActivationTest) {
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
  YRenderDevice::RenderDeviceMock::ExpectCreateViewPort(viewport_id2,
                                                        4, gTestWidth * 3,
                                                        2, gTestHeight * 1,
                                                        0.2f, 0.9f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id2);
  viewport.Activate();

  // Fourth activation should re-use the first viewport id.
  viewport.SetViewPort(kDimensionType_Absolute, 2.0f,
                       kDimensionType_Percentage, 4.0f,
                       kDimensionType_Absolute, 6.0f,
                       kDimensionType_Percentage, 8.0f,
                       0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectSetViewPort(viewport_id,
                                                     2, gTestWidth * 4,
                                                     6, gTestHeight * 8,
                                                     0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id);
  viewport.Activate();

  // Fifth activation should reuse the second viewport id.
  viewport.SetViewPort(kDimensionType_Absolute, 3.0f,
                       kDimensionType_Percentage, 6.0f,
                       kDimensionType_Absolute, 9.0f,
                       kDimensionType_Percentage, 12.0f,
                       0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectSetViewPort(viewport_id2,
                                                     3, gTestWidth * 6,
                                                     9, gTestHeight * 12,
                                                     0.3f, 0.8f);
  YRenderDevice::RenderDeviceMock::ExpectActivateViewPort(viewport_id2);
  viewport.Activate();

  // Release should release both.
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(viewport_id);
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(viewport_id2);
  viewport.Release();
}

}} // namespace YEngine { namespace YRenderer {
