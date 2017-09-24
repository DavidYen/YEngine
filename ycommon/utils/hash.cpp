#include "ycommon/utils/hash.h"

#include <city.h>

namespace ycommon { namespace utils {

uint32_t Hash::Hash32(const void* data, size_t data_size) {
  return CityHash32(static_cast<const char*>(data), data_size);
}

uint64_t Hash::Hash64(const void* data, size_t data_size) {
  return CityHash64(static_cast<const char*>(data), data_size);
}

}} // namespace ycommon { namespace utils {
