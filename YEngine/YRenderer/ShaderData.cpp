#include <YCommon/Headers/stdincludes.h>
#include "ShaderData.h"

#include <YCommon/YUtils/Hash.h>

#include "RenderDeviceState.h"
#include "RenderStateCache.h"

#define INVALID_CONSTANT_BUFFER static_cast<YRenderDevice::ConstantBufferID>(-1)
#define INVALID_SAMPLER_STATE static_cast<YRenderDevice::SamplerStateID>(-1)
#define INVALID_TEXTURE static_cast<YRenderDevice::TextureID>(-1)
#define INVALID_VERTEX_SHADER static_cast<YRenderDevice::VertexShaderID>(-1)
#define INVALID_PIXEL_SHADER static_cast<YRenderDevice::PixelShaderID>(-1)

namespace YEngine { namespace YRenderer {

ShaderFloatArg::ShaderFloatArg(const ShaderFloatParam* float_param)
  : mFloatParam(float_param),
    mUsageType(YRenderDevice::kUsageType_Invalid),
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

void ShaderFloatArg::Initialize(YRenderDevice::UsageType usage) {
  mUsageType = usage;
}

void ShaderFloatArg::Fill(const void* data, uint32_t data_size) {
  YASSERT(mUsageType != YRenderDevice::kUsageType_Invalid,
          "Shader Float Argument has not been initialized.");

  const uint8_t num_floats = mFloatParam->mNumFloats;
  YASSERT(data_size == num_floats * sizeof(float),
          "Invalid data size (%u), expected %u floats (%u).",
          static_cast<uint32_t>(data_size),
          num_floats, num_floats * sizeof(float));
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

void ShaderFloatArg::ActivateVertexShaderArg(RenderDeviceState& device_state) {
  const uint8_t reg = mFloatParam->mReg;
  const uint8_t num_floats = mFloatParam->mNumFloats;
  const uint8_t num_regs = num_floats / 4 + (num_floats % 4 ? 1 : 0);
  YASSERT(mConstantBufferIDs[mActiveIndex] != INVALID_CONSTANT_BUFFER,
          "Cannot activate non-filled shader float argument.");
  device_state.ActivateVertexFloatArg(reg, num_regs,
                                      mConstantBufferIDs[mActiveIndex]);
}

void ShaderFloatArg::ActivatePixelShaderArg(RenderDeviceState& device_state) {
  const uint8_t reg = mFloatParam->mReg;
  const uint8_t num_floats = mFloatParam->mNumFloats;
  const uint8_t num_regs = num_floats / 4 + (num_floats % 4 ? 1 : 0);
  YASSERT(mConstantBufferIDs[mActiveIndex] != INVALID_CONSTANT_BUFFER,
          "Cannot activate non-filled shader float argument.");
  device_state.ActivatePixelFloatArg(reg, num_regs,
                                     mConstantBufferIDs[mActiveIndex]);
}

ShaderTextureArg::ShaderTextureArg(const ShaderTextureParam* texture_param)
  : mTextureParam(texture_param),
    mSamplerStateHash(0),
    mWidth(0),
    mHeight(0),
    mActiveIndex(0),
    mNumMips(0),
    mUsageType(YRenderDevice::kUsageType_Invalid),
    mFormat(YRenderDevice::kPixelFormat_A8R8G8B8),
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

void ShaderTextureArg::Initialize(YRenderDevice::UsageType usage,
                                  uint32_t width, uint32_t height,
                                  uint8_t num_mips,
                                  YRenderDevice::PixelFormat format) {
  mUsageType = usage;
  mWidth = width;
  mHeight = height;
  mNumMips = num_mips,
  mFormat = format;
}

void ShaderTextureArg::Fill(const void* data, uint32_t data_size) {
  FillMips(1, &data, &data_size);
}

void ShaderTextureArg::FillMips(uint8_t mip_levels,
                                const void** datas, const uint32_t* data_sizes) {
  YASSERT(mUsageType != YRenderDevice::kUsageType_Invalid,
          "Shader Texture Argument has not been initialized.");
  YASSERT(mip_levels == mNumMips,
          "Number of mip levels do not match %u != %u.",
          mip_levels, mNumMips);
  mActiveIndex = !mActiveIndex;
  if (mTextureIDs[mActiveIndex] == INVALID_TEXTURE) {
    mTextureIDs[mActiveIndex] =
        YRenderDevice::RenderDevice::CreateTexture(mUsageType, mWidth,
                                                   mHeight, mNumMips, mFormat);
  } else {
    YASSERT(mUsageType == YRenderDevice::kUsageType_Static ||
            mUsageType == YRenderDevice::kUsageType_Dynamic,
            "Can only modify textures that are static or dynamic.");
  }

  const uint32_t format_size = YRenderDevice::kPixelFormatSize[mFormat];
  for (uint8_t i = 0; i < mip_levels; ++i) {
    const void* data = datas[i];
    const uint32_t data_size = data_sizes[i];
    const uint32_t mip_width = mWidth >> i;
    const uint32_t mip_height = mHeight >> i;
    const uint32_t width = mip_width ? mip_width : 1;
    const uint32_t height = mip_height ? mip_height : 1;
    const uint32_t mip_size = width * height * format_size;

    YASSERT(data_size == mip_size,
            "Invalid texture data size (%u), expected %u bytes.\n"
            "  Original Size: %ux%ux%u\n"
            "  Mip Level (%u): %ux%ux%u.",
            static_cast<uint32_t>(data_size),
            static_cast<uint32_t>(mip_size),
            mWidth, mHeight, format_size, i, width, height, format_size);

    YRenderDevice::RenderDevice::FillTextureMip(
        mTextureIDs[mActiveIndex], i, data, data_size);
  }
}

void ShaderTextureArg::ActivateVertexShaderTexture(
    RenderDeviceState& device_state) {
  YASSERT(mTextureIDs[mActiveIndex] != INVALID_TEXTURE,
          "Cannot activate texture which has not been filled.");
  const uint8_t slot_num = mTextureParam->mSlot;
  if (mSamplerStateHash != mTextureParam->mSamplerStateHash) {
    mSamplerStateHash = mTextureParam->mSamplerStateHash;
    mSamplerStateID = RenderStateCache::GetSamplerStateID(mSamplerStateHash);
  }

  device_state.ActivateVertexSamplerState(slot_num, mSamplerStateID);
  device_state.ActivateVertexTexture(slot_num, mTextureIDs[mActiveIndex]);
}

void ShaderTextureArg::ActivatePixelShaderTexture(
    RenderDeviceState& device_state) {
  YASSERT(mTextureIDs[mActiveIndex] != INVALID_TEXTURE,
          "Cannot activate texture which has not been filled.");
  const uint8_t slot_num = mTextureParam->mSlot;
  if (mSamplerStateHash != mTextureParam->mSamplerStateHash) {
    mSamplerStateHash = mTextureParam->mSamplerStateHash;
    mSamplerStateID = RenderStateCache::GetSamplerStateID(mSamplerStateHash);
  }

  device_state.ActivatePixelSamplerState(slot_num, mSamplerStateID);
  device_state.ActivatePixelTexture(slot_num, mTextureIDs[mActiveIndex]);
}

}} // namespace YEngine { namespace YRenderer {
