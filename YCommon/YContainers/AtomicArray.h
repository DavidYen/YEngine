#ifndef YCOMMON_YCONTAINERS_ATOMICARRAY_H
#define YCOMMON_YCONTAINERS_ATOMICARRAY_H

/*******
* Atomic Array able to store and remove fixed size elements.
*   - buffer size requirement: item_size * num_items
********/
namespace YCommon { namespace YContainers {

class AtomicArray {
 public:
  AtomicArray();
  AtomicArray(void* buffer, size_t buffer_size,
              size_t item_size, uint32_t num_items);
  ~AtomicArray();

  void Init(void* buffer, size_t buffer_size,
            size_t item_size, uint32_t num_items);

  // Allocates or Removes an index
  uint32_t Allocate(); // returns -1 on failure.
  void Remove(uint32_t index);

  // Returns inserted index or -1.
  uint32_t Insert(const void* data_item);
  uint32_t GetMaxUsedIndex();

 private:
  void* mBuffer;
  size_t mItemSize;
  uint32_t mNumItems;

  volatile uint64_t mUsedIndexes;
  volatile uint64_t mNextFreeIndex;;
};

template<typename T>
class TypedAtomicArray : public AtomicArray {
 public:
  TypedAtomicArray() : AtomicArray() {}
  TypedAtomicArray(T* buffer, size_t buffer_size, uint32_t num_items)
      : AtomicArray(static_cast<void*>(buffer), buffer_size,
                    sizeof(T), num_items) {}
  ~TypedAtomicArray() {}

  void Init(T* buffer, size_t buffer_size, uint32_t num_items) {
    return AtomicArray::Init(static_cast<void*>(buffer), buffer_size,
                             sizeof(T), num_items);
  }

  uint32_t Insert(const T* data_item) {
    return AtomicArray::Insert(static_cast<const void*>(data_item));
  }

  uint32_t Insert(const T& data_item) {
    return AtomicArray::Insert(static_cast<const void*>(&data_item));
  }
};

template<typename T, size_t items>
class ContainedAtomicArray : public TypedAtomicArray<T> {
 public:
  ContainedAtomicArray() : TypedAtomicArray(mBuffer, sizeof(mBuffer), items) {}
  ~ContainedAtomicArray() {}

  void Init() {
    TypedAtomicArray::Init(mBuffer, sizeof(mBuffer), items);
  }

  const T& operator[](size_t index) const { return mBuffer[index]; }
  T& operator[](size_t index) { return mBuffer[index]; }

 private:
  T mBuffer[items];
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_ATOMICARRAY_H
