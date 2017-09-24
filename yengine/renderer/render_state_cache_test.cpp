#include "yengine/renderer/render_state_cache.h"

#include <gtest/gtest.h>

#include "yengine/render_device/render_blend_state.h"
#include "yengine/render_device/render_device_mock.h"
#include "yengine/render_device/sampler_state.h"
#include "yengine/renderer/basic_renderer_test.h"

namespace yengine { namespace renderer {

class RenderStateCacheTest : public BasicRendererTest {};

TEST_F(BasicRendererTest, InitializeTearDown) {
}

TEST_F(RenderStateCacheTest, BlendStateCache) {
  render_device::RenderBlendState blend_state;
  blend_state.source = render_device::kRenderBlend_SrcColor;

  const uint64_t blend_hash = RenderStateCache::InsertBlendState(blend_state);
  ASSERT_EQ(blend_state, *RenderStateCache::GetBlendState(blend_hash));
}

TEST_F(RenderStateCacheTest, BlendStateID) {
  render_device::RenderBlendState blend_state;
  const uint64_t blend_hash = RenderStateCache::InsertBlendState(blend_state);
  const render_device::RenderBlendState* blend_state_pointer =
      RenderStateCache::GetBlendState(blend_hash);
  ASSERT_NE(blend_state_pointer, nullptr);

  const render_device::RenderBlendStateID kBlendStateID = 123;
  render_device::RenderDeviceMock::ExpectCreateRenderBlendState(
      kBlendStateID, *blend_state_pointer);
  ASSERT_EQ(kBlendStateID, RenderStateCache::GetBlendStateID(blend_hash));

  render_device::RenderDeviceMock::ExpectReleaseRenderBlendState(
      kBlendStateID);
  RenderStateCache::Terminate();
}

TEST_F(RenderStateCacheTest, SamplerStateCache) {
  render_device::SamplerState sampler_state;
  sampler_state.filter = render_device::kSamplerFilter_MinMagMipLinear;

  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  ASSERT_EQ(sampler_state, *RenderStateCache::GetSamplerState(sampler_hash));
}

TEST_F(RenderStateCacheTest, SamplerStateID) {
  render_device::SamplerState sampler_state;
  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  const render_device::SamplerState* sampler_state_pointer =
      RenderStateCache::GetSamplerState(sampler_hash);
  ASSERT_NE(sampler_state_pointer, nullptr);

  const render_device::SamplerStateID kSamplerStateID = 234;
  render_device::RenderDeviceMock::ExpectCreateSamplerState(
      kSamplerStateID, *sampler_state_pointer);
  ASSERT_EQ(kSamplerStateID, RenderStateCache::GetSamplerStateID(sampler_hash));

  render_device::RenderDeviceMock::ExpectReleaseSamplerState(kSamplerStateID);
  RenderStateCache::Terminate();
}

}} // namespace yengine { namespace renderer {

