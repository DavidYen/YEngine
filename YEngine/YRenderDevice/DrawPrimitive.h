#ifndef YENGINE_YRENDERDEVICE_DRAWPRIMITIVE_H
#define YENGINE_YRENDERDEVICE_DRAWPRIMITIVE_H

namespace YEngine { namespace YRenderDevice {

enum DrawPrimitive {
  kDrawPrimitive_PointList,
  kDrawPrimitive_LineList,
  kDrawPrimitive_LineStrip,
  kDrawPrimitive_TriangleList,
  kDrawPrimitive_TriangleStrip,
  kDrawPrimitive_TriangleFan,

  NUM_DRAW_PRIMITIVES
};

}} // namespace YEngine { namespace YRenderDevice {

#endif // YENGINE_YRENDERDEVICE_DRAWPRIMITIVE_H
