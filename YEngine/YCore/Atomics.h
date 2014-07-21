#ifndef YCORE_ATOMICS_H
#define YCORE_ATOMICS_H

namespace ycore { namespace atomic {
  /* Memory Barrier. */
  inline void MemoryBarrier();

  /* Sets and returns previous value. */
  inline int32_t AtomicSet32(volatile int32_t* dest, int32_t value);
  inline int64_t AtomicSet64(volatile int64_t* dest, int64_t value);
  inline void* AtomicSetPtr(volatile void* dest, int64_t value);

  /* Compares dest with expected and sets to value if they match. */
  inline bool AtomicCmpSet(volatile int32_t* dest, int32_t expected,
                           int32_t value);
  inline bool AtomicCmpSet(volatile int64_t* dest, int64_t expected,
                           int64_t value);
  inline bool AtomicCmpSet(volatile void* dest, void* expected,
                           void* value);

  /* Numeric Operations, returns the previous value of the destination. */
  inline int32_t AtomicAdd32(volatile int32_t* sum, int32_t value);
  inline int64_t AtomicAdd64(volatile int64_t* sum, int32_t value);

  inline int32_t AtomicAnd32(volatile int32_t* result, int32_t value);
  inline int64_t AtomicAnd64(volatile int64_t* result, int64_t value);

  inline int32_t AtomicOr32(volatile int32_t* result, int32_t value);
  inline int64_t AtomicOr64(volatile int64_t* result, int64_t value);

  inline int32_t AtomicXor32(volatile int32_t* result, int32_t value);
  inline int64_t AtomicXor64(volatile int64_t* result, int64_t value);
}}

#ifdef WIN
  #include "Atomics_Win.h"
#endif

#endif // YCORE_ATOMICS_H