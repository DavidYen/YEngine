#ifndef YCOMMON_CONTAINERS_MEM_POOL_H
#define YCOMMON_CONTAINERS_MEM_POOL_H

#include <stdint.h>

/*******
* Array able to store and remove fixed size elements.
*   - buffer size requirement: item_size * num_items
********/
namespace ycommon { namespace containers {

class MemPool {
 public:
  static size_t GetAllocationSize(size_t item_size, uint32_t num_items) {
    return item_size * num_items;
  }

  MemPool();
  MemPool(void* buffer, size_t buffer_size,
          size_t item_size, uint32_t num_items);
  ~MemPool();

  void Init(void* buffer, size_t buffer_size,
            size_t item_size, uint32_t num_items);
  void Reset();

  void Clear();

  // Allocates or Removes an index
  uint32_t Allocate(); // returns -1 on failure.
  void Remove(uint32_t index);

  // Returns inserted index or -1.
  uint32_t Insert(const void* data_item);
  uint32_t GetIndex(const void* item);

  // Gets max number of indexes used.
  uint32_t GetNumIndexesUsed();

  size_t GetItemSize() { return mItemSize; }
  uint32_t GetNumItems() { return mNumItems; }

 protected:
  void* mBuffer;
  size_t mItemSize;
  uint32_t mNumItems;

  uint32_t mUsedIndexes;
  uint32_t mNextFreeIndex;
};

template<typename T>
class TypedMemPool : public MemPool {
 public:
  static size_t GetAllocationSize(uint32_t num_items) {
    return sizeof(T) * num_items;
  }

  TypedMemPool() : MemPool() {}
  TypedMemPool(void* buffer, size_t buffer_size, uint32_t num_items)
      : MemPool(buffer, buffer_size, sizeof(T), num_items) {}
  ~TypedMemPool() {}

  void Init(void* buffer, size_t buffer_size, uint32_t num_items) {
    return MemPool::Init(buffer, buffer_size, sizeof(T), num_items);
  }

  uint32_t Insert(const T* data_item) {
    return MemPool::Insert(static_cast<const void*>(data_item));
  }

  uint32_t Insert(const T& data_item) {
    return MemPool::Insert(static_cast<const void*>(&data_item));
  }

  const T& operator[](size_t index) const {
    return static_cast<T*>(mBuffer)[index];
  }

  T& operator[](size_t index) {
    return static_cast<T*>(mBuffer)[index];
  }
};

template<typename T, size_t items>
class ContainedMemPool : public TypedMemPool<T> {
 public:
  static size_t GetAllocationSize() {
    return sizeof(T) * items;
  }

  ContainedMemPool() : TypedMemPool(mData, sizeof(mData), items) {}
  ~ContainedMemPool() {}

  void Init() {
    TypedMemPool::Init(mData, sizeof(mData), items);
  }

 private:
  T mData[items];
};

}} // namespace ycommon { namespace containers {

#endif // YCOMMON_CONTAINERS_MEM_POOL_H
