#include "yengine/renderer/render_device_state.h"

#include <string.h>

namespace yengine { namespace renderer {

void RenderDeviceState::Reset() {
  memset(this, -1, sizeof(*this));
}

void RenderDeviceState::ActivateViewPort(render_device::ViewPortID id) {
  if (mSetViewPort != id) {
    render_device::RenderDevice::ActivateViewPort(id);
    mSetViewPort = id;
  }
}

void RenderDeviceState::ActivateBlendState(
    render_device::RenderBlendStateID id) {
  if (mSetBlendStateID != id) {
    render_device::RenderDevice::ActivateRenderBlendState(id);
    mSetBlendStateID = id;
  }
}

void RenderDeviceState::ActivateRenderTarget(uint8_t target,
                                             render_device::RenderTargetID id) {
  if (mSetRenderTargetIDs[target] != id) {
    render_device::RenderDevice::ActivateRenderTarget(target, id);
    mSetRenderTargetIDs[target] = id;
  }
}

void RenderDeviceState::ActivateVertexDecl(render_device::VertexDeclID id) {
  if (mSetVertexDecl != id) {
    render_device::RenderDevice::ActivateVertexDeclaration(id);
    mSetVertexDecl = id;
  }
}

void RenderDeviceState::ActivateVertexFloatArg(
    uint8_t reg, uint8_t num_regs, render_device::ConstantBufferID id) {
  bool update = false;
  for (uint8_t i = 0; i < num_regs; ++i) {
    if (mSetVertexFloatArg[reg + i] != id) {
      mSetVertexFloatArg[reg + i] = id;
      update = true;
    }
  }

  if (update)
    render_device::RenderDevice::ActivateVertexConstantBuffer(reg, id);
}

void RenderDeviceState::ActivatePixelFloatArg(
    uint8_t reg, uint8_t num_regs, render_device::ConstantBufferID id) {
  bool update = false;
  for (uint8_t i = 0; i < num_regs; ++i) {
    if (mSetPixelFloatArg[reg + i] != id) {
      mSetPixelFloatArg[reg + i] = id;
      update = true;
    }
  }

  if (update)
    render_device::RenderDevice::ActivatePixelConstantBuffer(reg, id);
}

void RenderDeviceState::ActivateVertexSamplerState(
    uint8_t sampler, render_device::SamplerStateID id) {
  if (mSetVertexSamplerState[sampler] != id) {
    render_device::RenderDevice::ActivateVertexSamplerState(sampler, id);
    mSetVertexSamplerState[sampler] = id;
  }
}

void RenderDeviceState::ActivatePixelSamplerState(
    uint8_t sampler, render_device::SamplerStateID id) {
  if (mSetPixelSamplerState[sampler] != id) {
    render_device::RenderDevice::ActivatePixelSamplerState(sampler, id);
    mSetPixelSamplerState[sampler] = id;
  }
}

void RenderDeviceState::ActivateVertexTexture(
    uint8_t sampler, render_device::TextureID id) {
  if (mSetVertexTextureArg[sampler] != id) {
    render_device::RenderDevice::ActivateVertexTexture(sampler, id);
    mSetVertexTextureArg[sampler] = id;
  }
}

void RenderDeviceState::ActivatePixelTexture(
    uint8_t sampler, render_device::TextureID id) {
  if (mSetPixelTextureArg[sampler] != id) {
    render_device::RenderDevice::ActivatePixelTexture(sampler, id);
    mSetPixelTextureArg[sampler] = id;
  }
}

void RenderDeviceState::ActivateVertexShader(render_device::VertexShaderID id) {
  if (mSetVertexShader != id) {
    render_device::RenderDevice::ActivateVertexShader(id);
    mSetVertexShader = id;
  }
}

void RenderDeviceState::ActivatePixelShader(render_device::PixelShaderID id) {
  if (mSetPixelShader != id) {
    render_device::RenderDevice::ActivatePixelShader(id);
    mSetPixelShader = id;
  }
}

void RenderDeviceState::ActivateIndexStream(render_device::IndexBufferID id) {
  if (mSetIndexStream != id) {
    render_device::RenderDevice::ActivateIndexStream(id);
    mSetIndexStream = id;
  }
}

void RenderDeviceState::ActivateVertexStream(render_device::VertexBufferID id) {
  if (mSetVertexStream != id) {
    render_device::RenderDevice::ActivateVertexStream(0, id);
    mSetVertexStream = id;
  }
}

}} // namespace yengine { namespace renderer {
