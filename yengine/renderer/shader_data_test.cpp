#include "yengine/renderer/shader_data.h"

#include <gtest/gtest.h>

#include "ycommon/headers/test_helpers.h"
#include "yengine/render_device/pixel_format.h"
#include "yengine/render_device/render_device_mock.h"
#include "yengine/render_device/sampler_state.h"
#include "yengine/renderer/basic_renderer_test.h"
#include "yengine/renderer/render_device_state.h"

namespace yengine { namespace renderer {

class ShaderDataTest : public BasicRendererTest {};
class ShaderDataFailTest : public BasicRendererTest {};

TEST_F(ShaderDataTest, ShaderFloatArgEmptyRelease) {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(render_device::kUsageType_Static);
  float_arg.Release();
}

TEST_FAILURE_F(ShaderDataFailTest, ShaderFloatArgInvalidFill,
               "Invalid data") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(render_device::kUsageType_Static);

  const float kFloats[] = { 1.0f, 2.0f, 3.0f };
  render_device::RenderDeviceMock::ExpectCreateConstantBuffer(
      0, render_device::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));
}

TEST_FAILURE_F(ShaderDataFailTest, VertexShaderFloatArgActivateNoFill,
               "Cannot activate") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(render_device::kUsageType_Static);
  RenderDeviceState device_state;
  float_arg.ActivateVertexShaderArg(device_state);
}

TEST_FAILURE_F(ShaderDataFailTest, PixelShaderFloatArgActivateNoFill,
               "Cannot activate") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(render_device::kUsageType_Static);
  RenderDeviceState device_state;
  float_arg.ActivatePixelShaderArg(device_state);
}

TEST_F(ShaderDataTest, ShaderFloatFillTest) {
  const ShaderFloatParam float_param(4, 3);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(render_device::kUsageType_Static);
  const float kFloats[] = { 1.0f, 2.0f, 3.0f, 4.0f };

  // First fill creates constant buffer.
  const render_device::ConstantBufferID kConstantBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID, render_device::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Second fill creates another constant buffer.
  const render_device::ConstantBufferID kConstantBufferID2 = 234;
  render_device::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID2, render_device::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Third fill re-uses first constant buffer.
  render_device::RenderDeviceMock::ExpectFillConstantBuffer(
      kConstantBufferID, kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Fourth fill re-uses second constant buffer.
  render_device::RenderDeviceMock::ExpectFillConstantBuffer(
      kConstantBufferID2, kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Release
  render_device::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID);
  render_device::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID2);
  float_arg.Release();
}

TEST_F(ShaderDataTest, ShaderFloatActivationTest) {
  RenderDeviceState device_state;
  const ShaderFloatParam float_param(4, 3);
  ShaderFloatArg float_arg(&float_param);
  float_arg.Initialize(render_device::kUsageType_Static);
  const float kFloats[] = { 1.0f, 2.0f, 3.0f, 4.0f };

  // Fill creates constant buffer.
  const render_device::ConstantBufferID kConstantBufferID = 123;
  render_device::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID, render_device::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Activate
  render_device::RenderDeviceMock::ExpectActivateVertexConstantBuffer(
      3, kConstantBufferID);
  float_arg.ActivateVertexShaderArg(device_state);

  render_device::RenderDeviceMock::ExpectActivatePixelConstantBuffer(
      3, kConstantBufferID);
  float_arg.ActivatePixelShaderArg(device_state);

  // Release
  render_device::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID);
  float_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexArgEmptyRelease) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         123, 234, 1,
                         render_device::kPixelFormat_A8R8G8B8);
  texture_arg.Release();
}

TEST_FAILURE_F(ShaderDataFailTest, ShaderTexArgInvalidMipFill,
               "Number of mip levels do not match") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         2, 2, 2,
                         render_device::kPixelFormat_A8R8G8B8);

  const uint32_t kPixelData[] = { 1, 2, 3, 4 };
  const render_device::TextureID kTextureID = 123;
  render_device::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, render_device::kUsageType_Static, 2, 2, 2,
      render_device::kPixelFormat_A8R8G8B8);
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kPixelData, sizeof(kPixelData));
  texture_arg.Fill(kPixelData, sizeof(kPixelData));
}

TEST_FAILURE_F(ShaderDataFailTest, ShaderTexArgInvalidFillSize,
               "Invalid texture data") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         2, 2, 1,
                         render_device::kPixelFormat_A8R8G8B8);

  const uint32_t kPixelData[] = { 1, 2 };
  const render_device::TextureID kTextureID = 123;
  render_device::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, render_device::kUsageType_Static, 2, 2, 1,
      render_device::kPixelFormat_A8R8G8B8);
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kPixelData, sizeof(kPixelData));
  texture_arg.Fill(kPixelData, sizeof(kPixelData));
}

TEST_FAILURE_F(ShaderDataFailTest, VertexShaderTexArgActivateNoFill,
               "Cannot activate") {
  RenderDeviceState device_state;
  const ShaderTextureParam texture_param(3, 0);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         123, 234, 1,
                         render_device::kPixelFormat_A8R8G8B8);

  texture_arg.ActivateVertexShaderTexture(device_state);
}

TEST_FAILURE_F(ShaderDataFailTest, PixelShaderTexArgActivateNoFill,
               "Cannot activate") {
  RenderDeviceState device_state;
  const ShaderTextureParam texture_param(3, 0);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         123, 234, 1,
                         render_device::kPixelFormat_A8R8G8B8);

  texture_arg.ActivatePixelShaderTexture(device_state);
}

TEST_F(ShaderDataTest, ShaderTexFillTest) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         2, 2, 1,
                         render_device::kPixelFormat_A8R8G8B8);
  const uint32_t kTextureData[] = { 1, 2, 3, 4 };

  // First fill creates texture.
  const render_device::TextureID kTextureID = 123;
  render_device::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, render_device::kUsageType_Static, 2, 2, 1,
      render_device::kPixelFormat_A8R8G8B8);
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Second fill creates another texture.
  const render_device::TextureID kTextureID2 = 234;
  render_device::RenderDeviceMock::ExpectCreateTexture(
      kTextureID2, render_device::kUsageType_Static, 2, 2, 1,
      render_device::kPixelFormat_A8R8G8B8);
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID2, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Third fill re-uses first texture.
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Fourth fill re-uses second texture.
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID2, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Release
  render_device::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  render_device::RenderDeviceMock::ExpectReleaseTexture(kTextureID2);
  texture_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexFillMipTest) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         2, 2, 2,
                         render_device::kPixelFormat_A8R8G8B8);
  const uint32_t kTextureDataMip0[] = { 1, 2, 3, 4 };
  const uint32_t kTextureDataMip1[] = { 5 };
  const void* kTextureData[] = { kTextureDataMip0, kTextureDataMip1 };
  const uint32_t kTextureDataSizes[] = { sizeof(kTextureDataMip0),
                                         sizeof(kTextureDataMip1) };
  static_assert(ARRAY_SIZE(kTextureData) == ARRAY_SIZE(kTextureDataSizes),
                "Sanity check failed.");

  const render_device::TextureID kTextureID = 123;
  render_device::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, render_device::kUsageType_Static, 2, 2, 2,
      render_device::kPixelFormat_A8R8G8B8);
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureDataMip0, sizeof(kTextureDataMip0));
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 1, kTextureDataMip1, sizeof(kTextureDataMip1));
  texture_arg.FillMips(ARRAY_SIZE(kTextureData),
                       kTextureData, kTextureDataSizes);

  render_device::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  texture_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexActivationTest) {
  RenderDeviceState device_state;
  // Sampler State.
  render_device::SamplerState sampler_state;
  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  const render_device::SamplerState* sampler_state_pointer =
      RenderStateCache::GetSamplerState(sampler_hash);
  ASSERT_NE(sampler_state_pointer, nullptr);

  // Texture Arg.
  const ShaderTextureParam texture_param(3, sampler_hash);
  ShaderTextureArg texture_arg(&texture_param);
  texture_arg.Initialize(render_device::kUsageType_Static,
                         2, 2, 1,
                         render_device::kPixelFormat_A8R8G8B8);
  const uint32_t kTextureData[] = { 1, 2, 3, 4 };

  // Fill.
  const render_device::TextureID kTextureID = 123;
  render_device::RenderDeviceMock::ExpectCreateTexture(
      kTextureID, render_device::kUsageType_Static, 2, 2, 1,
      render_device::kPixelFormat_A8R8G8B8);
  render_device::RenderDeviceMock::ExpectFillTextureMip(
      kTextureID, 0, kTextureData, sizeof(kTextureData));
  texture_arg.Fill(kTextureData, sizeof(kTextureData));

  // Activate.
  const render_device::SamplerStateID kSamplerStateID = 234;
  render_device::RenderDeviceMock::ExpectCreateSamplerState(
      kSamplerStateID, *sampler_state_pointer);
  render_device::RenderDeviceMock::ExpectActivateVertexSamplerState(
      3, kSamplerStateID);
  render_device::RenderDeviceMock::ExpectActivateVertexTexture(3, kTextureID);
  texture_arg.ActivateVertexShaderTexture(device_state);

  render_device::RenderDeviceMock::ExpectActivatePixelSamplerState(
      3, kSamplerStateID);
  render_device::RenderDeviceMock::ExpectActivatePixelTexture(3, kTextureID);
  texture_arg.ActivatePixelShaderTexture(device_state);

  // Release.
  render_device::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  texture_arg.Release();

  render_device::RenderDeviceMock::ExpectReleaseSamplerState(kSamplerStateID);
  RenderStateCache::Terminate();
}

}} // namespace yengine { namespace renderer {
