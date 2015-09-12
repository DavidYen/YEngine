#ifndef YENGINE_YRENDERER_RENDERDEVICESTATE_H
#define YENGINE_YRENDERER_RENDERDEVICESTATE_H

#include <YEngine/YRenderDevice/RenderDevice.h>

/************************
* Device state caching to help render devices which do not check if the same
* state is set multiple times, this class does no error checking so it assumes
* all maxes and ids are valid.
*************************/

namespace YEngine { namespace YRenderer {

#define MAX_NUM_RENDER_TARGETS 4
#define MAX_NUM_SHADER_REGS 32
#define MAX_NUM_VERTEX_SAMPLERS 4
#define MAX_NUM_PIXEL_SAMPLERS 32

struct RenderDeviceState {
 public:
  RenderDeviceState() { Reset(); }
  ~RenderDeviceState() {}

  void Reset();

  void ActivateViewPort(YRenderDevice::ViewPortID id);
  void ActivateBlendState(YRenderDevice::RenderBlendStateID id);
  void ActivateRenderTarget(uint8_t target, YRenderDevice::RenderTargetID id);
  void ActivateVertexDecl(YRenderDevice::VertexDeclID id);
  void ActivateVertexFloatArg(uint8_t reg, uint8_t num_regs,
                              YRenderDevice::ConstantBufferID id);
  void ActivatePixelFloatArg(uint8_t reg, uint8_t num_regs,
                             YRenderDevice::ConstantBufferID id);
  void ActivateVertexSamplerState(uint8_t sampler,
                                  YRenderDevice::SamplerStateID id);
  void ActivatePixelSamplerState(uint8_t sampler,
                                 YRenderDevice::SamplerStateID id);
  void ActivateVertexTexture(uint8_t sampler,
                             YRenderDevice::TextureID id);
  void ActivatePixelTexture(uint8_t sampler,
                            YRenderDevice::TextureID id);
  void ActivateVertexShader(YRenderDevice::VertexShaderID id);
  void ActivatePixelShader(YRenderDevice::PixelShaderID id);
  void ActivateIndexStream(YRenderDevice::IndexBufferID id);
  void ActivateVertexStream(YRenderDevice::VertexBufferID id);

 private:
  YRenderDevice::ViewPortID mSetViewPort;
  YRenderDevice::RenderBlendStateID mSetBlendStateID;
  YRenderDevice::RenderTargetID mSetRenderTargetIDs[MAX_NUM_RENDER_TARGETS];
  YRenderDevice::VertexDeclID mSetVertexDecl;
  YRenderDevice::ConstantBufferID mSetVertexFloatArg[MAX_NUM_SHADER_REGS];
  YRenderDevice::ConstantBufferID mSetPixelFloatArg[MAX_NUM_SHADER_REGS];
  YRenderDevice::SamplerStateID mSetVertexSamplerState[MAX_NUM_VERTEX_SAMPLERS];
  YRenderDevice::SamplerStateID mSetPixelSamplerState[MAX_NUM_PIXEL_SAMPLERS];
  YRenderDevice::TextureID mSetVertexTextureArg[MAX_NUM_VERTEX_SAMPLERS];
  YRenderDevice::TextureID mSetPixelTextureArg[MAX_NUM_PIXEL_SAMPLERS];
  YRenderDevice::VertexShaderID mSetVertexShader;
  YRenderDevice::PixelShaderID mSetPixelShader;
  YRenderDevice::IndexBufferID mSetIndexStream;
  YRenderDevice::VertexBufferID mSetVertexStream;
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERDEVICESTATE_H
