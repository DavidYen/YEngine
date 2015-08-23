#ifndef YENGINE_YRENDERER_BASICRENDERERTEST_H
#define YENGINE_YRENDERER_BASICRENDERERTEST_H

#include <gtest/gtest.h>

#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YEngine/YRenderDevice/RenderDevice.h>

#include "RenderStateCache.h"

#define BASIC_RENDERER_WIDTH 128
#define BASIC_RENDERER_HEIGHT 128

namespace YEngine { namespace YRenderer {

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
    YRenderDevice::RenderDevice::Initialize(mHandle, BASIC_RENDERER_WIDTH,
                                            BASIC_RENDERER_HEIGHT, 
                                            mMemBuffer.Allocate(renderer_size),
                                            renderer_size);

    RenderStateCache::Initialize(mMemBuffer.Allocate(cache_size), cache_size);
  }

  void TearDown() override {
    RenderStateCache::Terminate();
    YRenderDevice::RenderDevice::Terminate();

    mMemBuffer.Reset();

    delete [] mBuffer;
    mBuffer = nullptr;
  }

  void* mBuffer;
  YCommon::YContainers::MemBuffer mMemBuffer;
  YCommon::YPlatform::PlatformHandle mHandle;
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_BASICRENDERERTEST_H