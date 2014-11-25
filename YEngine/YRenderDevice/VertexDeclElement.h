#ifndef YENGINE_YRENDERDEVICE_VERTEXDECLELEMENT_H
#define YENGINE_YRENDERDEVICE_VERTEXDECLELEMENT_H

namespace YEngine { namespace YRenderDevice {

enum VertexElementType {
  kVertexElementType_Float,
  kVertexElementType_Float2,
  kVertexElementType_Float3,
  kVertexElementType_Float4,

  NUM_VERTEX_ELEMENT_TYPES
};

enum VertexElementUsage {
  kVertexElementUsage_Position,
  kVertexElementUsage_Position1,
  kVertexElementUsage_Normal,
  kVertexElementUsage_Normal1,
  kVertexElementUsage_Binormal,
  kVertexElementUsage_BiNormal1,
  kVertexElementUsage_Tanget,
  kVertexElementUsage_Tanget1,
  kVertexElementUsage_TexCoord,
  kVertexElementUsage_TexCoord1,

  NUM_VERTEX_ELEMENT_USAGES
};

struct VertexDeclElement {
  uint8_t mStreamNum;
  uint8_t mElementOffset;
  uint8_t mInstanceDivisor; // 1 for non-instance items
  VertexElementType mElementType;
  VertexElementUsage mElementUsage;
};

}}

#endif // YENGINE_YRENDERDEVICE_VERTEXDECLELEMENT_H
