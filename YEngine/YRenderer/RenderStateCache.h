#ifndef YENGINE_YRENDERER_RENDERSTATECACHE_H
#define YENGINE_YRENDERER_RENDERSTATECACHE_H

#include <YEngine/YRenderDevice/RenderDevice.h>

namespace YEngine { namespace YRenderDevice {
  struct RenderBlendState;
  struct SamplerState;
}} // namespace YEngine { namespace YRenderDevice {

namespace YEngine { namespace YRenderer {

namespace RenderStateCache {
  size_t GetAllocationSize();

  void Initialize(void* buffer, size_t buffer_size);
  void Terminate();

  uint64_t InsertBlendState(const YRenderDevice::RenderBlendState& blend_state);
  uint64_t InsertSamplerState(const YRenderDevice::SamplerState& sampler_state);

  YRenderDevice::RenderBlendStateID GetBlendStateID(uint64_t blend_state_hash);
  YRenderDevice::SamplerStateID GetSamplerStateID(uint64_t sampler_state_hash);
}

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERSTATECACHE_H
