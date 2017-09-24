
#include "Windows.h"

namespace ycommon {

/* Memory Barrier. */
inline void MemoryBarrier() {
  ::MemoryBarrier();
}

inline void AcquireFence() {
  ::MemoryBarrier();
}

inline void ReleaseFence() {
  ::MemoryBarrier();
}

/* Sets and returns previous value. */
inline int32_t AtomicSet32(volatile int32_t* dest, int32_t value) {
  return InterlockedExchange(reinterpret_cast<volatile long*>(dest), value);
}

inline int64_t AtomicSet64(volatile int64_t* dest, int64_t value) {
  return InterlockedExchange64(dest, value);
}

inline void* AtomicSetPtr(void* volatile* dest, void* value) {
  return InterlockedExchangePointer(dest, value);
}

/* Compares dest with expected and sets to value if they match. */
inline bool AtomicCmpSet32(volatile int32_t* dest, int32_t expected,
                           int32_t value) {
  return InterlockedCompareExchange(reinterpret_cast<volatile long*>(dest),
                                    value, expected) == expected;
}
inline bool AtomicCmpSet64(volatile int64_t* dest, int64_t expected,
                           int64_t value) {
  return InterlockedCompareExchange64(dest, value, expected) == expected;
}
inline bool AtomicCmpSetPtr(void* volatile* dest, void* expected,
                            void* value) {
  return InterlockedCompareExchangePointer(dest, value, expected) == expected;
}

/* Numeric Operations, returns the previous value of the destination. */
inline int32_t AtomicAdd32(volatile int32_t* sum, int32_t value) {
  return InterlockedAdd(reinterpret_cast<volatile long*>(sum), value) - value;
}

inline int64_t AtomicAdd64(volatile int64_t* sum, int64_t value) {
  return InterlockedAdd64(sum, value) - value;
}

inline int32_t AtomicAnd32(volatile int32_t* result, int32_t value) {
  return InterlockedAnd(reinterpret_cast<volatile long*>(result), value);
}

inline int64_t AtomicAnd64(volatile int64_t* result, int64_t value) {
  return InterlockedAnd64(result, value);
}

inline int32_t AtomicOr32(volatile int32_t* result, int32_t value) {
  return InterlockedOr(reinterpret_cast<volatile long*>(result), value);
}

inline int64_t AtomicOr64(volatile int64_t* result, int64_t value) {
  return InterlockedOr64(result, value);
}

inline int32_t AtomicXor32(volatile int32_t* result, int32_t value) {
  return InterlockedXor(reinterpret_cast<volatile long*>(result), value);
}

inline int64_t AtomicXor64(volatile int64_t* result, int64_t value) {
  return InterlockedXor64(result, value);
}

} //namespace ycommon {

