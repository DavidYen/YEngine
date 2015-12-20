#include <YCommon/Headers/stdincludes.h>
#include "RenderDevice.h"

// Temporarily disable unreferenced parameter during implementation
#pragma warning(disable : 4100)

namespace YEngine { namespace YRenderDevice {

void RenderDevice::Initialize(const YCommon::YPlatform::PlatformHandle& handle,
                              uint32_t width, uint32_t height,
                              void* buffer, size_t buffer_size) {
}

void RenderDevice::Terminate() {
}

// System Values
RenderTargetID RenderDevice::GetNullRenderTarget() {
  return static_cast<RenderTargetID>(-1);
}

RenderTargetID RenderDevice::GetBackBufferRenderTarget() {
  return static_cast<RenderTargetID>(-1);
}

TextureID RenderDevice::GetRenderTargetTexture(RenderTargetID render_target) {
  return static_cast<TextureID>(-1);
}

// Creates
ViewPortID RenderDevice::CreateViewPort(uint32_t top, uint32_t left,
                                        uint32_t width, uint32_t height,
                                        float min_z, float max_z) {
  return static_cast<ViewPortID>(-1);
}

RenderBlendStateID RenderDevice::CreateRenderBlendState(const RenderBlendState& state) {
  return static_cast<ViewPortID>(-1);
}

RenderTargetID RenderDevice::CreateRenderTarget(uint32_t width, uint32_t height,
                                                PixelFormat format) {
  return static_cast<RenderTargetID>(-1);
}

VertexDeclID RenderDevice::CreateVertexDeclaration(const VertexDeclElement* elements,
                                                   uint32_t num_elements) {
  return static_cast<RenderTargetID>(-1);
}

VertexShaderID RenderDevice::CreateVertexShader(const void* shader_data,
                                                size_t shader_size) {
  return static_cast<VertexShaderID>(-1);
}

PixelShaderID RenderDevice::CreatePixelShader(const void* shader_data,
                                              size_t shader_size) {
  return 0;
}

SamplerStateID RenderDevice::CreateSamplerState(const SamplerState& state) {
  return 0;
}

TextureID RenderDevice::CreateTexture(UsageType type, uint32_t width, uint32_t height,
                        uint32_t mips, PixelFormat format,
                        const void* buffer, uint32_t buffer_size) {
  return 0;
}

VertexBufferID RenderDevice::CreateVertexBuffer(UsageType type, uint32_t stride,
                                  uint32_t count, const void* buffer,
                                  uint32_t buffer_size) {
  return 0;
}

IndexBufferID RenderDevice::CreateIndexBuffer(UsageType type, uint32_t count,
                                              const void* buffer,
                                              uint32_t buffer_size) {
  return 0;
}

ConstantBufferID RenderDevice::CreateConstantBuffer(UsageType type, uint32_t size,
                                                    const void* buffer,
                                                    uint32_t buffer_size) {
  return 0;
}

// Command List
void RenderDevice::BeginRecord() {
}
CommandListID RenderDevice::EndRecord() {
  return 0;
}

// Modifiers
void RenderDevice::SetViewPort(ViewPortID viewport,
                 uint32_t top, uint32_t left,
                 uint32_t width, uint32_t height,
                 float min_z, float max_z) {
}
void RenderDevice::FillTexture(TextureID texture,
                 const void* buffer, uint32_t size) {
}
void RenderDevice::FillTextureMip(TextureID texture, uint32_t mip,
                    const void* buffer, uint32_t size) {
}
void RenderDevice::ResetVertexBuffer(VertexBufferID vertex_buffer) {
}
void RenderDevice::FillVertexBuffer(VertexBufferID vertex_buffer, uint32_t count,
                      const void* buffer, uint32_t buffer_size,
                      uint32_t index_offset) {
}
void RenderDevice::FillVertexBufferInterleaved(VertexBufferID vertex_buffer,
                                 uint32_t count,
                                 uint32_t num_interleaves,
                                 const uint32_t* stride_sizes,
                                 const void* const* buffers,
                                 const uint32_t* buffer_sizes,
                                 uint32_t index_offset) {
}
void RenderDevice::ResetIndexBuffer(IndexBufferID index_buffer) {
}
void RenderDevice::AppendIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                     const void* buffer, uint32_t buffer_size,
                                     const uint32_t* starting_offset) {
}
void RenderDevice::FillIndexBuffer(IndexBufferID index_buffer, uint32_t count,
                                   const void* buffer, uint32_t buffer_size,
                                   uint16_t vertex_offset,
                                   uint32_t index_offset) {
}
void RenderDevice::FillConstantBuffer(ConstantBufferID constant_buffer,
                                      const void* buffer, uint32_t size) {
}

// Accessors
void RenderDevice::GetFrameBufferDimensions(uint32_t& width, uint32_t& height) {
}
void RenderDevice::GetViewPort(ViewPortID viewport,
                               uint32_t& top, uint32_t& left,
                               uint32_t& width, uint32_t& height,
                               float& min_z, float& max_z) {
}
uint32_t GetVertexBufferCount(VertexBufferID vertex_buffer) {
  return 0;
}
uint32_t GetIndexBufferCount(IndexBufferID index_buffer) {
  return 0;
}

// Releases
void RenderDevice::ReleaseViewPort(ViewPortID viewport) {
}
void RenderDevice::ReleaseRenderBlendState(RenderBlendStateID state) {
}
void RenderDevice::ReleaseRenderTarget(RenderTargetID render_target) {
}
void RenderDevice::ReleaseVertexDeclaration(VertexDeclID vertex_decl) {
}
void RenderDevice::ReleaseVertexShader(VertexShaderID shader) {
}
void RenderDevice::ReleasePixelShader(PixelShaderID shader) {
}
void RenderDevice::ReleaseSamplerState(SamplerStateID state) {
}
void RenderDevice::ReleaseTexture(TextureID texture) {
}
void RenderDevice::ReleaseVertexBuffer(VertexBufferID vertex_buffer) {
}
void RenderDevice::ReleaseIndexBuffer(IndexBufferID index_buffer) {
}
void RenderDevice::ReleaseConstantBuffer(ConstantBufferID constant_buffer) {
}
void RenderDevice::ReleaseCommandList(CommandListID command_list) {
}

// Activations
void RenderDevice::ActivateViewPort(ViewPortID viewport) {
}
void RenderDevice::ActivateRenderBlendState(RenderBlendStateID blend_state) {
}
void RenderDevice::ActivateRenderTarget(int target, RenderTargetID render_target) {
}
void RenderDevice::ActivateVertexDeclaration(VertexDeclID vertex_decl) {
}
void RenderDevice::ActivateVertexShader(VertexShaderID shader) {
}
void RenderDevice::ActivatePixelShader(PixelShaderID shader) {
}
void RenderDevice::ActivateVertexStream(uint32_t stream, VertexBufferID vertex_buffer) {
}
void RenderDevice::ActivateIndexStream(IndexBufferID index_buffer) {
}
void RenderDevice::ActivateVertexConstantBuffer(int start_reg,
                                  ConstantBufferID constant_buffer) {
}
void RenderDevice::ActivatePixelConstantBuffer(int start_reg,
                                 ConstantBufferID constant_buffer) {
}
void RenderDevice::ActivateVertexSamplerState(int sampler, SamplerStateID sampler_state) {
}
void RenderDevice::ActivatePixelSamplerState(int sampler, SamplerStateID sampler_state) {
}
void RenderDevice::ActivateVertexTexture(int sampler, TextureID texture) {
}
void RenderDevice::ActivatePixelTexture(int sampler, TextureID texture) {
}
void RenderDevice::ActivateDrawPrimitive(DrawPrimitive draw_primitive) {
}

// Draw
void RenderDevice::Draw(uint32_t start_vertex, uint32_t num_verts) {
}
void RenderDevice::DrawInstanced(uint32_t start_vertex, uint32_t verts_per_instance,
                   uint32_t start_instance, uint32_t num_instances) {
}
void RenderDevice::DrawIndexed(uint32_t start_index, uint32_t num_indexes,
                 uint32_t vertex_offset) {
}
void RenderDevice::DrawIndexedInstanced(uint32_t start_index, uint32_t index_per_instance,
                          uint32_t start_instance, uint32_t num_instances,
                          uint32_t vertex_offset) {
}

// Render
void RenderDevice::ExecuteCommandList(CommandListID commands) {
}
void RenderDevice::Present() {
}

}} // namespace YEngine { namespace YRenderDevice {
