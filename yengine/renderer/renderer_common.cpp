#include "yengine/renderer/renderer_common.h"

namespace yengine { namespace renderer {

uint32_t GetDimensionValue(uint32_t frame_value, DimensionType type,
                           float value) {
  return type == kDimensionType_Percentage ?
         static_cast<uint32_t>(frame_value * value + 0.5f) :
         std::min(frame_value, static_cast<uint32_t>(value));
}

}} // namespace yengine { namespace renderer {
