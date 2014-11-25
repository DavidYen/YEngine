#ifndef YENGINE_YRENDERDEVICE_VERTEXFORMAT_H
#define YENGINE_YRENDERDEVICE_VERTEXFORMAT_H

namespace YEngine { namespace YRenderDevice {

enum VertexFormat {
  kVertexFormat_PNTUV,
  kVertexFormat_PUV,

  NUM_VERTEX_FORMATS
};

// Format Definitions
struct Vertex_PNTUV {
  float position[3];
  float normal[3];
  float tangent[3];
  float texture_uv[2];
};

struct Vertex_PUV {
  float position[3];
  float texture_uv[2];
};

}} // namespace YEngine { namespace YRenderDevice {

#endif // YENGINE_YRENDERDEVICE_VERTEXFORMAT_H
