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

 private:
  YRenderDevice::ViewPortID mSetViewPort;
  YRenderDevice::RenderBlendStateID mSetBlendStateID;
  YRenderDevice::RenderTargetID mRenderTargetIDs[MAX_NUM_RENDER_TARGETS];
  YRenderDevice::ConstantBufferID mSetVertexFloatArg[MAX_NUM_SHADER_REGS];
  YRenderDevice::ConstantBufferID mSetPixelFloatArg[MAX_NUM_SHADER_REGS];
  YRenderDevice::TextureID mSetPixelTextureArg[MAX_NUM_PIXEL_SAMPLERS];
  YRenderDevice::TextureID mSetVertexTextureArg[MAX_NUM_VERTEX_SAMPLERS];
  YRenderDevice::VertexShaderID mSetVertexShader;
  YRenderDevice::PixelShaderID mSetPixelShader;
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERDEVICESTATE_H
