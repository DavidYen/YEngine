#ifndef YENGINE_RENDERER_SHADER_DATA_H
#define YENGINE_RENDERER_SHADER_DATA_H

#include "yengine/render_device/render_device.h"

namespace yengine { namespace renderer {

struct RenderDeviceState;

class ShaderFloatParam {
 public:
  ShaderFloatParam(uint8_t num_floats, uint8_t reg)
    : mNumFloats(num_floats),
      mReg(reg) {}

 private:
  friend class ShaderFloatArg;

  uint8_t mNumFloats;
  uint8_t mReg;
};

class ShaderFloatArg {
 public:
  ShaderFloatArg(const ShaderFloatParam* float_param);
  void Release();

  const ShaderFloatParam* GetFloatParam() const { return mFloatParam; }
  uint8_t GetReg() const { return mFloatParam->mReg; }

  void Initialize(render_device::UsageType usage);
  void Fill(const void* data, uint32_t data_size);
  void ActivateVertexShaderArg(RenderDeviceState& device_state);
  void ActivatePixelShaderArg(RenderDeviceState& device_state);

 private:
  const ShaderFloatParam* mFloatParam;
  render_device::UsageType mUsageType;
  uint8_t mActiveIndex;
  render_device::ConstantBufferID mConstantBufferIDs[2];
};

class ShaderTextureParam {
 public:
  ShaderTextureParam(uint8_t slot, uint64_t sampler_state_hash)
    : mSlot(slot),
      mSamplerStateHash(sampler_state_hash) {}

 private:
  friend class ShaderTextureArg;

  uint8_t mSlot;
  uint64_t mSamplerStateHash;
};

class ShaderTextureArg {
 public:
  ShaderTextureArg(const ShaderTextureParam* texture_param);
  void Release();

  const ShaderTextureParam* GetTextureParam() const { return mTextureParam; }
  uint8_t GetSlot() const { return mTextureParam->mSlot; }

  void Initialize(render_device::UsageType usage,
                  uint32_t width, uint32_t height,
                  uint8_t num_mips, render_device::PixelFormat format);
  void Fill(const void* data, uint32_t data_size);
  void FillMips(uint8_t mip_levels, const void** datas,
                const uint32_t* data_sizes);
  void ActivateVertexShaderTexture(RenderDeviceState& device_state);
  void ActivatePixelShaderTexture(RenderDeviceState& device_state);

 private:
  const ShaderTextureParam* mTextureParam;
  uint64_t mSamplerStateHash;
  uint32_t mWidth, mHeight;
  uint8_t mActiveIndex;
  uint8_t mNumMips;
  render_device::UsageType mUsageType;
  render_device::PixelFormat mFormat;
  render_device::SamplerStateID mSamplerStateID;
  render_device::TextureID mTextureIDs[2];
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_SHADER_DATA_H
