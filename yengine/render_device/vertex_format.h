#ifndef YENGINE_RENDER_DEVICE_VERTEX_FORMAT_H
#define YENGINE_RENDER_DEVICE_VERTEX_FORMAT_H

namespace yengine { namespace render_device {

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

}} // namespace yengine { namespace render_device {

#endif // YENGINE_RENDER_DEVICE_VERTEX_FORMAT_H
