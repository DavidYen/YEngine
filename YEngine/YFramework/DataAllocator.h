#ifndef YENGINE_YFRAMEWORK_ALLOCATIONDATA_H
#define YENGINE_YFRAMEWORK_ALLOCATIONDATA_H

namespace YEngine { namespace YFramework {

typedef uint8_t AllocDataID;

enum AllocType {
  kAllocType_UInt8,
  kAllocType_UInt16,
  kAllocType_UInt32,
  kAllocType_UInt64,
  kAllocType_Float,
  kAllocType_Float4,
  kAllocType_Float16,

  NUM_ALLOC_TYPES
};

struct AllocationData {
  uint64_t mNameHash;
  AllocType mAllocType;
  AllocDataID mAllocDataID;
};

class DataAllocator {
 public:
  DataAllocator();
  ~DataAllocator();

  // Reserve Alloc Type Additively.
  void ReserveAllocType(AllocType type, size_t count, size_t num_buckets);

  // Initialize should be called after all reservations are finished.
  void Initialize(void* buffer, size_t buffer_size);

  // Allocate Data Type.
  AllocDataID AllocateData(AllocType type, size_t num_elements);

 private:
  void* mDataTypeBuffers[NUM_ALLOC_TYPES];
  size_t mDataTypeCounts[NUM_ALLOC_TYPES];
};

}}

#endif // YENGINE_YFRAMEWORK_ALLOCATIONDATA_H
