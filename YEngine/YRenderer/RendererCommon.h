#ifndef YENGINE_YRENDERER_RENDERERCOMMON_H
#define YENGINE_YRENDERER_RENDERERCOMMON_H

#include <algorithm>

namespace YEngine { namespace YRenderer {

enum DimensionType {
  kDimensionType_Absolute, // Use absolute numbers.
  kDimensionType_Percentage, // Use percentage of frame buffer dimensions.
};

uint32_t GetDimensionValue(uint32_t frame_value, DimensionType type,
                           float value);

}} // namespace YEngine { namespace YRenderer {

#endif // YENGINE_YRENDERER_RENDERERCOMMON_H
