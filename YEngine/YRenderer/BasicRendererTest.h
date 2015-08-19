#ifndef YENGINE_YRENDERER_BASICRENDERERTEST_H
#define YENGINE_YRENDERER_BASICRENDERERTEST_H

#include <gtest/gtest.h>

#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YEngine/YRenderDevice/RenderDevice.h>

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
    mBuffer = new uint8_t[1024 * 10];
    mMemBuffer.Init(mBuffer, 10240);
    YRenderDevice::RenderDevice::Initialize(mHandle, BASIC_RENDERER_WIDTH,
                                            BASIC_RENDERER_HEIGHT, 
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

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_BASICRENDERERTEST_H