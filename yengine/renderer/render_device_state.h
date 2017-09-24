#ifndef YENGINE_RENDERER_RENDER_DEVICE_STATE_H
#define YENGINE_RENDERER_RENDER_DEVICE_STATE_H

#include "yengine/render_device/render_device.h"

/************************
* Device state caching to help render devices which do not check if the same
* state is set multiple times, this class does no error checking so it assumes
* all maxes and ids are valid.
*************************/

namespace yengine { namespace renderer {

#define MAX_NUM_RENDER_TARGETS 4
#define MAX_NUM_SHADER_REGS 32
#define MAX_NUM_VERTEX_SAMPLERS 4
#define MAX_NUM_PIXEL_SAMPLERS 32

struct RenderDeviceState {
 public:
  RenderDeviceState() { Reset(); }
  ~RenderDeviceState() {}

  void Reset();

  void ActivateViewPort(render_device::ViewPortID id);
  void ActivateBlendState(render_device::RenderBlendStateID id);
  void ActivateRenderTarget(uint8_t target, render_device::RenderTargetID id);
  void ActivateVertexDecl(render_device::VertexDeclID id);
  void ActivateVertexFloatArg(uint8_t reg, uint8_t num_regs,
                              render_device::ConstantBufferID id);
  void ActivatePixelFloatArg(uint8_t reg, uint8_t num_regs,
                             render_device::ConstantBufferID id);
  void ActivateVertexSamplerState(uint8_t sampler,
                                  render_device::SamplerStateID id);
  void ActivatePixelSamplerState(uint8_t sampler,
                                 render_device::SamplerStateID id);
  void ActivateVertexTexture(uint8_t sampler,
                             render_device::TextureID id);
  void ActivatePixelTexture(uint8_t sampler,
                            render_device::TextureID id);
  void ActivateVertexShader(render_device::VertexShaderID id);
  void ActivatePixelShader(render_device::PixelShaderID id);
  void ActivateIndexStream(render_device::IndexBufferID id);
  void ActivateVertexStream(render_device::VertexBufferID id);

 private:
  render_device::ViewPortID mSetViewPort;
  render_device::RenderBlendStateID mSetBlendStateID;
  render_device::RenderTargetID mSetRenderTargetIDs[MAX_NUM_RENDER_TARGETS];
  render_device::VertexDeclID mSetVertexDecl;
  render_device::ConstantBufferID mSetVertexFloatArg[MAX_NUM_SHADER_REGS];
  render_device::ConstantBufferID mSetPixelFloatArg[MAX_NUM_SHADER_REGS];
  render_device::SamplerStateID mSetVertexSamplerState[MAX_NUM_VERTEX_SAMPLERS];
  render_device::SamplerStateID mSetPixelSamplerState[MAX_NUM_PIXEL_SAMPLERS];
  render_device::TextureID mSetVertexTextureArg[MAX_NUM_VERTEX_SAMPLERS];
  render_device::TextureID mSetPixelTextureArg[MAX_NUM_PIXEL_SAMPLERS];
  render_device::VertexShaderID mSetVertexShader;
  render_device::PixelShaderID mSetPixelShader;
  render_device::IndexBufferID mSetIndexStream;
  render_device::VertexBufferID mSetVertexStream;
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_RENDER_DEVICE_STATE_H
