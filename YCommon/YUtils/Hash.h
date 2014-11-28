#ifndef YCOMMON_YUTILS_HASH_H
#define YCOMMON_YUTILS_HASH_H

namespace YCommon { namespace YUtils {

namespace Hash {
  uint32_t Hash32(void* data, size_t data_size);
  uint64_t Hash64(void* data, size_t data_size);
}

}} // namespace YCommon { namespace YUtils {

#endif // YCOMMON_YUTILS_HASH_H
