#ifndef YENGINE_YRENDERDEVICE_RENDERBLENDSTATE_H
#define YENGINE_YRENDERDEVICE_RENDERBLENDSTATE_H

namespace YEngine { namespace YRenderDevice {

enum RenderBlend {
  kRenderBlend_Zero,
  kRenderBlend_One,
  kRenderBlend_SrcColor,
  kRenderBlend_InvSrcColor,
  kRenderBlend_DestColor,
  kRenderBlend_InvDestColor,
  kRenderBlend_SrcAlpha,
  kRenderBlend_InvSrcAlpha,
  kRenderBlend_DestAlpha,
  kRenderBlend_InvDestAlpha,

  NUM_RENDER_BLENDS
};

enum RenderBlendOp {
  kRenderBlendOp_Add,
  kRenderBlendOp_Subtract,
  kRenderBlendOp_RevSubtract,
  kRenderBlendOp_Min,
  kRenderBlendOp_Max,

  NUM_RENDER_BLEND_OPS
};

struct RenderBlendState {
  RenderBlend source;
  RenderBlend dest;
  RenderBlendOp blend_op;
  RenderBlend alpha_source;
  RenderBlend alpha_dest;
  RenderBlendOp alpha_blend_op;
  uint8_t RenderTargetMask;
};

}} // namespace YEngine { namespace YRenderDevice {

#endif // YENGINE_YRENDERDEVICE_RENDERBLENDSTATE_H
