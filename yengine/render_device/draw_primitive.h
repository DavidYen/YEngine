#ifndef YENGINE_RENDER_DEVICE_DRAW_PRIMITIVE_H
#define YENGINE_RENDER_DEVICE_DRAW_PRIMITIVE_H

namespace yengine { namespace render_device {

enum DrawPrimitive {
  kDrawPrimitive_PointList,
  kDrawPrimitive_LineList,
  kDrawPrimitive_LineStrip,
  kDrawPrimitive_TriangleList,
  kDrawPrimitive_TriangleStrip,
  kDrawPrimitive_TriangleFan,

  NUM_DRAW_PRIMITIVES
};

static const uint32_t kPrimitiveMultiple[] = {
  1, // kDrawPrimitive_PointList,
  2, // kDrawPrimitive_LineList,
  1, // kDrawPrimitive_LineStrip,
  3, // kDrawPrimitive_TriangleList,
  1, // kDrawPrimitive_TriangleStrip,
  1, // kDrawPrimitive_TriangleFan,
};
static_assert(ARRAY_SIZE(kPrimitiveMultiple) == NUM_DRAW_PRIMITIVES,
              "kPrimitiveMultiple must be defined for every draw primitive.");

static const uint32_t kPrimitiveMinimum[] = {
  1, // kDrawPrimitive_PointList,
  2, // kDrawPrimitive_LineList,
  2, // kDrawPrimitive_LineStrip,
  3, // kDrawPrimitive_TriangleList,
  3, // kDrawPrimitive_TriangleStrip,
  3, // kDrawPrimitive_TriangleFan,
};
static_assert(ARRAY_SIZE(kPrimitiveMinimum) == NUM_DRAW_PRIMITIVES,
              "kPrimitiveMinimum must be defined for every draw primitive.");

}} // namespace yengine { namespace render_device {

#endif // YENGINE_RENDER_DEVICE_DRAW_PRIMITIVE_H
