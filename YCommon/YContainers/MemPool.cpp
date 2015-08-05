#include <YCommon/Headers/stdincludes.h>
#include "MemPool.h"

#include <algorithm>
#include <string>

namespace YCommon { namespace YContainers {

/*************
* Array is a regular array as items get added, when items are removed
* the element locations will contain an index to the next "free" element
* index. It basically becomes a linked list of free elements.
**************/
MemPool::MemPool()
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mUsedIndexes(0),
      mNextFreeIndex(static_cast<uint32_t>(-1)) {
}

MemPool::MemPool(void* buffer, size_t buffer_size,
                 size_t item_size, uint32_t num_items)
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mUsedIndexes(0),
      mNextFreeIndex(static_cast<uint32_t>(-1)) {
  Init(buffer, buffer_size, item_size, num_items);
}

MemPool::~MemPool() {
}

void MemPool::Init(void* buffer, size_t buffer_size,
                   size_t item_size, uint32_t num_items) {
  mBuffer = buffer;
  mItemSize = item_size;
  mNumItems = num_items;
  mUsedIndexes = 0;
  mNextFreeIndex = static_cast<uint32_t>(-1);

  (void) buffer_size;
  YASSERT(reinterpret_cast<uintptr_t>(buffer) % sizeof(uint32_t) == 0,
          "Array memory must be aligned to %u bytes - supplied %p.",
          static_cast<uint32_t>(sizeof(uint32_t)),
          buffer);
  YASSERT(item_size % sizeof(uint32_t) == 0,
          "Array element size must be a multiple of %u - supplied %u.",
          static_cast<uint32_t>(sizeof(uint32_t)),
          static_cast<uint32_t>(item_size));
  YASSERT(buffer_size >= (item_size * num_items),
          "Array %u[%u] requires at least %u bytes, supplied %u bytes.",
          static_cast<uint32_t>(item_size), num_items,
          static_cast<uint32_t>(item_size * num_items),
          static_cast<uint32_t>(buffer_size));
}

void MemPool::Reset() {
  mBuffer = NULL;
  mItemSize = 0;
  mNumItems = 0;
  mUsedIndexes = 0;
  mNextFreeIndex = static_cast<uint32_t>(-1);
}

void MemPool::Clear() {
  mUsedIndexes = 0;
  mNextFreeIndex = static_cast<uint32_t>(-1);
}

uint32_t MemPool::Allocate() {
  const size_t item_size = mItemSize;
  const uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);

  uint32_t item_index = mNextFreeIndex;
  if (item_index != static_cast<uint32_t>(-1)) {
    const uint32_t* free_item =
        reinterpret_cast<const uint32_t*>(buffer_ptr + (item_size*item_index));
    mNextFreeIndex = *free_item;
  } else if (mUsedIndexes < mNumItems) {
    item_index = mUsedIndexes++;
  }

  return item_index;
}

uint32_t MemPool::Insert(const void* data_item) {
  const uint32_t free_index = Allocate();

  // Copy data if index is reserved successfully.
  if (free_index != static_cast<uint32_t>(-1)) {
    const size_t item_size = mItemSize;
    memcpy(static_cast<uint8_t*>(mBuffer) + (item_size * free_index),
           data_item,
           item_size);
  }

  return free_index;
}

void MemPool::Remove(uint32_t index) {
  YASSERT(index < mUsedIndexes, "Removing Invalid Index: %u", index);
  const size_t item_size = mItemSize;
  uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);

  uint32_t* free_item =
      reinterpret_cast<uint32_t*>(buffer_ptr + (item_size * index));

  *free_item = mNextFreeIndex;
  mNextFreeIndex = index;
}

uint32_t MemPool::GetIndex(const void* item) {
  const uint8_t* item_ptr = static_cast<const uint8_t*>(item);
  const uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);
  const size_t index = (item_ptr - buffer_ptr) / mItemSize;
  YASSERT((item_ptr >= buffer_ptr) &&
          ((item_ptr - buffer_ptr) % mItemSize == 0) &&
          (index < mUsedIndexes),
          "Cannot obtain index from invalid memory pool memory: %p", item);
  return static_cast<uint32_t>(index);
}

uint32_t MemPool::GetNumIndexesUsed() {
  return mUsedIndexes;
}

}} // namespace YCommon { namespace YContainers {
