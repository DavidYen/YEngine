#include <YCommon/Headers/stdincludes.h>
#include "ShaderData.h"

#include <gtest/gtest.h>

#include <YCommon/Headers/TestHelpers.h>
#include <YEngine/YRenderDevice/PixelFormat.h>
#include <YEngine/YRenderDevice/RenderDevice_Mock.h>
#include <YEngine/YRenderDevice/SamplerState.h>

#include "BasicRendererTest.h"

namespace YEngine { namespace YRenderer {

class ShaderDataTest : public BasicRendererTest {};

TEST_F(ShaderDataTest, VertexDeclEmptyRelease) {
  const YRenderDevice::VertexDeclElement kElements[] = {
    { 0, 1, 1, YRenderDevice::kVertexElementType_Float,
      YRenderDevice::kVertexElementUsage_Position },
  };
  VertexDecl vertex_decl(kElements, ARRAY_SIZE(kElements));
  vertex_decl.Release();
}

TEST_F(ShaderDataTest, VertexDeclActivationTest) {
  const YRenderDevice::VertexDeclElement kElements[] = {
    { 0, 1, 1, YRenderDevice::kVertexElementType_Float,
      YRenderDevice::kVertexElementUsage_Position },
  };
  VertexDecl vertex_decl(kElements, ARRAY_SIZE(kElements));

  const YRenderDevice::VertexDeclID kVertexDeclID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateVertexDeclaration(
      kVertexDeclID,
      vertex_decl.GetVertexDeclElements(),
      vertex_decl.GetNumVertexElements());
  YRenderDevice::RenderDeviceMock::ExpectActivateVertexDeclaration(
      kVertexDeclID);
  vertex_decl.Activate();

  // Second activation should not create the declaration again
  YRenderDevice::RenderDeviceMock::ExpectActivateVertexDeclaration(
      kVertexDeclID);
  vertex_decl.Activate();

  YRenderDevice::RenderDeviceMock::ExpectReleaseVertexDeclaration(
      kVertexDeclID);
  vertex_decl.Release();
}

TEST_F(ShaderDataTest, ShaderFloatArgEmptyRelease) {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(YRenderDevice::kUsageType_Static, &float_param);
  float_arg.Release();
}

TEST_FAILURE(ShaderDataFailTest, ShaderFloatArgInvalidFill,
             "Invalid data") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(YRenderDevice::kUsageType_Static, &float_param);

  const float kFloats[] = { 1.0f, 2.0f, 3.0f };
  float_arg.Fill(kFloats, sizeof(kFloats));
}

TEST_FAILURE(ShaderDataFailTest, ShaderFloatArgActivateNoFill,
             "Cannot activate") {
  const ShaderFloatParam float_param(4, 0);
  ShaderFloatArg float_arg(YRenderDevice::kUsageType_Static, &float_param);

  float_arg.Activate();
}

TEST_F(ShaderDataTest, ShaderFloatFillTest) {
  const ShaderFloatParam float_param(4, 3);
  ShaderFloatArg float_arg(YRenderDevice::kUsageType_Static, &float_param);
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
  const ShaderFloatParam float_param(4, 3);
  ShaderFloatArg float_arg(YRenderDevice::kUsageType_Static, &float_param);
  const float kFloats[] = { 1.0f, 2.0f, 3.0f, 4.0f };

  // Fill creates constant buffer.
  const YRenderDevice::ConstantBufferID kConstantBufferID = 123;
  YRenderDevice::RenderDeviceMock::ExpectCreateConstantBuffer(
      kConstantBufferID, YRenderDevice::kUsageType_Static,
      sizeof(kFloats), kFloats, sizeof(kFloats));
  float_arg.Fill(kFloats, sizeof(kFloats));

  // Activate
  YRenderDevice::RenderDeviceMock::ExpectActivateConstantBuffer(
      3, kConstantBufferID);
  float_arg.Activate();

  YRenderDevice::RenderDeviceMock::ExpectActivateConstantBuffer(
      3, kConstantBufferID);
  float_arg.Activate();

  // Release
  YRenderDevice::RenderDeviceMock::ExpectReleaseConstantBuffer(
      kConstantBufferID);
  float_arg.Release();
}

TEST_F(ShaderDataTest, ShaderTexArgEmptyRelease) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               123, 234, 1,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);
  texture_arg.Release();
}

TEST_FAILURE(ShaderDataFailTest, ShaderTexArgInvalidMipFill,
             "Invalid data") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               123, 234, 2,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);
  
  texture_arg.Fill(nullptr, 0);
}

TEST_FAILURE(ShaderDataFailTest, ShaderTexArgInvalidFillSize,
             "Invalid data") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               123, 234, 1,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);

  const uint32_t kPixelData[] = { 1, 2 };
  texture_arg.Fill(kPixelData, sizeof(kPixelData));
}

TEST_FAILURE(ShaderDataFailTest, ShaderTexArgActivateNoFill,
             "Cannot activate") {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               123, 234, 1,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);

  texture_arg.Activate();
}

TEST_F(ShaderDataTest, ShaderTexFillTest) {
  const ShaderTextureParam texture_param(3, 0xDEADBEEF);
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               2, 2, 1,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);
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
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               2, 2, 2,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);
  const uint32_t kTextureDataMip0[] = { 1, 2, 3, 4 };
  const uint32_t kTextureDataMip1[] = { 5 };
  const void* kTextureData[] = { kTextureDataMip0, kTextureDataMip1 };
  const size_t kTextureDataSizes[] = { sizeof(kTextureDataMip0),
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
  // Sampler State.
  YRenderDevice::SamplerState sampler_state;
  const uint64_t sampler_hash =
      RenderStateCache::InsertSamplerState(sampler_state);
  const YRenderDevice::SamplerState* sampler_state_pointer =
      RenderStateCache::GetSamplerState(sampler_hash);
  ASSERT_NE(sampler_state_pointer, nullptr);

  // Texture Arg.
  const ShaderTextureParam texture_param(3, sampler_hash);
  ShaderTextureArg texture_arg(YRenderDevice::kUsageType_Static,
                               2, 2, 1,
                               YRenderDevice::kPixelFormat_A8R8G8B8,
                               &texture_param);
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
  YRenderDevice::RenderDeviceMock::ExpectActivateSamplerState(3,
                                                              kSamplerStateID);
  YRenderDevice::RenderDeviceMock::ExpectActivateTexture(3, kTextureID);
  texture_arg.Activate();

  // Release.
  YRenderDevice::RenderDeviceMock::ExpectReleaseTexture(kTextureID);
  texture_arg.Release();

  YRenderDevice::RenderDeviceMock::ExpectReleaseSamplerState(kSamplerStateID);
  RenderStateCache::Terminate();
}

}} // namespace YEngine { namespace YRenderer {
