#include <YCommon/Headers/stdincludes.h>
#include "RenderStateCache.h"

#include <gtest/gtest.h>

#include <YEngine/YRenderDevice/RenderBlendState.h>
#include <YEngine/YRenderDevice/RenderDevice_Mock.h>
#include <YEngine/YRenderDevice/SamplerState.h>

#include "BasicRendererTest.h"

namespace YEngine { namespace YRenderer {

class RenderStateCacheTest : public BasicRendererTest {
 protected:
  RenderStateCacheTest()
    : BasicRendererTest(),
      mCacheBuffer(nullptr) {
  }

  ~RenderStateCacheTest() override {
    delete [] mCacheBuffer;
  }

  void SetUp() override {
    BasicRendererTest::SetUp();

    delete [] mCacheBuffer;
    const size_t buffer_size = RenderStateCache::GetAllocationSize();
    mCacheBuffer = new uint8_t[buffer_size];
    RenderStateCache::Initialize(mCacheBuffer, buffer_size);
  }

  void TearDown() override {
    RenderStateCache::Terminate();
    delete mCacheBuffer;
    mCacheBuffer = nullptr;

    BasicRendererTest::TearDown();
  }

  void* mCacheBuffer;
};

TEST_F(BasicRendererTest, InitializeTearDown) {
}

TEST_F(RenderStateCacheTest, BlendStateCache) {
  YRenderDevice::RenderBlendState blend_state;
  blend_state.source = YRenderDevice::kRenderBlend_SrcColor;

  const uint64_t blend_hash = RenderStateCache::InsertBlendState(blend_state);
  ASSERT_EQ(blend_state, *RenderStateCache::GetBlendState(blend_hash));
}

TEST_F(RenderStateCacheTest, BlendStateID) {
  YRenderDevice::RenderBlendState blend_state;
  const uint64_t blend_hash = RenderStateCache::InsertBlendState(blend_state);
  const YRenderDevice::RenderBlendState* blend_state_pointer =
      RenderStateCache::GetBlendState(blend_hash);
  ASSERT_NE(blend_state_pointer, nullptr);

  const YRenderDevice::RenderBlendStateID kBlendStateID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateRenderBlendState(
      kBlendStateID, *blend_state_pointer);
  ASSERT_EQ(kBlendStateID, RenderStateCache::GetBlendStateID(blend_hash));

  YRenderDevice::RenderDeviceMock::ExpectReleaseRenderBlendState(
      kBlendStateID);
  RenderStateCache::Terminate();
}

TEST_F(RenderStateCacheTest, SamplerStateCache) {
  YRenderDevice::SamplerState sampler_state;
  sampler_state.filter = YRenderDevice::kSamplerFilter_MinMagMipLinear;

  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  ASSERT_EQ(sampler_state, *RenderStateCache::GetSamplerState(sampler_hash));
}

TEST_F(RenderStateCacheTest, SamplerStateID) {
  YRenderDevice::SamplerState sampler_state;
  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  const YRenderDevice::SamplerState* sampler_state_pointer =
      RenderStateCache::GetSamplerState(sampler_hash);
  ASSERT_NE(sampler_state_pointer, nullptr);

  const YRenderDevice::SamplerStateID kSamplerStateID = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateSamplerState(kSamplerStateID,
                                                  *sampler_state_pointer);
  ASSERT_EQ(kSamplerStateID, RenderStateCache::GetSamplerStateID(sampler_hash));

  YRenderDevice::RenderDeviceMock::ExpectReleaseSamplerState(kSamplerStateID);
  RenderStateCache::Terminate();
}

}} // namespace YEngine { namespace YRenderer {

