#ifndef YENGINE_YRENDERER_VERTEXBUFFER_H
#define YENGINE_YRENDERER_VERTEXBUFFER_H

#include <YEngine/YRenderDevice/RenderDevice.h>
#include <YEngine/YRenderDevice/VertexDeclElement.h>

#define MAX_VERTEX_ELEMENTS 8

namespace YEngine { namespace YRenderer {

struct RenderDeviceState;

class VertexDecl {
 public:
  VertexDecl(const YRenderDevice::VertexDeclElement* elements,
             uint8_t num_elements);
  void Release();
  void Activate(RenderDeviceState& device_state);

  const YRenderDevice::VertexDeclElement* GetVertexDeclElements() const {
    return mVertexElements;
  }
  const uint8_t GetNumVertexElements() const { return mNumVertexElements; }

 private:
  YRenderDevice::VertexDeclElement mVertexElements[MAX_VERTEX_ELEMENTS];
  uint8_t mNumVertexElements;
  YRenderDevice::VertexDeclID mVertexDeclID;
};

class IndexBuffer {
 public:
  IndexBuffer();
  void Release();

  void Initialize(YRenderDevice::UsageType usage, uint32_t count);
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
  YRenderDevice::UsageType mUsageType;
  uint32_t mTotalCount;
  uint32_t mFillCount;
  YRenderDevice::IndexBufferID mIndexBufferIDs[2];
};

class VertexBuffer {
 public:
  VertexBuffer();
  void Release();

  void Initialize(YRenderDevice::UsageType usage,
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
  YRenderDevice::UsageType mUsageType;
  uint32_t mStride;
  uint32_t mTotalSize;
  uint32_t mFillSize;
  YRenderDevice::VertexBufferID mVertexBufferIDs[2];
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_VERTEXBUFFER_H
