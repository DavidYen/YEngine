#ifndef YCOMMON_CONTAINERS_ATOMIC_MEM_POOL_H
#define YCOMMON_CONTAINERS_ATOMIC_MEM_POOL_H

#include <stdint.h>

/*******
* Atomic Array able to store and remove fixed size elements.
*   - buffer size requirement: item_size * num_items
********/
namespace ycommon { namespace containers {

class AtomicMemPool {
 public:
  AtomicMemPool();
  AtomicMemPool(void* buffer, size_t buffer_size,
              size_t item_size, uint32_t num_items);
  ~AtomicMemPool();

  void Init(void* buffer, size_t buffer_size,
            size_t item_size, uint32_t num_items);
  void Reset();

  // Allocates or Removes an index
  uint32_t Allocate(); // returns -1 on failure.
  void Remove(uint32_t index);

  // Returns inserted index or -1.
  uint32_t Insert(const void* data_item);
  uint32_t GetIndex(const void* buffer_item);

  // Gets max number of indexes used.
  uint32_t GetNumIndexesUsed();

  size_t GetItemSize() { return mItemSize; }
  uint32_t GetNumItems() { return mNumItems; }

 protected:
  void* mBuffer;
  size_t mItemSize;
  uint32_t mNumItems;

  volatile uint32_t mUsedIndexes;
  volatile uint64_t mNextFreeIndex;
};

template<typename T>
class TypedAtomicMemPool : public AtomicMemPool {
 public:
  TypedAtomicMemPool() : AtomicMemPool() {}
  TypedAtomicMemPool(T* buffer, size_t buffer_size, uint32_t num_items)
      : AtomicMemPool(static_cast<void*>(buffer), buffer_size,
                    sizeof(T), num_items) {}
  ~TypedAtomicMemPool() {}

  void Init(T* buffer, size_t buffer_size, uint32_t num_items) {
    return AtomicMemPool::Init(static_cast<void*>(buffer), buffer_size,
                             sizeof(T), num_items);
  }

  uint32_t Insert(const T* data_item) {
    return AtomicMemPool::Insert(static_cast<const void*>(data_item));
  }

  uint32_t Insert(const T& data_item) {
    return AtomicMemPool::Insert(static_cast<const void*>(&data_item));
  }

  const T& operator[](size_t index) const {
    return static_cast<T*>(mBuffer)[index];
  }

  T& operator[](size_t index) {
    return static_cast<T*>(mBuffer)[index];
  }
};

template<typename T, size_t items>
class ContainedAtomicMemPool : public TypedAtomicMemPool<T> {
 public:
  ContainedAtomicMemPool() : TypedAtomicMemPool(mData, sizeof(mData), items) {}
  ~ContainedAtomicMemPool() {}

  void Init() {
    TypedAtomicMemPool::Init(mData, sizeof(mData), items);
  }

 private:
  T mData[items];
};

}} // namespace ycommon { namespace containers {

#endif // YCOMMON_CONTAINERS_ATOMIC_MEM_POOL_H
