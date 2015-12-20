#ifndef YENGINE_YRENDERDEVICE_RENDERDEVICE_H
#define YENGINE_YRENDERDEVICE_RENDERDEVICE_H

#include "PixelFormat.h"
#include "VertexFormat.h"
#include "DrawPrimitive.h"

namespace YCommon { namespace YPlatform {
  struct PlatformHandle;
}}

namespace YEngine { namespace YRenderDevice {

typedef uint8_t ViewPortID;
typedef uint8_t RenderBlendStateID;
typedef uint8_t RenderTargetID;
typedef uint8_t VertexDeclID;
typedef uint8_t VertexShaderID;
typedef uint8_t PixelShaderID;
typedef uint8_t SamplerStateID;
typedef uint8_t CommandListID;
typedef uint16_t TextureID;
typedef uint16_t VertexBufferID;
typedef uint16_t IndexBufferID;
typedef uint16_t ConstantBufferID;

struct RenderBlendState;
struct SamplerState;
struct VertexDeclElement;

enum UsageType {
  kUsageType_Invalid,   // Invalid
  kUsageType_Immutable, // Never changing
  kUsageType_Static,    // Changing infrequently (< 1 per frame)
  kUsageType_Dynamic,   // Changing frequently (> 1 per frame)
  kUsageType_System,    // Not meant for user use

  NUM_USAGE_TYPES
};

namespace RenderDevice {
  void Initialize(const YCommon::YPlatform::PlatformHandle& handle,
                  uint32_t width, uint32_t height,
                  void* buffer, size_t buffer_size);
  void Terminate();

  // System Values
  RenderTargetID GetNullRenderTarget();
  RenderTargetID GetBackBufferRenderTarget();
  TextureID GetRenderTargetTexture(RenderTargetID render_target);

  // Creates
  ViewPortID CreateViewPort(uint32_t top, uint32_t left,
                            uint32_t width, uint32_t height,
                            float min_z, float max_z);
  RenderBlendStateID CreateRenderBlendState(const RenderBlendState& state);
  RenderTargetID CreateRenderTarget(uint32_t width, uint32_t height,
                                    PixelFormat format);
  VertexDeclID CreateVertexDeclaration(const VertexDeclElement* elements,
                                       uint32_t num_elements);
  VertexShaderID CreateVertexShader(const void* shader_data,
                                    size_t shader_size);
  PixelShaderID CreatePixelShader(const void* shader_data, size_t shader_size);
  SamplerStateID CreateSamplerState(const SamplerState& state);
  TextureID CreateTexture(UsageType type, uint32_t width, uint32_t height,
                          uint32_t mips, PixelFormat format,
                          const void* buffer = NULL, uint32_t buffer_size = 0);
  VertexBufferID CreateVertexBuffer(UsageType type, uint32_t stride,
                                    uint32_t count, const void* buffer = NULL,
                                    uint32_t buffer_size = 0);
  IndexBufferID CreateIndexBuffer(UsageType type, uint32_t count,
                                  const void* buffer = NULL,
                                  uint32_t buffer_size = 0);
  ConstantBufferID CreateConstantBuffer(UsageType type, uint32_t size,
                                        const void* buffer = NULL,
                                        uint32_t buffer_size = 0);

  // Command List
  void BeginRecord();
  CommandListID EndRecord();

  // Modifiers
  void SetViewPort(ViewPortID viewport,
                   uint32_t top, uint32_t left,
                   uint32_t width, uint32_t height,
                   float min_z, float max_z);
  void FillTexture(TextureID texture,
                   const void* buffer, uint32_t size);
  void FillTextureMip(TextureID texture, uint32_t mip,
                      const void* buffer, uint32_t size);
  void ResetVertexBuffer(VertexBufferID vertex_buffer);
  void FillVertexBuffer(VertexBufferID vertex_buffer, uint32_t count,
                        const void* buffer, uint32_t buffer_size,
                        uint32_t index_offset = 0);
  void FillVertexBufferInterleaved(VertexBufferID vertex_buffer,
                                   uint32_t count,
                                   uint32_t num_interleaves,
                                   const uint32_t* stride_sizes,
                                   const void* const* buffers,
                                   const uint32_t* buffer_sizes,
                                   uint32_t index_offset = 0);
  void ResetIndexBuffer(IndexBufferID index_buffer);
  void AppendIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                        const void* buffer, uint32_t buffer_size,
                        const uint32_t* starting_offset);
  void FillIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                       const void* buffer, uint32_t buffer_size,
                       uint16_t vertex_offset = 0,
                       uint32_t index_offset = 0);
  void FillConstantBuffer(ConstantBufferID constant_buffer,
                          const void* buffer, uint32_t size);

  // Accessors
  void GetFrameBufferDimensions(uint32_t& width, uint32_t& height);
  void GetViewPort(ViewPortID viewport,
                   uint32_t& top, uint32_t& left,
                   uint32_t& width, uint32_t& height,
                   float& min_z, float& max_z);
  uint32_t GetVertexBufferCount(VertexBufferID vertex_buffer);
  uint32_t GetIndexBufferCount(IndexBufferID index_buffer);

  // Releases
  void ReleaseViewPort(ViewPortID viewport);
  void ReleaseRenderBlendState(RenderBlendStateID state);
  void ReleaseRenderTarget(RenderTargetID render_target);
  void ReleaseVertexDeclaration(VertexDeclID vertex_decl);
  void ReleaseVertexShader(VertexShaderID shader);
  void ReleasePixelShader(PixelShaderID shader);
  void ReleaseSamplerState(SamplerStateID state);
  void ReleaseTexture(TextureID texture);
  void ReleaseVertexBuffer(VertexBufferID vertex_buffer);
  void ReleaseIndexBuffer(IndexBufferID index_buffer);
  void ReleaseConstantBuffer(ConstantBufferID constant_buffer);
  void ReleaseCommandList(CommandListID command_list);

  // Activations
  void ActivateViewPort(ViewPortID viewport);
  void ActivateRenderBlendState(RenderBlendStateID blend_state);
  void ActivateRenderTarget(int target, RenderTargetID render_target);
  void ActivateVertexDeclaration(VertexDeclID vertex_decl);
  void ActivateVertexShader(VertexShaderID shader);
  void ActivatePixelShader(PixelShaderID shader);
  void ActivateVertexStream(uint32_t stream, VertexBufferID vertex_buffer);
  void ActivateIndexStream(IndexBufferID index_buffer);
  void ActivateVertexConstantBuffer(int start_reg,
                                    ConstantBufferID constant_buffer);
  void ActivatePixelConstantBuffer(int start_reg,
                                   ConstantBufferID constant_buffer);
  void ActivateVertexSamplerState(int sampler, SamplerStateID sampler_state);
  void ActivatePixelSamplerState(int sampler, SamplerStateID sampler_state);
  void ActivateVertexTexture(int sampler, TextureID texture);
  void ActivatePixelTexture(int sampler, TextureID texture);
  void ActivateDrawPrimitive(DrawPrimitive draw_primitive);

  // Draw
  void Draw(uint32_t start_vertex, uint32_t num_verts);
  void DrawInstanced(uint32_t start_vertex, uint32_t verts_per_instance,
                     uint32_t start_instance, uint32_t num_instances);
  void DrawIndexed(uint32_t start_index, uint32_t num_indexes,
                   uint32_t vertex_offset = 0);
  void DrawIndexedInstanced(uint32_t start_index, uint32_t index_per_instance,
                            uint32_t start_instance, uint32_t num_instances,
                            uint32_t vertex_offset = 0);

  // Render
  void ExecuteCommandList(CommandListID commands);
  void Present();
}

}} // namespace YEngine { namespace YRenderDevice {

#endif // YENGINE_YRENDERDEVICE_RENDERDEVICE_H
