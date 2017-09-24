#ifndef YENGINE_RENDERER_RENDER_KEY_FIELD_H
#define YENGINE_RENDERER_RENDER_KEY_FIELD_H

namespace yengine { namespace renderer {

enum RenderKeyFieldType {
  kRenderKeyFieldType_ViewPort,
  kRenderKeyFieldType_RenderPass,
  kRenderKeyFieldType_VertexDecl,
  kRenderKeyFieldType_Shader,
  kRenderKeyFieldType_Depth,
  kRenderKeyFieldType_ArbitraryNumber,

  NUM_RENDER_KEY_FIELD_TYPES
};

struct RenderKeyField {
  uint8_t field_bits;
  RenderKeyFieldType field_type;
};

static const RenderKeyField kDefaultRenderKeyFields[] = {
  { 2, kRenderKeyFieldType_ViewPort },
  { 4, kRenderKeyFieldType_RenderPass },
  { 6, kRenderKeyFieldType_VertexDecl },
  { 6, kRenderKeyFieldType_Shader },
  { 6, kRenderKeyFieldType_Depth },
};

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_RENDER_KEY_FIELD_H
