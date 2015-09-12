#include <YCommon/Headers/stdincludes.h>
#include "RenderDeviceState.h"

#include <string.h>

namespace YEngine { namespace YRenderer {

void RenderDeviceState::Reset() {
  memset(this, -1, sizeof(*this));
}

void RenderDeviceState::ActivateViewPort(YRenderDevice::ViewPortID id) {
  if (mSetViewPort != id) {
    YRenderDevice::RenderDevice::ActivateViewPort(id);
    mSetViewPort = id;
  }
}

void RenderDeviceState::ActivateBlendState(
    YRenderDevice::RenderBlendStateID id) {
  if (mSetBlendStateID != id) {
    YRenderDevice::RenderDevice::ActivateRenderBlendState(id);
    mSetBlendStateID = id;
  }
}

void RenderDeviceState::ActivateRenderTarget(uint8_t target,
                                             YRenderDevice::RenderTargetID id) {
  if (mSetRenderTargetIDs[target] != id) {
    YRenderDevice::RenderDevice::ActivateRenderTarget(target, id);
    mSetRenderTargetIDs[target] = id;
  }
}

void RenderDeviceState::ActivateVertexDecl(YRenderDevice::VertexDeclID id) {
  if (mSetVertexDecl != id) {
    YRenderDevice::RenderDevice::ActivateVertexDeclaration(id);
    mSetVertexDecl = id;
  }
}

void RenderDeviceState::ActivateVertexFloatArg(
    uint8_t reg, uint8_t num_regs, YRenderDevice::ConstantBufferID id) {
  bool update = false;
  for (uint8_t i = 0; i < num_regs; ++i) {
    if (mSetVertexFloatArg[reg + i] != id) {
      mSetVertexFloatArg[reg + i] = id;
      update = true;
    }
  }

  if (update)
    YRenderDevice::RenderDevice::ActivateVertexConstantBuffer(reg, id);
}

void RenderDeviceState::ActivatePixelFloatArg(
    uint8_t reg, uint8_t num_regs, YRenderDevice::ConstantBufferID id) {
  bool update = false;
  for (uint8_t i = 0; i < num_regs; ++i) {
    if (mSetPixelFloatArg[reg + i] != id) {
      mSetPixelFloatArg[reg + i] = id;
      update = true;
    }
  }

  if (update)
    YRenderDevice::RenderDevice::ActivatePixelConstantBuffer(reg, id);
}

void RenderDeviceState::ActivateVertexSamplerState(
    uint8_t sampler, YRenderDevice::SamplerStateID id) {
  if (mSetVertexSamplerState[sampler] != id) {
    YRenderDevice::RenderDevice::ActivateVertexSamplerState(sampler, id);
    mSetVertexSamplerState[sampler] = id;
  }
}

void RenderDeviceState::ActivatePixelSamplerState(
    uint8_t sampler, YRenderDevice::SamplerStateID id) {
  if (mSetPixelSamplerState[sampler] != id) {
    YRenderDevice::RenderDevice::ActivatePixelSamplerState(sampler, id);
    mSetPixelSamplerState[sampler] = id;
  }
}

void RenderDeviceState::ActivateVertexTexture(
    uint8_t sampler, YRenderDevice::TextureID id) {
  if (mSetVertexTextureArg[sampler] != id) {
    YRenderDevice::RenderDevice::ActivateVertexTexture(sampler, id);
    mSetVertexTextureArg[sampler] = id;
  }
}

void RenderDeviceState::ActivatePixelTexture(
    uint8_t sampler, YRenderDevice::TextureID id) {
  if (mSetPixelTextureArg[sampler] != id) {
    YRenderDevice::RenderDevice::ActivatePixelTexture(sampler, id);
    mSetPixelTextureArg[sampler] = id;
  }
}

void RenderDeviceState::ActivateVertexShader(YRenderDevice::VertexShaderID id) {
  if (mSetVertexShader != id) {
    YRenderDevice::RenderDevice::ActivateVertexShader(id);
    mSetVertexShader = id;
  }
}

void RenderDeviceState::ActivatePixelShader(YRenderDevice::PixelShaderID id) {
  if (mSetPixelShader != id) {
    YRenderDevice::RenderDevice::ActivatePixelShader(id);
    mSetPixelShader = id;
  }
}

void RenderDeviceState::ActivateIndexStream(YRenderDevice::IndexBufferID id) {
  if (mSetIndexStream != id) {
    YRenderDevice::RenderDevice::ActivateIndexStream(id);
    mSetIndexStream = id;
  }
}

void RenderDeviceState::ActivateVertexStream(YRenderDevice::VertexBufferID id) {
  if (mSetVertexStream != id) {
    YRenderDevice::RenderDevice::ActivateVertexStream(0, id);
    mSetVertexStream = id;
  }
}

}} // namespace YEngine { namespace YRenderer {
