#include "yengine/renderer/render_state_cache.h"

#include "ycommon/containers/hash_table.h"
#include "ycommon/containers/mem_buffer.h"
#include "ycommon/containers/mem_pool.h"
#include "ycommon/utils/assert.h"
#include "ycommon/utils/hash.h"
#include "yengine/render_device/render_blend_state.h"
#include "yengine/render_device/sampler_state.h"

#define BLEND_STATE_SIZE 32
#define SAMPLER_STATE_SIZE 64

#define INVALID_BLEND_STATE static_cast<render_device::RenderBlendStateID>(-1)
#define INVALID_SAMPLER_STATE static_cast<render_device::SamplerStateID>(-1)

namespace yengine { namespace renderer {

namespace {
  ycommon::containers::MemBuffer gMemBuffer;

  struct BlendStateInternal {
    render_device::RenderBlendState mBlendState;
    render_device::RenderBlendStateID mBlendStateID;
  };
  struct SamplerStateInternal {
    render_device::SamplerState mSamplerState;
    render_device::SamplerStateID mSamplerStateID;
  };

  ycommon::containers::TypedMemPool<BlendStateInternal>
      gBlendStateCache;
  ycommon::containers::TypedMemPool<SamplerStateInternal>
      gSamplerStateCache;

  ycommon::containers::FullTypedHashTable<render_device::RenderBlendState,
                                           uint32_t> gBlendStateHash;
  ycommon::containers::FullTypedHashTable<render_device::SamplerState,
                                           uint32_t> gSamplerStateHash;
}

#define INITIALIZE_CACHE(MEMPOOL, POOLSIZE, NAME) \
  do { \
    const size_t pool_buffer_size = \
        MEMPOOL.GetAllocationSize(POOLSIZE); \
    void* pool_buffer = gMemBuffer.Allocate(pool_buffer_size, 128); \
    YASSERT(pool_buffer, \
            "Not enough space to allocate " NAME " cache.\n" \
            "  Free Space:   %u\n" \
            "  Needed Space: %u", \
            static_cast<uint32_t>(gMemBuffer.FreeSpace()), \
            static_cast<uint32_t>(pool_buffer_size + 128)); \
    MEMPOOL.Init(pool_buffer, pool_buffer_size, POOLSIZE); \
  } while(0)

#define INITIALIZE_HASH(HASHTABLE, TABLESIZE, NAME) \
  do { \
    const size_t table_buffer_size = \
        HASHTABLE.GetAllocationSize(TABLESIZE); \
    void* table_buffer = gMemBuffer.Allocate(table_buffer_size, 128); \
    YASSERT(table_buffer, \
            "Not enough space to allocate " NAME " hash.\n" \
            "  Free Space:   %u\n" \
            "  Needed Space: %u", \
            static_cast<uint32_t>(gMemBuffer.FreeSpace()), \
            static_cast<uint32_t>(table_buffer_size + 128)); \
    HASHTABLE.Init(table_buffer, table_buffer_size, TABLESIZE); \
  } while(0)

size_t RenderStateCache::GetAllocationSize() {
  return gBlendStateCache.GetAllocationSize(BLEND_STATE_SIZE) + 128 +
         gSamplerStateCache.GetAllocationSize(SAMPLER_STATE_SIZE) + 128 +
         gBlendStateHash.GetAllocationSize(BLEND_STATE_SIZE * 2) + 128 +
         gSamplerStateHash.GetAllocationSize(SAMPLER_STATE_SIZE * 2) + 128;
}

void RenderStateCache::Initialize(void* buffer, size_t buffer_size) {
  gMemBuffer.Init(buffer, buffer_size);

  INITIALIZE_CACHE(gBlendStateCache, BLEND_STATE_SIZE, "Blend State");
  INITIALIZE_CACHE(gSamplerStateCache, SAMPLER_STATE_SIZE, "Sampler State");

  INITIALIZE_HASH(gBlendStateHash, BLEND_STATE_SIZE * 2, "Blend State");
  INITIALIZE_HASH(gSamplerStateHash, SAMPLER_STATE_SIZE * 2, "Sampler State");
}

void RenderStateCache::Terminate() {
  gSamplerStateHash.Reset();
  gBlendStateHash.Reset();

  const uint32_t num_sampler_state = gSamplerStateCache.GetNumIndexesUsed();
  for (uint32_t i = 0; i < num_sampler_state; ++i) {
    SamplerStateInternal& sampler_state = gSamplerStateCache[i];
    if (sampler_state.mSamplerStateID != INVALID_SAMPLER_STATE) {
      render_device::RenderDevice::ReleaseSamplerState(
          sampler_state.mSamplerStateID);
    }
  }
  gSamplerStateCache.Reset();

  const uint32_t num_blend_states = gBlendStateCache.GetNumIndexesUsed();
  for (uint32_t i = 0; i < num_blend_states; ++i) {
    BlendStateInternal& blend_state = gBlendStateCache[i];
    if (blend_state.mBlendStateID != INVALID_BLEND_STATE) {
      render_device::RenderDevice::ReleaseRenderBlendState(
          blend_state.mBlendStateID);
    }
  }
  gBlendStateCache.Reset();
}

uint64_t RenderStateCache::InsertBlendState(
    const render_device::RenderBlendState& blend_state) {
  const uint64_t blend_hash =
      ycommon::utils::Hash::Hash64(&blend_state, sizeof(blend_state));
  uint32_t* cached_blend_state = gBlendStateHash.GetValue(blend_hash);
  if (nullptr == cached_blend_state) {
    BlendStateInternal new_blend_state = { blend_state, INVALID_BLEND_STATE };
    const uint32_t index = gBlendStateCache.Insert(new_blend_state);
    gBlendStateHash.Insert(blend_hash, index);
  }
  return blend_hash;
}

uint64_t RenderStateCache::InsertSamplerState(
    const render_device::SamplerState& sampler_state) {
  const uint64_t sampler_hash =
      ycommon::utils::Hash::Hash64(&sampler_state, sizeof(sampler_state));
  uint32_t* cached_sampler_state = gSamplerStateHash.GetValue(sampler_hash);
  if (nullptr == cached_sampler_state) {
    SamplerStateInternal new_state = { sampler_state, INVALID_SAMPLER_STATE };
    const uint32_t index = gSamplerStateCache.Insert(new_state);
    gSamplerStateHash.Insert(sampler_hash, index);
  }
  return sampler_hash;
}

const render_device::RenderBlendState* RenderStateCache::GetBlendState(
    uint64_t blend_hash) {
  uint32_t* index = gBlendStateHash.GetValue(blend_hash);
  return index ? &gBlendStateCache[*index].mBlendState : nullptr;
}

const render_device::SamplerState* RenderStateCache::GetSamplerState(
    uint64_t sampler_hash) {
  uint32_t* index = gSamplerStateHash.GetValue(sampler_hash);
  return index ? &gSamplerStateCache[*index].mSamplerState : nullptr;
}

render_device::RenderBlendStateID RenderStateCache::GetBlendStateID(
    uint64_t blend_state_hash) {
  uint32_t* index = gBlendStateHash.GetValue(blend_state_hash);
  YASSERT(index, "Invalid Blend Hash: 0x%08X%08X.",
          static_cast<uint32_t>(blend_state_hash >> 32),
          static_cast<uint32_t>(blend_state_hash));

  BlendStateInternal& blend_state = gBlendStateCache[*index];
  if (blend_state.mBlendStateID == INVALID_BLEND_STATE) {
    blend_state.mBlendStateID =
        render_device::RenderDevice::CreateRenderBlendState(
            blend_state.mBlendState);
  }
  return blend_state.mBlendStateID;
}

render_device::SamplerStateID RenderStateCache::GetSamplerStateID(
    uint64_t sampler_state_hash) {
  uint32_t* index = gSamplerStateHash.GetValue(sampler_state_hash);
  YASSERT(index, "Invalid Sampler Hash: 0x%08X%08X.",
          static_cast<uint32_t>(sampler_state_hash >> 32),
          static_cast<uint32_t>(sampler_state_hash));

  SamplerStateInternal& sampler_state = gSamplerStateCache[*index];
  if (sampler_state.mSamplerStateID == INVALID_SAMPLER_STATE) {
    sampler_state.mSamplerStateID =
        render_device::RenderDevice::CreateSamplerState(
            sampler_state.mSamplerState);
  }
  return sampler_state.mSamplerStateID;
}

}} // namespace yengine { namespace renderer {
