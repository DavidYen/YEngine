#ifndef YENGINE_RENDERER_RENDERER_COMMON_H
#define YENGINE_RENDERER_RENDERER_COMMON_H

#include <algorithm>

namespace yengine { namespace renderer {

enum DimensionType {
  kDimensionType_Absolute, // Use absolute numbers.
  kDimensionType_Percentage, // Use percentage of frame buffer dimensions.
};

uint32_t GetDimensionValue(uint32_t frame_value, DimensionType type,
                           float value);

}} // namespace yengine { namespace renderer {

#endif // YENGINE_RENDERER_RENDERER_COMMON_H
