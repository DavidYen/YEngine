#ifndef YCOMMON_CONTAINERS_UNORDERED_ARRAY_H
#define YCOMMON_CONTAINERS_UNORDERED_ARRAY_H

#include <stdint.h>

/*******
* Array where index is not kept constant useful for keeping track of
*   items in an contiguous array with fast insertion/deletions.
********/
namespace ycommon { namespace containers {

typedef void (*PreItemSwap)(uint32_t old_index, uint32_t new_index, void* arg);

class UnorderedArray {
 public:
  static size_t GetAllocationSize(size_t item_size, uint32_t num_items) {
    return item_size * num_items;
  }

  UnorderedArray();
  UnorderedArray(void* buffer, size_t buffer_size,
                 size_t item_size, uint32_t num_items);
  ~UnorderedArray();

  void Init(void* buffer, size_t buffer_size,
            size_t item_size, uint32_t num_items);
  void Reset();
  void Clear();

  void SetItemSwappedCallBack(PreItemSwap callback_routine,
                              void* arg = nullptr);

  // Allocates or Removes an index
  uint32_t Allocate(); // returns -1 on failure.
  void Remove(uint32_t index);

  void* GetData(uint32_t index) {
    return static_cast<uint8_t*>(mBuffer) + (mItemSize * index);
  }

  size_t GetItemSize() { return mItemSize; }
  uint32_t GetCount() { return mItemCount; }
  uint32_t GetTotalSize() { return mTotalSize; }

 protected:
  void* mBuffer;
  size_t mItemSize;
  uint32_t mItemCount;
  uint32_t mTotalSize;

  PreItemSwap mPreSwapItemCallback;
  void* mCallbackArg;
};

template<typename T>
class TypedUnorderedArray : public UnorderedArray {
 public:
  static size_t GetAllocationSize(uint32_t num_items) {
    return sizeof(T) * num_items;
  }

  TypedUnorderedArray() : UnorderedArray() {}
  TypedUnorderedArray(void* buffer, size_t buffer_size, uint32_t num_items)
      : UnorderedArray(buffer, buffer_size, sizeof(T), num_items) {}
  ~TypedUnorderedArray() {}

  void Init(void* buffer, size_t buffer_size, uint32_t num_items) {
    return UnorderedArray::Init(buffer, buffer_size, sizeof(T), num_items);
  }

  uint32_t PushBack(const T* data) {
    const uint32_t index = Allocate();
    memcpy(GetData(index), data, sizeof(data));
    return index;
  }

  uint32_t PushBack(const T& data) {
    return PushBack(&data);
  }

  const T& operator[](size_t index) const {
    return static_cast<T*>(mBuffer)[index];
  }

  T& operator[](size_t index) {
    return static_cast<T*>(mBuffer)[index];
  }

  size_t GetItemSize() { return sizeof(T); }
};

template<typename T, size_t items>
class ContainedUnorderedArray : public TypedUnorderedArray<T> {
 public:
  static size_t GetAllocationSize() {
    return sizeof(T) * items;
  }

  ContainedUnorderedArray() : TypedUnorderedArray(mData, sizeof(mData), items) {
  }
  ~ContainedUnorderedArray() {}

  void Init() {
    TypedUnorderedArray::Init(mData, sizeof(mData), items);
  }

 private:
  T mData[items];
};

}} // namespace ycommon { namespace containers {

#endif // YCOMMON_CONTAINERS_UNORDERED_ARRAY_H
