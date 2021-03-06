#ifndef YENGINE_RENDER_DEVICE_RENDER_BLEND_STATE_H
#define YENGINE_RENDER_DEVICE_RENDER_BLEND_STATE_H

namespace yengine { namespace render_device {

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
  uint8_t render_target_mask;

  RenderBlendState()
    : source(kRenderBlend_Zero),
      dest(kRenderBlend_One),
      blend_op(kRenderBlendOp_Add),
      alpha_source(kRenderBlend_SrcAlpha),
      alpha_dest(kRenderBlend_DestAlpha),
      alpha_blend_op(kRenderBlendOp_Add),
      render_target_mask(0xFF) {
  }

  bool operator==(const RenderBlendState& other) const {
    return (source == other.source &&
            dest == other.dest &&
            blend_op == other.blend_op &&
            alpha_source == other.alpha_source &&
            alpha_dest == other.alpha_dest &&
            alpha_blend_op == other.alpha_blend_op &&
            render_target_mask == other.render_target_mask);
  }
};

}} // namespace yengine { namespace render_device {

#endif // YENGINE_RENDER_DEVICE_RENDER_BLEND_STATE_H
