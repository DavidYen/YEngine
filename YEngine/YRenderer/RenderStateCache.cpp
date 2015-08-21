#include <YCommon/Headers/stdincludes.h>
#include "RenderStateCache.h"

#include <YCommon/YContainers/HashTable.h>
#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YContainers/MemPool.h>
#include <YCommon/YUtils/Hash.h>
#include <YEngine/YRenderDevice/RenderBlendState.h>
#include <YEngine/YRenderDevice/SamplerState.h>

#define BLEND_STATE_SIZE 32
#define SAMPLER_STATE_SIZE 64

#define INVALID_BLEND_STATE static_cast<YRenderDevice::RenderBlendStateID>(-1)
#define INVALID_SAMPLER_STATE static_cast<YRenderDevice::SamplerStateID>(-1)

namespace YEngine { namespace YRenderer {

namespace {
  YCommon::YContainers::MemBuffer gMemBuffer;

  struct BlendStateInternal {
    YRenderDevice::RenderBlendState mBlendState;
    YRenderDevice::RenderBlendStateID mBlendStateID;
  };
  struct SamplerStateInternal {
    YRenderDevice::SamplerState mSamplerState;
    YRenderDevice::SamplerStateID mSamplerStateID;
  };

  YCommon::YContainers::TypedMemPool<BlendStateInternal>
      gBlendStateCache;
  YCommon::YContainers::TypedMemPool<SamplerStateInternal>
      gSamplerStateCache;

  YCommon::YContainers::FullTypedHashTable<YRenderDevice::RenderBlendState,
                                           uint32_t> gBlendStateHash;
  YCommon::YContainers::FullTypedHashTable<YRenderDevice::SamplerState,
                                           uint32_t> gSamplerStateHash;
}

#define INITIALIZE_CACHE(MEMPOOL, POOLSIZE, NAME) \
  do { \
    const size_t pool_buffer_size = \
        MEMPOOL.GetAllocationSize(POOLSIZE); \
    void* pool_buffer = gMemBuffer.Allocate(pool_buffer_size, 128); \
    YASSERT(pool_buffer, \
            "Not enough space to allocate " NAME " cache."); \
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
            static_cast<uint32_t>(table_buffer_size)); \
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
      YRenderDevice::RenderDevice::ReleaseSamplerState(
          sampler_state.mSamplerStateID);
    }
  }
  gSamplerStateCache.Reset();

  const uint32_t num_blend_states = gBlendStateCache.GetNumIndexesUsed();
  for (uint32_t i = 0; i < num_blend_states; ++i) {
    BlendStateInternal& blend_state = gBlendStateCache[i];
    if (blend_state.mBlendStateID != INVALID_BLEND_STATE) {
      YRenderDevice::RenderDevice::ReleaseRenderBlendState(
          blend_state.mBlendStateID);
    }
  }
  gBlendStateCache.Reset();
}

uint64_t RenderStateCache::InsertBlendState(
    const YRenderDevice::RenderBlendState& blend_state) {
  const uint64_t blend_hash =
      YCommon::YUtils::Hash::Hash64(&blend_state, sizeof(blend_state));
  uint32_t* cached_blend_state = gBlendStateHash.GetValue(blend_hash);
  if (nullptr == cached_blend_state) {
    BlendStateInternal new_blend_state = { blend_state, INVALID_BLEND_STATE };
    const uint32_t index = gBlendStateCache.Insert(new_blend_state);
    gBlendStateHash.Insert(blend_hash, index);
  }
  return blend_hash;
}

uint64_t RenderStateCache::InsertSamplerState(
    const YRenderDevice::SamplerState& sampler_state) {
  const uint64_t sampler_hash =
      YCommon::YUtils::Hash::Hash64(&sampler_state, sizeof(sampler_state));
  uint32_t* cached_sampler_state = gSamplerStateHash.GetValue(sampler_hash);
  if (nullptr == cached_sampler_state) {
    SamplerStateInternal new_state = { sampler_state, INVALID_SAMPLER_STATE };
    const uint32_t index = gSamplerStateCache.Insert(new_state);
    gBlendStateHash.Insert(sampler_hash, index);
  }
  return sampler_hash;
}

YRenderDevice::RenderBlendStateID RenderStateCache::GetBlendStateID(
    uint64_t blend_state_hash) {
  uint32_t* index = gBlendStateHash.GetValue(blend_state_hash);
  YASSERT(index, "Invalid Blend State Hash.");

  BlendStateInternal& blend_state = gBlendStateCache[*index];
  if (blend_state.mBlendStateID == INVALID_BLEND_STATE) {
    blend_state.mBlendStateID =
        YRenderDevice::RenderDevice::CreateRenderBlendState(
            blend_state.mBlendState);
  }
  return blend_state.mBlendStateID;
}

YRenderDevice::SamplerStateID RenderStateCache::GetSamplerStateID(
    uint64_t sampler_state_hash) {
  uint32_t* index = gSamplerStateHash.GetValue(sampler_state_hash);
  YASSERT(index, "Invalid Blend State Hash.");

  SamplerStateInternal& sampler_state = gSamplerStateCache[*index];
  if (sampler_state.mSamplerStateID == INVALID_SAMPLER_STATE) {
    sampler_state.mSamplerStateID =
        YRenderDevice::RenderDevice::CreateSamplerState(
            sampler_state.mSamplerState);
  }
  return sampler_state.mSamplerStateID;
}

}} // namespace YEngine { namespace YRenderer {
