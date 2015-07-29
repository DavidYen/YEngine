#ifndef YENGINE_YRENDERER_RENDERKEYFIELD_H
#define YENGINE_YRENDERER_RENDERKEYFIELD_H

namespace YEngine { namespace YRenderer {

enum RenderKeyFieldType {
  kRenderKeyFieldType_ViewPort,
  kRenderKeyFieldType_RenderPass,
  kRenderKeyFieldType_VertexDecl,
  kRenderKeyFieldType_Shader,
  kRenderKeyFieldType_VertexData,
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
  { 8, kRenderKeyFieldType_VertexData },
  { 6, kRenderKeyFieldType_Depth },
};

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERKEYFIELD_H
