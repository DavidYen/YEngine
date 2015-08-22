#include <YCommon/Headers/stdincludes.h>
#include "ShaderData.h"

#include <string.h>

#include <YCommon/YUtils/Hash.h>

#include "RenderStateCache.h"

#define INVALID_VERTEX_DECL static_cast<YRenderDevice::VertexDeclID>(-1)
#define INVALID_CONSTANT_BUFFER static_cast<YRenderDevice::ConstantBufferID>(-1)
#define INVALID_SAMPLER_STATE static_cast<YRenderDevice::SamplerStateID>(-1)
#define INVALID_TEXTURE static_cast<YRenderDevice::TextureID>(-1)
#define INVALID_VERTEX_SHADER static_cast<YRenderDevice::VertexShaderID>(-1)
#define INVALID_PIXEL_SHADER static_cast<YRenderDevice::PixelShaderID>(-1)

namespace YEngine { namespace YRenderer {

VertexDecl::VertexDecl(const YRenderDevice::VertexDeclElement* elements,
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
    YRenderDevice::RenderDevice::ReleaseVertexDeclaration(mVertexDeclID);
  }
}

void VertexDecl::Activate() {
  if (mVertexDeclID == INVALID_VERTEX_DECL) {
    mVertexDeclID =
        YRenderDevice::RenderDevice::CreateVertexDeclaration(
            mVertexElements, mNumVertexElements);
  }

  YRenderDevice::RenderDevice::ActivateVertexDeclaration(mVertexDeclID);
}

ShaderFloatArg::ShaderFloatArg(YRenderDevice::UsageType usage,
                               const ShaderFloatParam* float_param)
  : mNumFloats(float_param->mNumFloats),
    mReg(float_param->mReg),
    mUsageType(usage),
    mActiveIndex(0) {
  mConstantBufferIDs[0] = INVALID_CONSTANT_BUFFER;
  mConstantBufferIDs[1] = INVALID_CONSTANT_BUFFER;
}

void ShaderFloatArg::Release() {
  for (int i = 0; i < ARRAY_SIZE(mConstantBufferIDs); ++i) {
    if (mConstantBufferIDs[i] != INVALID_CONSTANT_BUFFER) {
      YRenderDevice::RenderDevice::ReleaseConstantBuffer(mConstantBufferIDs[i]);
      mConstantBufferIDs[i] = INVALID_CONSTANT_BUFFER;
    }
  }
}

void ShaderFloatArg::Fill(const void* data, size_t data_size) {
  YASSERT(data_size == mNumFloats * sizeof(float),
          "Invalid data size (%u), expected %u floats (%u).",
          static_cast<uint32_t>(data_size),
          mNumFloats, mNumFloats * sizeof(float));
  mActiveIndex = !mActiveIndex;
  if (mConstantBufferIDs[mActiveIndex] == INVALID_CONSTANT_BUFFER) {
    mConstantBufferIDs[mActiveIndex] =
        YRenderDevice::RenderDevice::CreateConstantBuffer(mUsageType, data_size,
                                                          data, data_size);
  } else {
    YASSERT(mUsageType == YRenderDevice::kUsageType_Static ||
            mUsageType == YRenderDevice::kUsageType_Dynamic,
            "Can only modify shader float args that are static or dynamic.");
    YRenderDevice::RenderDevice::FillConstantBuffer(
        mConstantBufferIDs[mActiveIndex], data, data_size);
  }
}

void ShaderFloatArg::Activate() {
  YASSERT(mConstantBufferIDs[mActiveIndex] != INVALID_CONSTANT_BUFFER,
          "Cannot activate non-filled shader float argument.");
  YRenderDevice::RenderDevice::ActivateConstantBuffer(
      mReg, mConstantBufferIDs[mActiveIndex]);
}

ShaderTextureArg::ShaderTextureArg(YRenderDevice::UsageType usage,
                                   uint32_t width, uint32_t height,
                                   uint8_t num_mips,
                                   YRenderDevice::PixelFormat format,
                                   const ShaderTextureParam* texture_param)
  : mSamplerStateHash(texture_param->mSamplerStateHash),
    mWidth(width),
    mHeight(height),
    mSlot(texture_param->mSlot),
    mActiveIndex(0),
    mNumMips(num_mips),
    mUsageType(usage),
    mFormat(format),
    mSamplerStateID(INVALID_SAMPLER_STATE) {
  mTextureIDs[0] = INVALID_TEXTURE;
  mTextureIDs[1] = INVALID_TEXTURE;
}

void ShaderTextureArg::Release() {
  for (int i = 0; i < ARRAY_SIZE(mTextureIDs); ++i) {
    if (mTextureIDs[i] != INVALID_TEXTURE) {
      YRenderDevice::RenderDevice::ReleaseTexture(mTextureIDs[i]);
      mTextureIDs[i] = INVALID_TEXTURE;
    }
  }
}

void ShaderTextureArg::Fill(const void* data, size_t data_size) {
  FillMips(1, &data, &data_size);
}

void ShaderTextureArg::FillMips(uint8_t mip_levels,
                                const void** datas, size_t* data_sizes) {
  YASSERT(mip_levels == mNumMips,
          "Number of mip levels do not match %u != %u.",
          mip_levels, mNumMips);
  mActiveIndex = !mActiveIndex;
  if (mTextureIDs[mActiveIndex] == INVALID_TEXTURE) {
    mTextureIDs[mActiveIndex] = YRenderDevice::RenderDevice::CreateTexture(mUsageType, mWidth,
                                                             mHeight, mNumMips,
                                                             mFormat);
  } else {
    YASSERT(mUsageType == YRenderDevice::kUsageType_Static ||
            mUsageType == YRenderDevice::kUsageType_Dynamic,
            "Can only modify textures that are static or dynamic.");
  }

  for (uint8_t i = 0; i < mip_levels; ++i) {
    const void* data = datas[i];
    const size_t data_size = data_sizes[i];
    const uint32_t mip_width = mWidth >> i;
    const uint32_t mip_height = mHeight >> i;
    const uint32_t width = mip_width ? mip_width : 1;
    const uint32_t height = mip_height ? mip_height : 1;

    YASSERT(data_size == width * height,
            "Invalid texture data size (%u), expected %u bytes.\n"
            "  Original Size: %ux%u\n"
            "  Mip Level (%u): %ux%u.",
            static_cast<uint32_t>(data_size),
            static_cast<uint32_t>(width * height),
            mWidth, mHeight, i, width, height);

    YRenderDevice::RenderDevice::FillTextureMip(mTextureIDs[mActiveIndex], i,
                                  data, data_size);
  }
}

void ShaderTextureArg::Activate() {
  YASSERT(mTextureIDs[mActiveIndex] != INVALID_TEXTURE,
          "Cannot activate texture which has not been filled.");
  if (mSamplerStateID == INVALID_SAMPLER_STATE) {
    mSamplerStateID = RenderStateCache::GetSamplerStateID(mSamplerStateHash);
  }
  YRenderDevice::RenderDevice::ActivateSamplerState(mSlot, mSamplerStateID);
  YRenderDevice::RenderDevice::ActivateTexture(mSlot, mTextureIDs[mActiveIndex]);
}

VertexShader::VertexShader(const void* shader_data, size_t shader_size)
  : mShaderHash(0),
    mActiveIndex(0) {
  mVertexShaderIDs[0] = INVALID_VERTEX_SHADER;
  mVertexShaderIDs[1] = INVALID_VERTEX_SHADER;
  SetVertexShader(shader_data, shader_size);
}

void VertexShader::Release() {
  mShaderHash = 0;
  for (int i = 0; i < ARRAY_SIZE(mVertexShaderIDs); ++i) {
    if (mVertexShaderIDs[i] != INVALID_VERTEX_SHADER) {
      YRenderDevice::RenderDevice::ReleaseVertexShader(mVertexShaderIDs[i]);
      mVertexShaderIDs[i] = INVALID_VERTEX_SHADER;
    }
  }
}

void VertexShader::SetVertexShader(const void* shader_data,
                                   size_t shader_size) {
  const uint64_t shader_hash = YCommon::YUtils::Hash::Hash64(shader_data,
                                                             shader_size);
  if (mShaderHash != shader_hash) {
    mActiveIndex = !mActiveIndex;
    if (mVertexShaderIDs[mActiveIndex] != INVALID_VERTEX_SHADER) {
      YRenderDevice::RenderDevice::ReleaseVertexShader(
          mVertexShaderIDs[mActiveIndex]);
    }
    mVertexShaderIDs[mActiveIndex] =
        YRenderDevice::RenderDevice::CreateVertexShader(shader_data,
                                                        shader_size);
    mShaderHash = shader_hash;
  }
}

void VertexShader::Activate() {
  YRenderDevice::RenderDevice::ActivateVertexShader(
      mVertexShaderIDs[mActiveIndex]);
}

PixelShader::PixelShader(const void* shader_data, size_t shader_size)
  : mShaderHash(0),
    mActiveIndex(0) {
  mPixelShaderIDs[0] = INVALID_PIXEL_SHADER;
  mPixelShaderIDs[1] = INVALID_PIXEL_SHADER;
  SetPixelShader(shader_data, shader_size);
}

void PixelShader::Release() {
  mShaderHash = 0;
  for (int i = 0; i < ARRAY_SIZE(mPixelShaderIDs); ++i) {
    if (mPixelShaderIDs[i] != INVALID_PIXEL_SHADER) {
      YRenderDevice::RenderDevice::ReleasePixelShader(mPixelShaderIDs[i]);
      mPixelShaderIDs[i] = INVALID_PIXEL_SHADER;
    }
  }
}

void PixelShader::SetPixelShader(const void* shader_data,
                                 size_t shader_size) {
  const uint64_t shader_hash = YCommon::YUtils::Hash::Hash64(shader_data,
                                                             shader_size);
  if (mShaderHash != shader_hash) {
    mActiveIndex = !mActiveIndex;
    if (mPixelShaderIDs[mActiveIndex] != INVALID_PIXEL_SHADER) {
      YRenderDevice::RenderDevice::ReleasePixelShader(
          mPixelShaderIDs[mActiveIndex]);
    }
    mPixelShaderIDs[mActiveIndex] =
        YRenderDevice::RenderDevice::CreatePixelShader(shader_data,
                                                       shader_size);
    mShaderHash = shader_hash;
  }
}

void PixelShader::Activate() {
  YRenderDevice::RenderDevice::ActivatePixelShader(
      mPixelShaderIDs[mActiveIndex]);
}

}} // namespace YEngine { namespace YRenderer {
