#include <YCommon/Headers/stdincludes.h>
#include "UnorderedArray.h"

#include <string>

namespace YCommon { namespace YContainers {

UnorderedArray::UnorderedArray()
  : mBuffer(nullptr),
    mItemSize(0),
    mItemCount(0),
    mTotalSize(0),
    mPreSwapItemCallback(nullptr),
    mCallbackArg(nullptr) {
}

UnorderedArray::UnorderedArray(void* buffer, size_t buffer_size,
                               size_t item_size, uint32_t num_items)
  : mBuffer(nullptr),
    mItemSize(0),
    mItemCount(0),
    mTotalSize(0),
    mPreSwapItemCallback(nullptr),
    mCallbackArg(nullptr) {
  Init(buffer, buffer_size, item_size, num_items);
}

UnorderedArray::~UnorderedArray() {
}

void UnorderedArray::Init(void* buffer, size_t buffer_size,
                          size_t item_size, uint32_t num_items) {
  YASSERT(item_size * num_items <= buffer_size,
          "Unordered Array requires %u bytes, supplied %u bytes.",
          buffer_size, item_size * num_items);

  mBuffer = buffer;
  mItemSize = item_size;
  mItemCount = 0;
  mTotalSize = num_items;
  mPreSwapItemCallback = nullptr;
  mCallbackArg = nullptr;
}

void UnorderedArray::Reset() {
  mBuffer = nullptr;
  mItemSize = 0;
  mItemCount = 0;
  mTotalSize = 0;
  mPreSwapItemCallback = nullptr;
  mCallbackArg = nullptr;
}

void UnorderedArray::Clear() {
  mItemCount = 0;
}

void UnorderedArray::SetItemSwappedCallBack(PreItemSwap callback_routine,
                                            void* arg) {
  mPreSwapItemCallback = callback_routine;
  mCallbackArg = arg;
}

// Allocates or Removes an index
uint32_t UnorderedArray::Allocate() {
  if (mItemCount < mTotalSize) {
    return mItemCount++;
  }
  return static_cast<uint32_t>(-1);
}

void UnorderedArray::Remove(uint32_t index) {
  YASSERT(index < mItemCount,
          "Invalid index to remove from array[%u]: %u", mItemCount, index);
  if (index != mItemCount - 1) {
    if (mPreSwapItemCallback) {
      mPreSwapItemCallback(mItemCount - 1, index, mCallbackArg);
    }
    memcpy(GetData(index), GetData(mItemCount-1), mItemSize);
  }
  mItemCount--;
}

}} // namespace YCommon { namespace YContainers {
