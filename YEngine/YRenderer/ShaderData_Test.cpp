#include <YCommon/Headers/stdincludes.h>
#include "ShaderData.h"

#include <gtest/gtest.h>

#include <YCommon/Headers/TestHelpers.h>
#include <YEngine/YRenderDevice/PixelFormat.h>
#include <YEngine/YRenderDevice/RenderDevice_Mock.h>
#include <YEngine/YRenderDevice/SamplerState.h>

#include "BasicRendererTest.h"
#include "RenderDeviceState.h"

namespace YEngine { namespace YRenderer {

class ShaderDataTest : public BasicRendererTest {};
class ShaderDataFailTest : public BasicRendererTest {};

TEST_F(ShaderDataTest, ShaderFloatArgEmptyRelease) {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(YRenderDevice::kUsageType_Static);
  float_arg.Release();
}

TEST_FAILURE_F(ShaderDataFailTest, ShaderFloatArgInvalidFill,
               "Invalid data") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(YRenderDevice::kUsageType_Static);

  const float kFloats[] = { 1.0f, 2.0f, 3.0f };
  YRenderDevice::RenderDeviceMock::ExpectCreateConstantBuffer(
      0, YRenderDevice::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));
}

TEST_FAILURE_F(ShaderDataFailTest, VertexShaderFloatArgActivateNoFill,
               "Cannot activate") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(YRenderDevice::kUsageType_Static);
  RenderDeviceState device_state;
  float_arg.ActivateVertexShaderArg(device_state);
}

TEST_FAILURE_F(ShaderDataFailTest, PixelShaderFloatArgActivateNoFill,
               "Cannot activate") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(YRenderDevice::kUsageType_Static);
  RenderDeviceState device_state;
  float_arg.ActivatePixelShaderArg(device_state);
}

TEST_F(ShaderDataTest, ShaderFloatFillTest) {
  const ShaderFloatParam float_param(4, 3);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(YRenderDevice::kUsageType_Static);
  const float kFloats[] = { 1.0f, 2.0f, 3.0f, 4.0f };

  // First fill creates constant buffer.
  const YRenderDevice::ConstantBufferID kConstantBufferID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID, YRenderDevice::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Second fill creates another constant buffer.
  const YRenderDevice::ConstantBufferID kConstantBufferID2 = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID2, YRenderDevice::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Third fill re-uses first constant buffer.
  YRenderDevice::RenderDeviceMock::ExpectFillConstantBuffer(
      kConstantBufferID, kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Fourth fill re-uses second constant buffer.
  YRenderDevice::RenderDeviceMock::ExpectFillConstantBuffer(
      kConstantBufferID2, kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Release
  YRenderDevice::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID);
  YRenderDevice::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID2);
  float_arg.Release();
}

TEST_F(ShaderDataTest, ShaderFloatActivationTest) {
  RenderDeviceState device_state;
  const ShaderFloatParam float_param(4, 3);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(YRenderDevice::kUsageType_Static);
  const float kFloats[] = { 1.0f, 2.0f, 3.0f, 4.0f };

  // Fill creates constant buffer.
  const YRenderDevice::ConstantBufferID kConstantBufferID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID, YRenderDevice::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Activate
  YRenderDevice::RenderDeviceMock::ExpectActivateVertexConstantBuffer(
      3, kConstantBufferID);
  float_arg.ActivateVertexShaderArg(device_state);

  YRenderDevice::RenderDeviceMock::ExpectActivatePixelConstantBuffer(
      3, kConstantBufferID);
  float_arg.ActivatePixelShaderArg(device_state);

  // Release
  YRenderDevice::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID);
  float_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexArgEmptyRelease) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         123, 234, 1,
                         YRenderDevice::kPixelFormat_A8R8G8B8);
  texture_arg.Release();
}

TEST_FAILURE_F(ShaderDataFailTest, ShaderTexArgInvalidMipFill,
               "Number of mip levels do not match") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         2, 2, 2,
                         YRenderDevice::kPixelFormat_A8R8G8B8);

  const uint32_t kPixelData[] = { 1, 2, 3, 4 };
  const YRenderDevice::TextureID kTextureID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, YRenderDevice::kUsageType_Static, 2, 2, 2,
      YRenderDevice::kPixelFormat_A8R8G8B8);
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kPixelData, sizeof(kPixelData));
  texture_arg.Fill(kPixelData, sizeof(kPixelData));
}

TEST_FAILURE_F(ShaderDataFailTest, ShaderTexArgInvalidFillSize,
               "Invalid texture data") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         2, 2, 1,
                         YRenderDevice::kPixelFormat_A8R8G8B8);

  const uint32_t kPixelData[] = { 1, 2 };
  const YRenderDevice::TextureID kTextureID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, YRenderDevice::kUsageType_Static, 2, 2, 1,
      YRenderDevice::kPixelFormat_A8R8G8B8);
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kPixelData, sizeof(kPixelData));
  texture_arg.Fill(kPixelData, sizeof(kPixelData));
}

TEST_FAILURE_F(ShaderDataFailTest, VertexShaderTexArgActivateNoFill,
               "Cannot activate") {
  RenderDeviceState device_state;
  const ShaderTextureParam texture_param(3, 0);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         123, 234, 1,
                         YRenderDevice::kPixelFormat_A8R8G8B8);

  texture_arg.ActivateVertexShaderTexture(device_state);
}

TEST_FAILURE_F(ShaderDataFailTest, PixelShaderTexArgActivateNoFill,
               "Cannot activate") {
  RenderDeviceState device_state;
  const ShaderTextureParam texture_param(3, 0);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         123, 234, 1,
                         YRenderDevice::kPixelFormat_A8R8G8B8);

  texture_arg.ActivatePixelShaderTexture(device_state);
}

TEST_F(ShaderDataTest, ShaderTexFillTest) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         2, 2, 1,
                         YRenderDevice::kPixelFormat_A8R8G8B8);
  const uint32_t kTextureData[] = { 1, 2, 3, 4 };

  // First fill creates texture.
  const YRenderDevice::TextureID kTextureID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, YRenderDevice::kUsageType_Static, 2, 2, 1,
      YRenderDevice::kPixelFormat_A8R8G8B8);
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Second fill creates another texture.
  const YRenderDevice::TextureID kTextureID2 = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateTexture(
      kTextureID2, YRenderDevice::kUsageType_Static, 2, 2, 1,
      YRenderDevice::kPixelFormat_A8R8G8B8);
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID2, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Third fill re-uses first texture.
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Fourth fill re-uses second texture.
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID2, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Release
  YRenderDevice::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  YRenderDevice::RenderDeviceMock::ExpectReleaseTexture(kTextureID2);
  texture_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexFillMipTest) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         2, 2, 2,
                         YRenderDevice::kPixelFormat_A8R8G8B8);
  const uint32_t kTextureDataMip0[] = { 1, 2, 3, 4 };
  const uint32_t kTextureDataMip1[] = { 5 };
  const void* kTextureData[] = { kTextureDataMip0, kTextureDataMip1 };
  const uint32_t kTextureDataSizes[] = { sizeof(kTextureDataMip0),
                                         sizeof(kTextureDataMip1) };
  static_assert(ARRAY_SIZE(kTextureData) == ARRAY_SIZE(kTextureDataSizes),
                "Sanity check failed.");

  const YRenderDevice::TextureID kTextureID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, YRenderDevice::kUsageType_Static, 2, 2, 2,
      YRenderDevice::kPixelFormat_A8R8G8B8);
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureDataMip0, sizeof(kTextureDataMip0));
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 1, kTextureDataMip1, sizeof(kTextureDataMip1));
  texture_arg.FillMips(ARRAY_SIZE(kTextureData),
                       kTextureData, kTextureDataSizes);

  YRenderDevice::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  texture_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexActivationTest) {
  RenderDeviceState device_state;
  // Sampler State.
  YRenderDevice::SamplerState sampler_state;
  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  const YRenderDevice::SamplerState* sampler_state_pointer =
      RenderStateCache::GetSamplerState(sampler_hash);
  ASSERT_NE(sampler_state_pointer, nullptr);

  // Texture Arg.
  const ShaderTextureParam texture_param(3, sampler_hash);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(YRenderDevice::kUsageType_Static,
                         2, 2, 1,
                         YRenderDevice::kPixelFormat_A8R8G8B8);
  const uint32_t kTextureData[] = { 1, 2, 3, 4 };

  // Fill.
  const YRenderDevice::TextureID kTextureID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, YRenderDevice::kUsageType_Static, 2, 2, 1,
      YRenderDevice::kPixelFormat_A8R8G8B8);
  YRenderDevice::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Activate.
  const YRenderDevice::SamplerStateID kSamplerStateID = 234;
  YRenderDevice::RenderDeviceMock::ExpectCreateSamplerState(
      kSamplerStateID, *sampler_state_pointer);
  YRenderDevice::RenderDeviceMock::ExpectActivateVertexSamplerState(
      3, kSamplerStateID);
  YRenderDevice::RenderDeviceMock::ExpectActivateVertexTexture(3, kTextureID);
  texture_arg.ActivateVertexShaderTexture(device_state);

  YRenderDevice::RenderDeviceMock::ExpectActivatePixelSamplerState(
      3, kSamplerStateID);
  YRenderDevice::RenderDeviceMock::ExpectActivatePixelTexture(3, kTextureID);
  texture_arg.ActivatePixelShaderTexture(device_state);

  // Release.
  YRenderDevice::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  texture_arg.Release();

  YRenderDevice::RenderDeviceMock::ExpectReleaseSamplerState(kSamplerStateID);
  RenderStateCache::Terminate();
}

}} // namespace YEngine { namespace YRenderer {
