#include <YCommon/Headers/stdincludes.h>
#include "ShaderData.h"

#include <gtest/gtest.h>

#include <YEngine/YRenderDevice/RenderDevice_Mock.h>

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

}} // namespace YEngine { namespace YRenderer {
