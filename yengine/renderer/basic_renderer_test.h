#ifndef YENGINE_RENDERER_BASIC_RENDERER_TEST_H
#define YENGINE_RENDERER_BASIC_RENDERER_TEST_H

#include <gtest/gtest.h>

#include "ycommon/containers/mem_buffer.h"
#include "ycommon/platform/platform_handle.h"
#include "yengine/render_device/render_device.h"
#include "yengine/renderer/render_state_cache.h"

#define BASIC_RENDERER_WIDTH 128
#define BASIC_RENDERER_HEIGHT 128

namespace yengine { namespace renderer {

class BasicRendererTest : public ::testing::Test {
 protected:
  BasicRendererTest()
    : mBuffer(nullptr) {
    memset(&mHandle, 0, sizeof(mHandle));
  }

  ~BasicRendererTest() override {
    delete [] mBuffer;
  }

  void SetUp() override {
    delete [] mBuffer;
    const size_t renderer_size = 1024 * 10;
    const size_t cache_size = RenderStateCache::GetAllocationSize();
    const size_t total_size = renderer_size + cache_size;

    mBuffer = new uint8_t[total_size];
    mMemBuffer.Init(mBuffer, total_size);
    render_device::RenderDevice::Initialize(mHandle, BASIC_RENDERER_WIDTH,
                                            BASIC_RENDERER_HEIGHT, 
                                            mMemBuffer.Allocate(renderer_size),
                                            renderer_size);

    RenderStateCache::Initialize(mMemBuffer.Allocate(cache_size), cache_size);
  }

  void TearDown() override {
    RenderStateCache::Terminate();
    render_device::RenderDevice::Terminate();

    mMemBuffer.Reset();

    delete [] mBuffer;
    mBuffer = nullptr;
  }

  void* mBuffer;
  ycommon::containers::MemBuffer mMemBuffer;
  ycommon::platform::PlatformHandle mHandle;
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_BASIC_RENDERER_TEST_H
