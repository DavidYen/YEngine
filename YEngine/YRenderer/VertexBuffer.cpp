#include <YCommon/Headers/stdincludes.h>
#include "VertexBuffer.h"

#include "RenderDeviceState.h"

#define INVALID_INDEX_BUFFER static_cast<YRenderDevice::IndexBufferID>(-1)
#define INVALID_VERTEX_BUFFER static_cast<YRenderDevice::VertexBufferID>(-1)

namespace YEngine { namespace YRenderer {

IndexBuffer::IndexBuffer()
  : mDirty(false),
    mActiveIndex(0),
    mUsageType(YRenderDevice::kUsageType_Invalid),
    mTotalCount(0),
    mFillCount(0) {
  for (int i = 0; i < ARRAY_SIZE(mIndexBufferIDs); ++i) {
    mIndexBufferIDs[i] = INVALID_INDEX_BUFFER;
  }
}

void IndexBuffer::Release() {
  for (int i = 0; i < ARRAY_SIZE(mIndexBufferIDs); ++i) {
    if (mIndexBufferIDs[i] != INVALID_INDEX_BUFFER) {
      YRenderDevice::RenderDevice::ReleaseIndexBuffer(mIndexBufferIDs[i]);
      mIndexBufferIDs[i] = INVALID_INDEX_BUFFER;
    }
  }
}

void IndexBuffer::Initialize(YRenderDevice::UsageType usage, uint32_t count) {
  if (mUsageType != usage || mTotalCount != count ) {
    mUsageType = usage;
    mTotalCount = count;
    mDirty = true;
  }
}

void IndexBuffer::Fill(const uint16_t* data, uint32_t num_ints) {
  FillMulti(1, &data, &num_ints);
}

void IndexBuffer::FillMulti(uint32_t arrays,
                            const uint16_t* const* datas,
                            const uint32_t* num_ints) {
  YASSERT(mUsageType != YRenderDevice::kUsageType_Invalid,
          "Index buffer has not been initialized.");

  mFillCount = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    mFillCount += num_ints[i];
  }

  YASSERT(mFillCount <= mTotalCount,
          "Index buffer total size (%u) exceeded: %u",
          mTotalCount, mFillCount);

  if (mDirty) {
    Release();
    mDirty = false;
  }

  mActiveIndex = !mActiveIndex;
  if (mIndexBufferIDs[mActiveIndex] == INVALID_INDEX_BUFFER) {
    mIndexBufferIDs[mActiveIndex] =
        YRenderDevice::RenderDevice::CreateIndexBuffer(mUsageType, mTotalCount);
  }

  uint32_t current_offset = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    const uint16_t* data = datas[i];
    const uint32_t count = num_ints[i];
    YRenderDevice::RenderDevice::FillIndexBuffer(mIndexBufferIDs[mActiveIndex],
                                                 count,
                                                 data,
                                                 sizeof(data[0]) * count,
                                                 current_offset);
    current_offset += count;
  }
}

void IndexBuffer::Activate(RenderDeviceState& device_state) {
  const YRenderDevice::IndexBufferID id = mIndexBufferIDs[mActiveIndex];
  YASSERT(id != INVALID_INDEX_BUFFER,
          "Index buffer has not been filled yet.");
  device_state.ActivateIndexStream(id);
}

VertexBuffer::VertexBuffer()
  : mDirty(false),
    mActiveIndex(0),
    mUsageType(YRenderDevice::kUsageType_Invalid),
    mStride(1),
    mTotalSize(0),
    mFillSize(0) {
  for (int i = 0; i < ARRAY_SIZE(mVertexBufferIDs); ++i) {
    mVertexBufferIDs[i] = INVALID_VERTEX_BUFFER;
  }
}

void VertexBuffer::Release() {
  for (int i = 0; i < ARRAY_SIZE(mVertexBufferIDs); ++i) {
    if (mVertexBufferIDs[i] != INVALID_VERTEX_BUFFER) {
      YRenderDevice::RenderDevice::ReleaseVertexBuffer(mVertexBufferIDs[i]);
      mVertexBufferIDs[i] = INVALID_VERTEX_BUFFER;
    }
  }
}

void VertexBuffer::Initialize(YRenderDevice::UsageType usage,
                              uint32_t stride,
                              uint32_t count) {
  YASSERT(stride > 0, "Invalid vertex buffer stride: %u", stride);

  const uint32_t total_size = stride * count;
  if (mUsageType != usage || mStride != stride || mTotalSize != total_size) {
    mUsageType = usage;
    mStride = stride;
    mTotalSize = total_size;
    mDirty = true;
  }
}

void VertexBuffer::Fill(const float* data, uint32_t num_floats) {
  FillMulti(1, &data, &num_floats);
}

void VertexBuffer::FillMulti(uint32_t arrays,
                             const float* const* datas,
                             const uint32_t* float_counts) {
  YASSERT(mUsageType != YRenderDevice::kUsageType_Invalid,
          "Vertex buffer has not been initialized.");

  mFillSize = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    mFillSize += float_counts[i] * sizeof(float);
  }

  YASSERT(mFillSize <= mTotalSize,
          "Index buffer total size (%u) exceeded: %u",
          mTotalSize, mFillSize);

  if (mDirty) {
    Release();
    mDirty = false;
  }

  mActiveIndex = !mActiveIndex;
  if (mVertexBufferIDs[mActiveIndex] == INVALID_VERTEX_BUFFER) {
    mVertexBufferIDs[mActiveIndex] =
        YRenderDevice::RenderDevice::CreateVertexBuffer(mUsageType,
                                                        mStride,
                                                        mTotalSize / mStride);
  }

  const YRenderDevice::VertexBufferID vertex_buffer_id =
      mVertexBufferIDs[mActiveIndex];
  uint32_t current_offset = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    const float* data = datas[i];
    const uint32_t data_size = float_counts[i] * sizeof(float);

    YASSERT(data_size % mStride == 0,
            "Buffer data size (%u) is not a multiple of the stride (%u).",
            data_size, mStride);
    const uint32_t count = data_size / mStride;
    YRenderDevice::RenderDevice::FillVertexBuffer(vertex_buffer_id,
                                                  count,
                                                  data,
                                                  data_size,
                                                  current_offset);
    current_offset += count;
  }
}

void VertexBuffer::Activate(RenderDeviceState& device_state) {
  const YRenderDevice::VertexBufferID id = mVertexBufferIDs[mActiveIndex];
  YASSERT(id != INVALID_VERTEX_BUFFER,
          "Vertex buffer has not been filled yet.");
  device_state.ActivateVertexStream(mVertexBufferIDs[mActiveIndex]);
}

}} // namespace YEngine { namespace YRenderer {
