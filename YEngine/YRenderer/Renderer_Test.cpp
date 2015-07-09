#include <YCommon/Headers/stdincludes.h>
#include "Renderer.h"

#include <gtest/gtest.h>

#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YEngine/YCore/StringTable.h>
#include <YEngine/YRenderDevice/RenderDevice.h>
#include <YEngine/YRenderDevice/RenderDevice_Mock.h>

namespace YEngine { namespace YRenderer {

namespace {
  const uint32_t gTestWidth = 128;
  const uint32_t gTestHeight = 128;
};

class RendererTest : public ::testing::Test {
 protected:
  RendererTest()
    : mBuffer(nullptr) {
    memset(&mHandle, 0, sizeof(mHandle));
  }

  virtual ~RendererTest() {
    delete [] mBuffer;
  }

  virtual void SetUp() {
    delete [] mBuffer;
    mBuffer = new uint8_t[10240 * 10];
    mMemBuffer.Init(mBuffer, 10240 * 10);

    YCore::StringTable::Initialize(32, 128, mMemBuffer.Allocate(10240), 10240);
    YRenderDevice::RenderDevice::Initialize(mHandle, gTestWidth, gTestHeight,
                                            mMemBuffer.Allocate(10240), 10240);
    Renderer::Initialize(mMemBuffer.Allocate(10240), 10240);
  }

  virtual void TearDown() {
    YRenderDevice::RenderDevice::Terminate();
    YCore::StringTable::Terminate();

    delete [] mBuffer;
    mBuffer = nullptr;
  }

  void* mBuffer;
  YCommon::YContainers::MemBuffer mMemBuffer;
  YCommon::YPlatform::PlatformHandle mHandle;
};

TEST_F(RendererTest, FixtureInitialization) {
}

TEST_F(RendererTest, ViewportTest) {
  const char name[] = "test_viewport";
  const size_t name_size = sizeof(name);
  YRenderDevice::RenderDeviceMock::ExpectCreateViewPort(123, 2, 3, 4, 5,
                                                        0.1f, 0.5f);
  YRenderer::Renderer::RegisterViewPort(name, name_size,
                                        2, 3, 4, 5, 0.1f, 0.5f);

  // Same hash should produce same viewport.
  YRenderer::Renderer::RegisterViewPort(name, name_size,
                                        3, 4, 5, 6, 0.1f, 0.5f);

  // 1st Release should not do anything.
  YRenderer::Renderer::ReleaseViewPort(name, name_size);

  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(123);
  EXPECT_TRUE(YRenderer::Renderer::ReleaseViewPort(name, name_size));
}

TEST_F(RendererTest, RenderTargetTest) {
  const char name[] = "test_render_target";
  const size_t name_size = sizeof(name);
  const YRenderDevice::PixelFormat format =
      static_cast<YRenderDevice::PixelFormat>(123);
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderTarget(
      234, gTestWidth / 2, gTestHeight / 2, format);
  YRenderer::Renderer::RegisterRenderTarget(name, name_size,
                                            format, 0.5f, 0.5f);

  // Same hash should produce same viewport.
  YRenderer::Renderer::RegisterRenderTarget(name, name_size,
                                            format, 1.0f, 1.0f);

  // 1st Release should not do anything.
  YRenderer::Renderer::ReleaseRenderTarget(name, name_size);

  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderTarget(234);
  EXPECT_TRUE(YRenderer::Renderer::ReleaseRenderTarget(name, name_size));
}



}} // namespace YEngine { namespace YRenderer {
