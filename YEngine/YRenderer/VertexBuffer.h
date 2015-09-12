#ifndef YENGINE_YRENDERER_VERTEXBUFFER_H
#define YENGINE_YRENDERER_VERTEXBUFFER_H

#include <YEngine/YRenderDevice/RenderDevice.h>

namespace YEngine { namespace YRenderer {

struct RenderDeviceState;

class IndexBuffer {
 public:
  IndexBuffer();
  void Release();

  void Initialize(YRenderDevice::UsageType usage, uint32_t count);
  uint32_t GetFillCount() const { return mFillCount; }

  void Fill(const uint16_t* data, uint32_t num_ints);
  void FillMulti(uint32_t arrays,
                 const uint16_t* const* datas,
                 const uint32_t* num_ints);
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

  void Fill(const float* data, uint32_t num_floats);
  void FillMulti(uint32_t arrays,
                 const float* const* datas,
                 const uint32_t* float_counts);
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
