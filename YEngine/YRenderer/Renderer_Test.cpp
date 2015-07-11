#include <YCommon/Headers/stdincludes.h>
#include "Renderer.h"

#include <gtest/gtest.h>

#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YEngine/YCore/StringTable.h>
#include <YEngine/YRenderDevice/RenderBlendState.h>
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
  Renderer::RegisterViewPort(name, sizeof(name),
                             Renderer::kDimensionType_Absolute, 1.0f,
                             Renderer::kDimensionType_Absolute, 2.0f,
                             Renderer::kDimensionType_Absolute, 3.0f,
                             Renderer::kDimensionType_Absolute, 4.0f,
                             0.1f, 1.0f);
  Renderer::RegisterViewPort(name, sizeof(name),
                             Renderer::kDimensionType_Absolute, 1.0f,
                             Renderer::kDimensionType_Absolute, 2.0f,
                             Renderer::kDimensionType_Absolute, 3.0f,
                             Renderer::kDimensionType_Absolute, 4.0f,
                             0.1f, 1.0f);

  EXPECT_FALSE(Renderer::ReleaseViewPort(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseViewPort(name, sizeof(name)));
}

TEST_F(RendererTest, RenderTargetTest) {
  const char name[] = "test_render_target";
  const YRenderDevice::PixelFormat format =
      static_cast<YRenderDevice::PixelFormat>(123);
  Renderer::RegisterRenderTarget(name, sizeof(name), format,
                                 Renderer::kDimensionType_Absolute, 1.0f,
                                 Renderer::kDimensionType_Absolute, 2.0f);
  Renderer::RegisterRenderTarget(name, sizeof(name), format,
                                 Renderer::kDimensionType_Absolute, 1.0f,
                                 Renderer::kDimensionType_Absolute, 2.0f);

  EXPECT_FALSE(Renderer::ReleaseRenderTarget(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderTarget(name, sizeof(name)));
}

TEST_F(RendererTest, BackBufferNameTest) {
  const char name[] = "test_back_buffer";
  Renderer::RegisterBackBufferName(name, sizeof(name));
  Renderer::RegisterBackBufferName(name, sizeof(name));

  EXPECT_FALSE(Renderer::ReleaseBackBufferName(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseBackBufferName(name, sizeof(name)));
}

TEST_F(RendererTest, RenderPassTest) {
  const char name[] = "test_render_pass";
  const char variant[] = "test_variant";
  YRenderDevice::RenderBlendState blend_state;
  Renderer::RegisterRenderPass(name, sizeof(name),
                               variant, sizeof(variant),
                               blend_state,
                               nullptr, nullptr, 0);
  Renderer::RegisterRenderPass(name, sizeof(name),
                               variant, sizeof(variant),
                               blend_state,
                               nullptr, nullptr, 0);

  EXPECT_FALSE(Renderer::ReleaseRenderPass(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderPass(name, sizeof(name)));
}

TEST_F(RendererTest, BasicActivationTest) {
  const char viewport_name[] = "test";
  const YRenderDevice::ViewPortID viewport_id = 123;
  Renderer::RegisterViewPort(viewport_name, sizeof(viewport_name),
                             Renderer::kDimensionType_Absolute, 1.0f,
                             Renderer::kDimensionType_Percentage, 0.25f,
                             Renderer::kDimensionType_Absolute, 3.0f,
                             Renderer::kDimensionType_Percentage, 0.5f,
                             0.1f, 1.0f);
  YRenderDevice::RenderDeviceMock::ExpectCreateViewPort(viewport_id,
                                                        1, gTestWidth / 4,
                                                        3, gTestHeight / 2,
                                                        0.1f, 1.0f);

  const char render_target_name[] = "test_render_target";
  const YRenderDevice::PixelFormat format =
      static_cast<YRenderDevice::PixelFormat>(456);
  const YRenderDevice::RenderTargetID render_target_id = 12;
  Renderer::RegisterRenderTarget(render_target_name,
                                 sizeof(render_target_name), format,
                                 Renderer::kDimensionType_Absolute, 1.0f,
                                 Renderer::kDimensionType_Percentage, 0.5f);
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderTarget(render_target_id,
                                                            1, gTestWidth / 2,
                                                            format);

  const char render_pass_name[] = "test";
  const char shader_variant[] = "test_variant";
  YRenderDevice::RenderBlendState blend_state;
  const char* render_targets[] = { render_target_name };
  size_t render_target_sizes[] = { sizeof(render_target_name) };
  const YRenderDevice::RenderBlendStateID blend_state_id = 10;
  Renderer::RegisterRenderPass(render_pass_name, sizeof(render_pass_name),
                               shader_variant, sizeof(shader_variant),
                               blend_state,
                               render_targets, render_target_sizes, 1);
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderBlendState(
      blend_state_id, blend_state);

  const char passes_name[] = "test";

  Renderer::ActivateRenderPasses(passes_name, sizeof(passes_name));

  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderBlendState(
      blend_state_id);
  EXPECT_TRUE(Renderer::ReleaseRenderPass(render_pass_name,
                                          sizeof(render_pass_name)));
  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderTarget(render_target_id);
  Renderer::ReleaseRenderTarget(render_target_name, sizeof(render_target_name));
  YRenderDevice::RenderDeviceMock::ExpectReleaseViewPort(viewport_id);
  Renderer::ReleaseViewPort(viewport_name, sizeof(viewport_name));
}

}} // namespace YEngine { namespace YRenderer {
