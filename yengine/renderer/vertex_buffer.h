#ifndef YENGINE_RENDERER_VERTEX_BUFFER_H
#define YENGINE_RENDERER_VERTEX_BUFFER_H

#include "yengine/render_device/render_device.h"
#include "yengine/render_device/vertex_decl_element.h"

#define MAX_VERTEX_ELEMENTS 8

namespace yengine { namespace renderer {

struct RenderDeviceState;

class VertexDecl {
 public:
  VertexDecl(const render_device::VertexDeclElement* elements,
             uint8_t num_elements);
  void Release();
  void Activate(RenderDeviceState& device_state);

  const render_device::VertexDeclElement* GetVertexDeclElements() const {
    return mVertexElements;
  }
  const uint8_t GetNumVertexElements() const { return mNumVertexElements; }

 private:
  render_device::VertexDeclElement mVertexElements[MAX_VERTEX_ELEMENTS];
  uint8_t mNumVertexElements;
  render_device::VertexDeclID mVertexDeclID;
};

class IndexBuffer {
 public:
  IndexBuffer();
  void Release();

  void Initialize(render_device::UsageType usage, uint32_t count);
  uint32_t GetFillCount() const { return mFillCount; }

  void Fill(const uint16_t* data, uint32_t num_ints);
  void FillMulti(uint32_t arrays,
                 const uint16_t* const* datas,
                 const uint32_t* num_ints,
                 const uint16_t* float_index_offsets);
  void Activate(RenderDeviceState& device_state);

 private:
  bool mDirty;
  uint8_t mActiveIndex;
  render_device::UsageType mUsageType;
  uint32_t mTotalCount;
  uint32_t mFillCount;
  render_device::IndexBufferID mIndexBufferIDs[2];
};

class VertexBuffer {
 public:
  VertexBuffer();
  void Release();

  void Initialize(render_device::UsageType usage,
                  uint32_t stride,
                  uint32_t count);
  uint32_t GetFillSize() const { return mFillSize; }

  void Fill(const void* data, uint32_t data_size);
  void FillMulti(uint32_t arrays,
                 const float* const* floats,
                 const uint32_t* float_counts);
  void FillMulti(uint32_t arrays,
                 const void* const* datas,
                 const uint32_t* data_sizes,
                 uint32_t data_stride = 1);
  void Activate(RenderDeviceState& device_state);

 private:
  bool mDirty;
  uint8_t mActiveIndex;
  render_device::UsageType mUsageType;
  uint32_t mStride;
  uint32_t mTotalSize;
  uint32_t mFillSize;
  render_device::VertexBufferID mVertexBufferIDs[2];
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_VERTEX_BUFFER_H
