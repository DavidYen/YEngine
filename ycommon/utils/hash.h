#ifndef YCOMMON_UTILS_HASH_H
#define YCOMMON_UTILS_HASH_H

#include <stdint.h>

namespace ycommon { namespace utils {

namespace Hash {
  uint32_t Hash32(const void* data, size_t data_size);
  uint64_t Hash64(const void* data, size_t data_size);
}

}} // namespace ycommon { namespace utils {

#endif // YCOMMON_UTILS_HASH_H
