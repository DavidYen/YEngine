#include <YCommon/Headers/stdincludes.h>
#include "Hash.h"

#include <third_party/google/cityhash/src/city.h>

namespace YCommon { namespace YUtils {

uint32_t Hash::Hash32(void* data, size_t data_size) {
  return CityHash32(static_cast<const char*>(data), data_size);
}

uint64_t Hash::Hash64(void* data, size_t data_size) {
  return CityHash64(static_cast<const char*>(data), data_size);
}

}} // namespace YCommon { namespace YUtils {
