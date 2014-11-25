#ifndef YCOMMON_HEADERS_ATOMICS_H
#define YCOMMON_HEADERS_ATOMICS_H

namespace YCommon {

/* Memory Barrier. */
inline void MemoryBarrier();
inline void AcquireFence();
inline void ReleaseFence();

/* Sets and returns previous value. */
inline int32_t AtomicSet32(volatile int32_t* dest, int32_t value);
inline int64_t AtomicSet64(volatile int64_t* dest, int64_t value);
inline void* AtomicSetPtr(volatile void* dest, intptr_t value);

/* Compares dest with expected and sets to value if they match. */
inline bool AtomicCmpSet32(volatile int32_t* dest, int32_t expected,
                           int32_t value);
inline bool AtomicCmpSet64(volatile int64_t* dest, int64_t expected,
                           int64_t value);
inline bool AtomicCmpSetPtr(volatile void* dest, void* expected,
                            void* value);

/* Numeric Operations, returns the previous value of the destination. */
inline int32_t AtomicAdd32(volatile int32_t* sum, int32_t value);
inline int64_t AtomicAdd64(volatile int64_t* sum, int64_t value);

inline int32_t AtomicAnd32(volatile int32_t* result, int32_t value);
inline int64_t AtomicAnd64(volatile int64_t* result, int64_t value);

inline int32_t AtomicOr32(volatile int32_t* result, int32_t value);
inline int64_t AtomicOr64(volatile int64_t* result, int64_t value);

inline int32_t AtomicXor32(volatile int32_t* result, int32_t value);
inline int64_t AtomicXor64(volatile int64_t* result, int64_t value);

/* Convenience Unsigned Variants. */
inline uint32_t AtomicSet32(volatile uint32_t* dest, uint32_t value) {
  return static_cast<uint32_t>(AtomicSet32(
    reinterpret_cast<volatile int32_t*>(dest),
    static_cast<int32_t>(value)));
}
inline uint64_t AtomicSet64(volatile uint64_t* dest, uint64_t value) {
  return static_cast<uint32_t>(AtomicSet64(
    reinterpret_cast<volatile int64_t*>(dest),
    static_cast<int64_t>(value)));
}
inline void* AtomicSetPtr(volatile void* dest, uintptr_t value) {
  return AtomicSetPtr(dest, static_cast<intptr_t>(value));
}

inline bool AtomicCmpSet32(volatile uint32_t* dest, uint32_t expected,
                           uint32_t value) {
  return AtomicCmpSet32(reinterpret_cast<volatile int32_t*>(dest),
                        static_cast<int32_t>(expected),
                        static_cast<int32_t>(value));
}
inline bool AtomicCmpSet64(volatile uint64_t* dest, uint64_t expected,
                           uint64_t value) {
  return AtomicCmpSet64(reinterpret_cast<volatile int64_t*>(dest),
                        static_cast<int64_t>(expected),
                        static_cast<int64_t>(value));
}

inline uint32_t AtomicAdd32(volatile uint32_t* sum, uint32_t value) {
  return static_cast<uint32_t>(AtomicAdd32(
    reinterpret_cast<volatile int32_t*>(sum),
    static_cast<uint32_t>(value)));
}
inline uint64_t AtomicAdd64(volatile uint64_t* sum, uint64_t value) {
  return static_cast<uint64_t>(AtomicAdd64(
    reinterpret_cast<volatile int64_t*>(sum),
    static_cast<int64_t>(value)));
}

inline uint32_t AtomicAnd32(volatile uint32_t* result, uint32_t value) {
  return static_cast<uint32_t>(AtomicAnd32(
    reinterpret_cast<volatile int32_t*>(result),
    static_cast<int32_t>(value)));
}
inline uint64_t AtomicAnd64(volatile uint64_t* result, uint64_t value) {
  return static_cast<uint64_t>(AtomicAnd64(
    reinterpret_cast<volatile int64_t*>(result),
    static_cast<int64_t>(value)));
}

inline uint32_t AtomicOr32(volatile uint32_t* result, uint32_t value) {
  return static_cast<uint32_t>(AtomicOr32(
    reinterpret_cast<volatile int32_t*>(result),
    static_cast<int32_t>(value)));
}
inline uint64_t AtomicOr64(volatile uint64_t* result, uint64_t value) {
  return static_cast<uint64_t>(AtomicOr64(
    reinterpret_cast<volatile int64_t*>(result),
    static_cast<int64_t>(value)));
}

inline uint32_t AtomicXor32(volatile uint32_t* result, uint32_t value) {
  return static_cast<uint32_t>(AtomicXor32(
    reinterpret_cast<volatile int32_t*>(result),
    static_cast<int32_t>(value)));
}
inline uint64_t AtomicXor64(volatile uint64_t* result, uint64_t value) {
  return static_cast<uint64_t>(AtomicXor64(
    reinterpret_cast<volatile int64_t*>(result),
    static_cast<int64_t>(value)));
}

} // namespace YCommon {

#ifdef WIN
  #include "Atomics_Win.h"
#endif

#endif // YCOMMON_HEADERS_ATOMICS_H
