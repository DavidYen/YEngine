#ifndef YCOMMON_YCONTAINERS_MEMPOOL_H
#define YCOMMON_YCONTAINERS_MEMPOOL_H

/*******
* Array able to store and remove fixed size elements.
*   - buffer size requirement: item_size * num_items
********/
namespace YCommon { namespace YContainers {

class MemPool {
 public:
  MemPool();
  MemPool(void* buffer, size_t buffer_size,
              size_t item_size, uint32_t num_items);
  ~MemPool();

  void Init(void* buffer, size_t buffer_size,
            size_t item_size, uint32_t num_items);
  void Reset();

  // Allocates or Removes an index
  uint32_t Allocate(); // returns -1 on failure.
  void Remove(uint32_t index);

  // Returns inserted index or -1.
  uint32_t Insert(const void* data_item);

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
  TypedMemPool() : MemPool() {}
  TypedMemPool(T* buffer, size_t buffer_size, uint32_t num_items)
      : MemPool(static_cast<void*>(buffer), buffer_size,
                    sizeof(T), num_items) {}
  ~TypedMemPool() {}

  void Init(T* buffer, size_t buffer_size, uint32_t num_items) {
    return MemPool::Init(static_cast<void*>(buffer), buffer_size,
                             sizeof(T), num_items);
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
  ContainedMemPool() : TypedMemPool(mData, sizeof(mData), items) {}
  ~ContainedMemPool() {}

  void Init() {
    TypedMemPool::Init(mData, sizeof(mData), items);
  }

 private:
  T mData[items];
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_MEMPOOL_H
