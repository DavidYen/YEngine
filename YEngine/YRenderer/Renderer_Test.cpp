#include <YCommon/Headers/stdincludes.h>
#include "Renderer.h"

#include <gtest/gtest.h>

#include <YCommon/YContainers/MemBuffer.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YEngine/YCore/StringTable.h>
#include <YEngine/YRenderDevice/RenderBlendState.h>
#include <YEngine/YRenderDevice/RenderDevice.h>
#include <YEngine/YRenderDevice/RenderDevice_Mock.h>
#include <YEngine/YRenderDevice/SamplerState.h>

namespace YEngine { namespace YRenderer {

namespace {
  const uint32_t gTestWidth = 128;
  const uint32_t gTestHeight = 128;
};

class RendererTest : public ::testing::Test {
 protected:
  RendererTest()
    : mBuffer(nullptr) {
    memset(&mHandle, 0, sizeof(mHandle));
  }

  virtual ~RendererTest() {
    delete [] mBuffer;
  }

  virtual void SetUp() {
    delete [] mBuffer;
    mBuffer = new uint8_t[2 * 1024 * 1024];
    mMemBuffer.Init(mBuffer, 2 * 1024 * 1024);

    YCore::StringTable::Initialize(32, 128, mMemBuffer.Allocate(10240), 10240);
    YRenderDevice::RenderDevice::Initialize(mHandle, gTestWidth, gTestHeight,
                                            mMemBuffer.Allocate(10240), 10240);
    Renderer::Initialize(mMemBuffer.Allocate(1024 * 1024), 1024 * 1024);
  }

  virtual void TearDown() {
    YRenderDevice::RenderDevice::Terminate();
    YCore::StringTable::Terminate();

    mMemBuffer.Reset();

    delete [] mBuffer;
    mBuffer = nullptr;
  }

  void* mBuffer;
  YCommon::YContainers::MemBuffer mMemBuffer;
  YCommon::YPlatform::PlatformHandle mHandle;
};

TEST_F(RendererTest, FixtureInitialization) {
}

TEST_F(RendererTest, RegisterViewportTest) {
  const char name[] = "test_viewport";
  Renderer::RegisterViewPort(name, sizeof(name),
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f,
                             kDimensionType_Absolute, 3.0f,
                             kDimensionType_Absolute, 4.0f,
                             0.1f, 1.0f);
  Renderer::RegisterViewPort(name, sizeof(name),
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f,
                             kDimensionType_Absolute, 3.0f,
                             kDimensionType_Absolute, 4.0f,
                             0.1f, 1.0f);

  EXPECT_FALSE(Renderer::ReleaseViewPort(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseViewPort(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterRenderTargetTest) {
  const char name[] = "test_render_target";
  const YRenderDevice::PixelFormat format =
      static_cast<YRenderDevice::PixelFormat>(123);
  Renderer::RegisterRenderTarget(name, sizeof(name), format,
                                 kDimensionType_Absolute, 1.0f,
                                 kDimensionType_Absolute, 2.0f);
  Renderer::RegisterRenderTarget(name, sizeof(name), format,
                                 kDimensionType_Absolute, 1.0f,
                                 kDimensionType_Absolute, 2.0f);

  EXPECT_FALSE(Renderer::ReleaseRenderTarget(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderTarget(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterBackBufferNameTest) {
  const char name[] = "test_back_buffer";
  Renderer::RegisterBackBufferName(name, sizeof(name));
  Renderer::RegisterBackBufferName(name, sizeof(name));

  EXPECT_FALSE(Renderer::ReleaseBackBufferName(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseBackBufferName(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterRenderPassTest) {
  const char name[] = "test_render_pass";
  const char variant[] = "test_variant";
  YRenderDevice::RenderBlendState blend_state;
  Renderer::RegisterRenderPass(name, sizeof(name),
                               variant, sizeof(variant),
                               blend_state,
                               nullptr, nullptr, 0);
  Renderer::RegisterRenderPass(name, sizeof(name),
                               variant, sizeof(variant),
                               blend_state,
                               nullptr, nullptr, 0);

  EXPECT_FALSE(Renderer::ReleaseRenderPass(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderPass(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterVertexDeclTest) {
  const char name[] = "test_vertex_decl";
  Renderer::RegisterVertexDecl(name, sizeof(name), nullptr, 0);
  Renderer::RegisterVertexDecl(name, sizeof(name), nullptr, 0);

  EXPECT_FALSE(Renderer::ReleaseVertexDecl(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseVertexDecl(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterShaderFloatParamTest) {
  const char name[] = "test_float_param";
  Renderer::RegisterShaderFloatParam(name, sizeof(name),
                                     4, 0, 1);
  Renderer::RegisterShaderFloatParam(name, sizeof(name),
                                     4, 0, 1);

  EXPECT_FALSE(Renderer::ReleaseShaderFloatParam(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseShaderFloatParam(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterShaderTextureParamTest) {
  const char name[] = "test_texture_param";
  YRenderDevice::SamplerState sampler_state;
  Renderer::RegisterShaderTextureParam(name, sizeof(name), 0, sampler_state);
  Renderer::RegisterShaderTextureParam(name, sizeof(name), 0, sampler_state);

  EXPECT_FALSE(Renderer::ReleaseShaderTextureParam(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseShaderTextureParam(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterShaderDataTest) {
  const char shader_name[] = "test_shader_name";
  const char variant_name[] = "test_variant_name";
  const char vertex_decl_name[] = "vertex_decl_name";
  const char vertex_shader[] = "test vertex shader";
  const char pixel_shader[] = "test pixel shader";

  const YRenderDevice::VertexShaderID vertex_shader_id = 10;
  const YRenderDevice::PixelShaderID pixel_shader_id = 20;
  YRenderDevice::RenderDeviceMock::ExpectCreateVertexShader(
      vertex_shader_id, vertex_shader, sizeof(vertex_shader));
  YRenderDevice::RenderDeviceMock::ExpectCreatePixelShader(
      pixel_shader_id, pixel_shader, sizeof(pixel_shader));

  Renderer::RegisterVertexDecl(vertex_decl_name, sizeof(vertex_decl_name),
                               nullptr, 0);

  Renderer::RegisterShaderData(shader_name, sizeof(shader_name),
                               variant_name, sizeof(variant_name),
                               vertex_decl_name, sizeof(vertex_decl_name),
                               0, nullptr, nullptr,
                               vertex_shader, sizeof(vertex_shader),
                               0, nullptr, nullptr,
                               pixel_shader, sizeof(pixel_shader));
  Renderer::RegisterShaderData(shader_name, sizeof(shader_name),
                               variant_name, sizeof(variant_name),
                               vertex_decl_name, sizeof(vertex_decl_name),
                               0, nullptr, nullptr,
                               vertex_shader, sizeof(vertex_shader),
                               0, nullptr, nullptr,
                               pixel_shader, sizeof(pixel_shader));

  EXPECT_FALSE(Renderer::ReleaseShaderData(shader_name, sizeof(shader_name),
                                           variant_name, sizeof(variant_name)));

  YRenderDevice::RenderDeviceMock::ExpectReleasePixelShader(pixel_shader_id);
  YRenderDevice::RenderDeviceMock::ExpectReleaseVertexShader(vertex_shader_id);
  EXPECT_TRUE(Renderer::ReleaseShaderData(shader_name, sizeof(shader_name),
                                          variant_name, sizeof(variant_name)));
  EXPECT_TRUE(Renderer::ReleaseVertexDecl(vertex_decl_name, 
                                          sizeof(vertex_decl_name)));
}

TEST_F(RendererTest, RegisterRenderPassesTest) {
  const char name[] = "test_render_passes_name";

  Renderer::RegisterRenderPasses(name, sizeof(name), nullptr, nullptr, 0);
  Renderer::RegisterRenderPasses(name, sizeof(name), nullptr, nullptr, 0);

  EXPECT_FALSE(Renderer::ReleaseRenderPasses(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderPasses(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterRenderTypeTest) {
  const char name[] = "test_render_type_name";
  const char shader[] = "test_shader_name";

  Renderer::RegisterRenderType(name, sizeof(name), shader, sizeof(shader));
  Renderer::RegisterRenderType(name, sizeof(name), shader, sizeof(shader));

  EXPECT_FALSE(Renderer::ReleaseRenderType(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderType(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterVertexDataTest) {
  const char name[] = "test_vertex_data_name";

  Renderer::RegisterVertexData(name, sizeof(name));
  Renderer::RegisterVertexData(name, sizeof(name));

  EXPECT_FALSE(Renderer::ReleaseVertexData(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseVertexData(name, sizeof(name)));
}

TEST_F(RendererTest, RegisterShaderFloatArgTest) {
  const char name[] = "test_float_arg";
  const char param[] = "test_float_param";
  Renderer::RegisterShaderFloatParam(param, sizeof(param), 4, 0, 1);

  Renderer::RegisterShaderArg(name, sizeof(name), param, sizeof(param));
  Renderer::RegisterShaderArg(name, sizeof(name), param, sizeof(param));

  EXPECT_FALSE(Renderer::ReleaseShaderArg(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseShaderArg(name, sizeof(name)));

  EXPECT_TRUE(Renderer::ReleaseShaderFloatParam(param, sizeof(param)));
}

TEST_F(RendererTest, RegisterShaderTextureArgTest) {
  const char name[] = "test_texture_arg";
  const char param[] = "test_texture_param";
  YRenderDevice::SamplerState sampler_state;
  Renderer::RegisterShaderTextureParam(param, sizeof(param), 0, sampler_state);

  Renderer::RegisterShaderArg(name, sizeof(name), param, sizeof(param));
  Renderer::RegisterShaderArg(name, sizeof(name), param, sizeof(param));

  EXPECT_FALSE(Renderer::ReleaseShaderArg(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseShaderArg(name, sizeof(name)));

  EXPECT_TRUE(Renderer::ReleaseShaderTextureParam(param, sizeof(param)));
}

TEST_F(RendererTest, RegisterGlobalFloatArgTest) {
  const char name[] = "global_float_arg";
  const char param[] = "test_float_param";
  Renderer::RegisterShaderFloatParam(param, sizeof(param), 4, 0, 1);
  Renderer::RegisterShaderArg(name, sizeof(name), param, sizeof(param));

  Renderer::RegisterGlobalArg(param, sizeof(param), name, sizeof(name));
  Renderer::RegisterGlobalArg(param, sizeof(param), name, sizeof(name));

  EXPECT_FALSE(Renderer::ReleaseGlobalArg(param, sizeof(param)));
  EXPECT_TRUE(Renderer::ReleaseGlobalArg(param, sizeof(param)));

  EXPECT_TRUE(Renderer::ReleaseShaderArg(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseShaderFloatParam(param, sizeof(param)));
}

TEST_F(RendererTest, RegisterGlobalTextureArgTest) {
  const char name[] = "global_texture_arg";
  const char param[] = "test_texture_param";
  YRenderDevice::SamplerState sampler_state;
  Renderer::RegisterShaderTextureParam(param, sizeof(param), 0, sampler_state);
  Renderer::RegisterShaderArg(name, sizeof(name), param, sizeof(param));

  Renderer::RegisterGlobalArg(param, sizeof(param), name, sizeof(name));
  Renderer::RegisterGlobalArg(param, sizeof(param), name, sizeof(name));

  EXPECT_FALSE(Renderer::ReleaseGlobalArg(param, sizeof(param)));
  EXPECT_TRUE(Renderer::ReleaseGlobalArg(param, sizeof(param)));

  EXPECT_TRUE(Renderer::ReleaseShaderArg(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseShaderTextureParam(param, sizeof(param)));
}

TEST_F(RendererTest, RegisterRenderObjectTest) {
  const char name[] = "render_object_name";
  const char viewport[] = "test_viewport";
  const char render_type[] = "test_render_type_name";
  const char shader[] = "test_shader_name";
  const char vertex_data[] = "test_vertex_data_name";

  Renderer::RegisterViewPort(viewport, sizeof(viewport),
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Absolute, 2.0f,
                             kDimensionType_Absolute, 3.0f,
                             kDimensionType_Absolute, 4.0f,
                             0.1f, 1.0f);
  Renderer::RegisterRenderType(render_type, sizeof(render_type),
                               shader, sizeof(shader));
  Renderer::RegisterVertexData(vertex_data, sizeof(vertex_data));

  Renderer::RegisterRenderObject(name, sizeof(name),
                                viewport, sizeof(viewport),
                                render_type, sizeof(render_type),
                                vertex_data, sizeof(vertex_data),
                                0, nullptr, nullptr);
  Renderer::RegisterRenderObject(name, sizeof(name),
                                viewport, sizeof(viewport),
                                render_type, sizeof(render_type),
                                vertex_data, sizeof(vertex_data),
                                0, nullptr, nullptr);

  EXPECT_FALSE(Renderer::ReleaseRenderObject(name, sizeof(name)));
  EXPECT_TRUE(Renderer::ReleaseRenderObject(name, sizeof(name)));

  EXPECT_TRUE(Renderer::ReleaseVertexData(vertex_data, sizeof(vertex_data)));
  EXPECT_TRUE(Renderer::ReleaseRenderType(render_type, sizeof(render_type)));
  EXPECT_TRUE(Renderer::ReleaseViewPort(viewport, sizeof(viewport)));
}

TEST_F(RendererTest, BasicActivationTest) {
  // Setup Viewport
  const char viewport_name[] = "test_view_port";
  Renderer::RegisterViewPort(viewport_name, sizeof(viewport_name),
                             kDimensionType_Absolute, 1.0f,
                             kDimensionType_Percentage, 0.25f,
                             kDimensionType_Absolute, 3.0f,
                             kDimensionType_Percentage, 0.5f,
                             0.1f, 1.0f);

  // Setup Render Target
  const char render_target_name[] = "test_render_target";
  const YRenderDevice::PixelFormat format =
      static_cast<YRenderDevice::PixelFormat>(456);
  Renderer::RegisterRenderTarget(render_target_name,
                                 sizeof(render_target_name), format,
                                 kDimensionType_Absolute, 1.0f,
                                 kDimensionType_Percentage, 0.5f);

  // Setup Render Pass
  const char render_pass_name[] = "test_render_pass";
  const char shader_variant[] = "test_variant";
  YRenderDevice::RenderBlendState blend_state;
  const char* render_targets[] = { render_target_name };
  size_t render_target_sizes[] = { sizeof(render_target_name) };
  Renderer::RegisterRenderPass(render_pass_name, sizeof(render_pass_name),
                               shader_variant, sizeof(shader_variant),
                               blend_state,
                               render_targets, render_target_sizes, 1);

  // Setup Render Passes
  const char passes_name[] = "test_render_passes";
  const char* passes_names[] = { render_pass_name };
  size_t passes_sizes[] = { sizeof(render_pass_name) };
  Renderer::RegisterRenderPasses(passes_name, sizeof(passes_name),
                                 passes_names, passes_sizes, 1);

  // Setup Render Type
  const char render_type[] = "test_render_type_name";
  const char shader[] = "test_shader_name";
  Renderer::RegisterRenderType(render_type, sizeof(render_type),
                               shader, sizeof(shader));

  // Setup Vertex Data
  const char vertex_data[] = "test_vertex_data_name";
  Renderer::RegisterVertexData(vertex_data, sizeof(vertex_data));

  // Setup Render Object
  const char render_object[] = "test_render_object";
  Renderer::RegisterRenderObject(render_object, sizeof(render_object),
                                 viewport_name, sizeof(viewport_name),
                                 render_type, sizeof(render_type),
                                 vertex_data, sizeof(vertex_data),
                                 0, nullptr, nullptr);

  Renderer::ActivateRenderPasses(passes_name, sizeof(passes_name));
  Renderer::DeactivateRenderPasses();

  EXPECT_TRUE(Renderer::ReleaseRenderObject(render_object,
                                            sizeof(render_object)));

  EXPECT_TRUE(Renderer::ReleaseRenderType(render_type, sizeof(render_type)));
  EXPECT_TRUE(Renderer::ReleaseVertexData(vertex_data, sizeof(vertex_data)));
  EXPECT_TRUE(Renderer::ReleaseRenderPasses(passes_name, sizeof(passes_name)));
  EXPECT_TRUE(Renderer::ReleaseRenderPass(render_pass_name,
                                          sizeof(render_pass_name)));
  EXPECT_TRUE(Renderer::ReleaseRenderTarget(render_target_name,
                                            sizeof(render_target_name)));
  EXPECT_TRUE(Renderer::ReleaseViewPort(viewport_name, sizeof(viewport_name)));
}

}} // namespace YEngine { namespace YRenderer {
