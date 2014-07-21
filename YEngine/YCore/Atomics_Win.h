
#include "Windows.h"

namespace ycore { namespace atomic {

/* Memory Barrier. */
inline void MemoryBarrier() {
  ::MemoryBarrier();
}

/* Sets and returns previous value. */
inline int32_t AtomicSet32(volatile int32_t* dest, int32_t value) {
  return InterlockedExchange(dest, value);
}

inline int64_t AtomicSet64(volatile int64_t* dest, int64_t value) {
  return InterlockedExchange64(dest, value);
}

inline void* AtomicSetPtr(volatile void* dest, int64_t value) {
  return InterlockedExchangePointer(dest, value);
}

/* Compares dest with expected and sets to value if they match. */
inline bool AtomicCmpSet(volatile int32_t* dest, int32_t expected,
                         int32_t value) {
  return InterlockedCompareExchange(dest, value, expected) == expected;
}
inline bool AtomicCmpSet(volatile int64_t* dest, int64_t expected,
                         int64_t value) {
  return InterlockedCompareExchange64(dest, value, expected) == expected;
}
inline bool AtomicCmpSet(volatile void* dest, void* expected,
                         void* value) {
  return InterlockedCompareExchangePointer(dest, value, expected) == expected;
}

/* Numeric Operations, returns the previous value of the destination. */
inline int32_t AtomicAdd32(volatile int32_t* sum, int32_t value) {
  return InterlockedAdd(dest, value) - value;
}

inline int64_t AtomicAdd64(volatile int64_t* sum, int32_t value) {
  return InterlockedAdd64(dest, value) - value;
}

inline int32_t AtomicAnd32(volatile int32_t* result, int32_t value) {
  return InterlockedAnd(result, value);
}

inline int64_t AtomicAnd64(volatile int64_t* result, int64_t value) {
  return InterlockedAnd64(result, value);
}

inline int32_t AtomicOr32(volatile int32_t* result, int32_t value) {
  return InterlockedOr(result, value);
}

inline int64_t AtomicOr64(volatile int64_t* result, int64_t value) {
  return InterlockedOr64(result, value);
}

inline int32_t AtomicXor32(volatile int32_t* result, int32_t value) {
  return InterlockedXor(result, value);
}

inline int64_t AtomicXor64(volatile int64_t* result, int64_t value) {
  return InterlockedXor64(result, value);
}

}}
