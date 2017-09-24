#ifndef YENGINE_RENDERER_RENDER_STATE_CACHE_H
#define YENGINE_RENDERER_RENDER_STATE_CACHE_H

#include <stdint.h>

#include "yengine/render_device/render_device.h"

namespace yengine { namespace render_device {
  struct RenderBlendState;
  struct SamplerState;
}} // namespace yengine { namespace render_device {

namespace yengine { namespace renderer {

namespace RenderStateCache {
  size_t GetAllocationSize();

  void Initialize(void* buffer, size_t buffer_size);
  void Terminate();

  uint64_t InsertBlendState(const render_device::RenderBlendState& blend_state);
  uint64_t InsertSamplerState(const render_device::SamplerState& sampler_state);

  const render_device::RenderBlendState* GetBlendState(uint64_t blend_hash);
  const render_device::SamplerState* GetSamplerState(uint64_t sampler_hash);

  render_device::RenderBlendStateID GetBlendStateID(uint64_t blend_state_hash);
  render_device::SamplerStateID GetSamplerStateID(uint64_t sampler_state_hash);
}

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_RENDER_STATE_CACHE_H
