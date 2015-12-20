#ifndef YENGINE_YRENDERDEVICE_RENDERDEVICE_MOCK_H
#define YENGINE_YRENDERDEVICE_RENDERDEVICE_MOCK_H

#include "RenderDevice.h"

namespace YEngine { namespace YRenderDevice {

namespace RenderDeviceMock {
  // System Values
  void ExpectGetNullRenderTarget(RenderTargetID ret);
  void ExpectGetBackBufferRenderTarget(RenderTargetID ret);
  void ExpectGetRenderTargetTexture(TextureID ret,
                                    RenderTargetID render_target);

  // Creates
  void ExpectCreateViewPort(ViewPortID ret,
                            uint32_t top, uint32_t left,
                            uint32_t width, uint32_t height,
                            float min_z, float max_z);
  void ExpectCreateRenderBlendState(RenderBlendStateID ret,
                                    const RenderBlendState& state);
  void ExpectCreateRenderTarget(RenderTargetID ret,
                                uint32_t width, uint32_t height,
                                PixelFormat format);
  void ExpectCreateVertexDeclaration(VertexDeclID ret,
                                     const VertexDeclElement* elements,
                                     uint32_t num_elements);
  void ExpectCreateVertexShader(VertexShaderID ret, const void* shader_data,
                                size_t shader_size);
  void ExpectCreatePixelShader(PixelShaderID ret,
                               const void* shader_data, size_t shader_size);
  void ExpectCreateSamplerState(SamplerStateID ret, const SamplerState& state);
  void ExpectCreateTexture(TextureID ret,
                           UsageType type, uint32_t width, uint32_t height,
                           uint32_t mips, PixelFormat format,
                           const void* buffer = NULL, uint32_t buffer_size = 0);
  void ExpectCreateVertexBuffer(VertexBufferID ret,
                                UsageType type, uint32_t stride,
                                uint32_t count, const void* buffer = NULL,
                                uint32_t buffer_size = 0);
  void ExpectCreateIndexBuffer(IndexBufferID ret,
                               UsageType type, uint32_t count,
                               const void* buffer = NULL,
                               uint32_t buffer_size = 0);
  void ExpectCreateConstantBuffer(ConstantBufferID ret,
                                  UsageType type, uint32_t size,
                                  const void* buffer = NULL,
                                  uint32_t buffer_size = 0);

  // Command List
  void ExpectBeginRecord();
  void ExpectEndRecord(CommandListID ret);

  // Modifiers
  void ExpectSetViewPort(ViewPortID viewport,
                         uint32_t top, uint32_t left,
                         uint32_t width, uint32_t height,
                         float min_z, float max_z);
  void ExpectFillTexture(TextureID texture,
                         const void* buffer, uint32_t size);
  void ExpectFillTextureMip(TextureID texture, uint32_t mip,
                            const void* buffer, uint32_t size);
  void ExpectResetVertexBuffer(VertexBufferID vertex_buffer);
  void ExpectAppendVertexBuffer(VertexBufferID vertex_buffer, uint32_t count,
                               const void* buffer, uint32_t buffer_size,
                               uint32_t* starting_offset);
  void ExpectFillVertexBuffer(VertexBufferID vertex_buffer, uint32_t count,
                             const void* buffer, uint32_t buffer_size,
                             uint32_t index_offset = 0);
  void ExpectFillVertexBufferInterleaved(VertexBufferID vertex_buffer,
                                         uint32_t count,
                                         uint32_t num_interleaves,
                                         const uint32_t* stride_sizes,
                                         const void* const* buffers,
                                         const uint32_t* buffer_sizes,
                                         uint32_t index_offset = 0);

  void ExpectResetIndexBuffer(IndexBufferID index_buffer);
  void ExpectAppendIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                               const void* buffer, uint32_t buffer_size,
                               uint32_t* starting_offset);
  void ExpectFillIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                             const void* buffer, uint32_t buffer_size,
                             uint16_t vertex_offset = 0,
                             uint32_t index_offset = 0);
  void ExpectFillConstantBuffer(ConstantBufferID constant_buffer,
                                const void* buffer, uint32_t size);

  // Accessors
  void ExpectGetViewPort(ViewPortID viewport,
                         uint32_t& top, uint32_t& left,
                         uint32_t& width, uint32_t& height,
                         float& min_z, float& max_z);
  void ExpectGetVertexBufferCount(uint32_t ret,
                                  VertexBufferID vertex_buffer);
  void ExpectGetIndexBufferCount(uint32_t ret,
                                 IndexBufferID index_buffer);

  // Releases
  void ExpectReleaseViewPort(ViewPortID viewport);
  void ExpectReleaseRenderBlendState(RenderBlendStateID state);
  void ExpectReleaseRenderTarget(RenderTargetID render_target);
  void ExpectReleaseVertexDeclaration(VertexDeclID vertex_decl);
  void ExpectReleaseVertexShader(VertexShaderID shader);
  void ExpectReleasePixelShader(PixelShaderID shader);
  void ExpectReleaseSamplerState(SamplerStateID state);
  void ExpectReleaseTexture(TextureID texture);
  void ExpectReleaseVertexBuffer(VertexBufferID vertex_buffer);
  void ExpectReleaseIndexBuffer(IndexBufferID index_buffer);
  void ExpectReleaseConstantBuffer(ConstantBufferID constant_buffer);
  void ExpectReleaseCommandList(CommandListID command_list);

  // Activations
  void ExpectActivateViewPort(ViewPortID viewport);
  void ExpectActivateRenderBlendState(RenderBlendStateID blend_state);
  void ExpectActivateRenderTarget(int target, RenderTargetID render_target);
  void ExpectActivateVertexDeclaration(VertexDeclID vertex_decl);
  void ExpectActivateVertexShader(VertexShaderID shader);
  void ExpectActivatePixelShader(PixelShaderID shader);
  void ExpectActivateVertexStream(uint32_t stream,
                                  VertexBufferID vertex_buffer);
  void ExpectActivateIndexStream(IndexBufferID index_buffer);
  void ExpectActivateVertexConstantBuffer(int start_reg,
                                          ConstantBufferID constant_buffer);
  void ExpectActivatePixelConstantBuffer(int start_reg,
                                         ConstantBufferID constant_buffer);
  void ExpectActivateVertexSamplerState(int sampler,
                                        SamplerStateID sampler_state);
  void ExpectActivatePixelSamplerState(int sampler,
                                       SamplerStateID sampler_state);
  void ExpectActivateVertexTexture(int sampler, TextureID texture);
  void ExpectActivatePixelTexture(int sampler, TextureID texture);
  void ExpectActivateDrawPrimitive(DrawPrimitive draw_primitive);

  // Draw
  void ExpectDraw(uint32_t start_vertex, uint32_t num_verts);
  void ExpectDrawInstanced(uint32_t start_vertex, uint32_t verts_per_instance,
                           uint32_t start_instance, uint32_t num_instances);
  void ExpectDrawIndexed(uint32_t start_index, uint32_t num_indexes,
                         uint32_t vertex_offset = 0);
  void ExpectDrawIndexedInstanced(uint32_t start_index,
                                  uint32_t index_per_instance,
                                  uint32_t start_instance,
                                  uint32_t num_instances,
                                  uint32_t vertex_offset = 0);

  // Render
  void ExpectExecuteCommandList(CommandListID commands);
  void ExpectPresent();
}

}} // namespace YEngine { namespace YRenderDevice {

#endif // YENGINE_YRENDERDEVICE_RENDERDEVICE_MOCK_H
