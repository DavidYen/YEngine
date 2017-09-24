#include "yengine/renderer/vertex_buffer.h"

#include <string.h>

#include "ycommon/utils/assert.h"
#include "yengine/renderer/render_device_state.h"

#define INVALID_VERTEX_DECL static_cast<render_device::VertexDeclID>(-1)
#define INVALID_INDEX_BUFFER static_cast<render_device::IndexBufferID>(-1)
#define INVALID_VERTEX_BUFFER static_cast<render_device::VertexBufferID>(-1)

namespace yengine { namespace renderer {

VertexDecl::VertexDecl(const render_device::VertexDeclElement* elements,
                       uint8_t num_elements)
  : mNumVertexElements(num_elements),
    mVertexDeclID(INVALID_VERTEX_DECL) {
  YASSERT(num_elements < MAX_VERTEX_ELEMENTS,
          "Maximum vertex elements (%d) exceeded: %d",
          static_cast<int>(MAX_VERTEX_ELEMENTS),
          static_cast<int>(num_elements));
  memset(mVertexElements, 0, sizeof(mVertexElements));
  memcpy(mVertexElements, elements, num_elements * sizeof(elements[0]));
}

void VertexDecl::Release() {
  if (mVertexDeclID != INVALID_VERTEX_DECL) {
    render_device::RenderDevice::ReleaseVertexDeclaration(mVertexDeclID);
  }
}

void VertexDecl::Activate(RenderDeviceState& device_state) {
  if (mVertexDeclID == INVALID_VERTEX_DECL) {
    mVertexDeclID =
        render_device::RenderDevice::CreateVertexDeclaration(
            mVertexElements, mNumVertexElements);
  }

  device_state.ActivateVertexDecl(mVertexDeclID);
}

IndexBuffer::IndexBuffer()
  : mDirty(false),
    mActiveIndex(0),
    mUsageType(render_device::kUsageType_Invalid),
    mTotalCount(0),
    mFillCount(0) {
  for (int i = 0; i < ARRAY_SIZE(mIndexBufferIDs); ++i) {
    mIndexBufferIDs[i] = INVALID_INDEX_BUFFER;
  }
}

void IndexBuffer::Release() {
  for (int i = 0; i < ARRAY_SIZE(mIndexBufferIDs); ++i) {
    if (mIndexBufferIDs[i] != INVALID_INDEX_BUFFER) {
      render_device::RenderDevice::ReleaseIndexBuffer(mIndexBufferIDs[i]);
      mIndexBufferIDs[i] = INVALID_INDEX_BUFFER;
    }
  }
  mDirty = false;
}

void IndexBuffer::Initialize(render_device::UsageType usage, uint32_t count) {
  if (mUsageType != usage || mTotalCount != count ) {
    mUsageType = usage;
    mTotalCount = count;
    mDirty = true;
  }
}

void IndexBuffer::Fill(const uint16_t* data, uint32_t num_ints) {
  uint16_t index_offsets = 0;
  FillMulti(1, &data, &num_ints, &index_offsets);
}

void IndexBuffer::FillMulti(uint32_t arrays,
                            const uint16_t* const* datas,
                            const uint32_t* num_ints,
                            const uint16_t* float_index_offsets) {
  YASSERT(mUsageType != render_device::kUsageType_Invalid,
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
  }

  mActiveIndex = !mActiveIndex;
  if (mIndexBufferIDs[mActiveIndex] == INVALID_INDEX_BUFFER) {
    mIndexBufferIDs[mActiveIndex] =
        render_device::RenderDevice::CreateIndexBuffer(mUsageType, mTotalCount);
  }

  uint32_t current_offset = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    const uint16_t* data = datas[i];
    const uint32_t count = num_ints[i];
    render_device::RenderDevice::FillIndexBuffer(mIndexBufferIDs[mActiveIndex],
                                                 count,
                                                 data,
                                                 sizeof(data[0]) * count,
                                                 float_index_offsets[i],
                                                 current_offset);
    current_offset += count;
  }
}

void IndexBuffer::Activate(RenderDeviceState& device_state) {
  const render_device::IndexBufferID id = mIndexBufferIDs[mActiveIndex];
  YASSERT(id != INVALID_INDEX_BUFFER,
          "Index buffer has not been filled yet.");
  device_state.ActivateIndexStream(id);
}

VertexBuffer::VertexBuffer()
  : mDirty(false),
    mActiveIndex(0),
    mUsageType(render_device::kUsageType_Invalid),
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
      render_device::RenderDevice::ReleaseVertexBuffer(mVertexBufferIDs[i]);
      mVertexBufferIDs[i] = INVALID_VERTEX_BUFFER;
    }
  }
  mDirty = false;
}

void VertexBuffer::Initialize(render_device::UsageType usage,
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

void VertexBuffer::Fill(const void* data, uint32_t data_size) {
  FillMulti(1, &data, &data_size);
}

void VertexBuffer::FillMulti(uint32_t arrays,
                             const float* const* floats,
                             const uint32_t* float_counts) {

  FillMulti(arrays, reinterpret_cast<const void* const*>(floats),
            float_counts, sizeof(floats[0][0]));
}

void VertexBuffer::FillMulti(uint32_t arrays,
                             const void* const* datas,
                             const uint32_t* data_sizes,
                             uint32_t data_stride) {
  YASSERT(mUsageType != render_device::kUsageType_Invalid,
          "Vertex buffer has not been initialized.");

  mFillSize = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    mFillSize += data_sizes[i] * data_stride;
  }

  YASSERT(mFillSize <= mTotalSize,
          "Vertex buffer total size (%u) exceeded: %u",
          mTotalSize, mFillSize);

  if (mDirty) {
    Release();
  }

  mActiveIndex = !mActiveIndex;
  if (mVertexBufferIDs[mActiveIndex] == INVALID_VERTEX_BUFFER) {
    mVertexBufferIDs[mActiveIndex] =
        render_device::RenderDevice::CreateVertexBuffer(mUsageType,
                                                        mStride,
                                                        mTotalSize / mStride);
  }

  const render_device::VertexBufferID vertex_buffer_id =
      mVertexBufferIDs[mActiveIndex];

  uint32_t current_offset = 0;
  for (uint32_t i = 0; i < arrays; ++i) {
    const void* data = datas[i];
    const uint32_t data_size = data_sizes[i] * data_stride;

    YASSERT(data_size % mStride == 0,
            "Buffer data size (%u) is not a multiple of the stride (%u).",
            data_size, mStride);
    const uint32_t count = data_size / mStride;
    render_device::RenderDevice::FillVertexBuffer(vertex_buffer_id,
                                                  count,
                                                  data,
                                                  data_size,
                                                  current_offset);
    current_offset += count;
  }
}

void VertexBuffer::Activate(RenderDeviceState& device_state) {
  const render_device::VertexBufferID id = mVertexBufferIDs[mActiveIndex];
  YASSERT(id != INVALID_VERTEX_BUFFER,
          "Vertex buffer has not been filled yet.");
  device_state.ActivateVertexStream(mVertexBufferIDs[mActiveIndex]);
}

}} // namespace yengine { namespace renderer {
