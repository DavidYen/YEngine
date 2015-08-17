#include <YCommon/Headers/stdincludes.h>
#include "RendererCommon.h"

namespace YEngine { namespace YRenderer {

uint32_t GetDimensionValue(uint32_t frame_value, DimensionType type,
                           float value) {
  return type == kDimensionType_Percentage ?
         static_cast<uint32_t>(frame_value * value + 0.5f) :
         std::min(frame_value, static_cast<uint32_t>(value));
}

}} // namespace YEngine { namespace YRenderer {
