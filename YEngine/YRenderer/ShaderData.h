#ifndef YENGINE_YRENDERER_SHADERDATA_H
#define YENGINE_YRENDERER_SHADERDATA_H

#include <YEngine/YRenderDevice/RenderDevice.h>
#include <YEngine/YRenderDevice/VertexDeclElement.h>

#define MAX_VERTEX_ELEMENTS 8

namespace YEngine { namespace YRenderer {

class VertexDecl {
 public:
  VertexDecl(const YRenderDevice::VertexDeclElement* elements,
             uint8_t num_elements);
  void Release();
  void Activate();

  const YRenderDevice::VertexDeclElement* GetVertexDeclElements() const {
    return mVertexElements;
  }
  const uint8_t GetNumVertexElements() const { return mNumVertexElements; }

 private:
  YRenderDevice::VertexDeclElement mVertexElements[MAX_VERTEX_ELEMENTS];
  uint8_t mNumVertexElements;
  YRenderDevice::VertexDeclID mVertexDeclID;
};

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
  ShaderFloatArg(YRenderDevice::UsageType usage,
                 const ShaderFloatParam* float_param);
  void Release();

  void Fill(const void* data, size_t data_size);
  void Activate();

 private:
  uint8_t mNumFloats;
  uint8_t mReg;
  YRenderDevice::UsageType mUsageType;
  uint8_t mActiveIndex;
  YRenderDevice::ConstantBufferID mConstantBufferIDs[2];
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
  ShaderTextureArg(YRenderDevice::UsageType usage,
                   uint32_t width, uint32_t height,
                   uint8_t num_mips, YRenderDevice::PixelFormat format,
                   const ShaderTextureParam* texture_param);
  void Release();

  void Fill(const void* data, size_t data_size);
  void FillMips(uint8_t mip_levels, const void** datas, size_t* data_sizes);
  void Activate();

 private:
  uint64_t mSamplerStateHash;
  uint32_t mWidth, mHeight;
  uint8_t mSlot;
  uint8_t mActiveIndex;
  uint8_t mNumMips;
  YRenderDevice::UsageType mUsageType;
  YRenderDevice::PixelFormat mFormat;
  YRenderDevice::SamplerStateID mSamplerStateID;
  YRenderDevice::TextureID mTextureIDs[2];
};

class VertexShader {
 public:
  VertexShader(const void* shader_data, size_t shader_size);
  void Release();

  void SetVertexShader(const void* shader_data, size_t shader_size);
  void Activate();

 private:
  uint64_t mShaderHash;
  YRenderDevice::VertexShaderID mVertexShaderIDs[2];
  uint8_t mActiveIndex;
};

class PixelShader {
 public:
  PixelShader(const void* shader_data, size_t shader_size);
  void Release();

  void SetPixelShader(const void* shader_data, size_t shader_size);
  void Activate();

 private:
  uint64_t mShaderHash;
  uint8_t mActiveIndex;
  YRenderDevice::PixelShaderID mPixelShaderIDs[2];
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_SHADERDATA_H
