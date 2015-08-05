#include <YCommon/Headers/stdincludes.h>
#include "AtomicMemPool.h"

#include <algorithm>
#include <string>

#include <YCommon/Headers/Atomics.h>
#include <YCommon/Headers/AtomicNumberMask.h>

namespace YCommon { namespace YContainers {

/*************
* Atomic Array is a regular array as items get added, when items are removed
* the element locations will contain an index to the next "free" element
* index. It basically becomes a linked list of free elements.
**************/
AtomicMemPool::AtomicMemPool()
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mUsedIndexes(0),
      mNextFreeIndex(CONSTRUCT_VALUE(static_cast<uint64_t>(0),
                                     static_cast<uint32_t>(-1))) {
}

AtomicMemPool::AtomicMemPool(void* buffer, size_t buffer_size,
                         size_t item_size, uint32_t num_items)
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mUsedIndexes(0),
      mNextFreeIndex(CONSTRUCT_VALUE(static_cast<uint64_t>(0),
                                     static_cast<uint32_t>(-1))) {
  Init(buffer, buffer_size, item_size, num_items);
}

AtomicMemPool::~AtomicMemPool() {
}

void AtomicMemPool::Init(void* buffer, size_t buffer_size,
                       size_t item_size, uint32_t num_items) {
  mBuffer = buffer;
  mItemSize = item_size;
  mNumItems = num_items;
  mUsedIndexes = 0;
  mNextFreeIndex = CONSTRUCT_VALUE(static_cast<uint64_t>(0),
                                   static_cast<uint32_t>(-1));

  (void) buffer_size;
  YASSERT(reinterpret_cast<uintptr_t>(buffer) % sizeof(uint32_t) == 0,
          "Atomic Array memory must be aligned to %u bytes - supplied %p.",
          static_cast<uint32_t>(sizeof(uint32_t)),
          buffer);
  YASSERT(item_size % sizeof(uint32_t) == 0,
          "Atomic Array element size must be a multiple of %u - supplied %u.",
          static_cast<uint32_t>(sizeof(uint32_t)),
          static_cast<uint32_t>(item_size));
  YASSERT(buffer_size >= (item_size * num_items),
          "Atomic Array %u[%u] requires at least %u bytes, supplied %u bytes.",
          static_cast<uint32_t>(item_size), num_items,
          static_cast<uint32_t>(item_size * num_items),
          static_cast<uint32_t>(buffer_size));
}

void AtomicMemPool::Reset() {
  mBuffer = NULL;
  mItemSize = 0;
  mNumItems = 0;
  mUsedIndexes = 0;
  mNextFreeIndex = CONSTRUCT_VALUE(static_cast<uint64_t>(0),
                                   static_cast<uint32_t>(-1));
}

uint32_t AtomicMemPool::Allocate() {
  uint32_t used_indexes = mUsedIndexes;
  uint64_t next_free_index_value = mNextFreeIndex;
  MemoryBarrier();

  const size_t item_size = mItemSize;
  uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);

  // Check if there are any free items
  uint32_t next_num = static_cast<uint32_t>(GET_NUM(next_free_index_value));
  while (next_num != static_cast<uint32_t>(-1)) {
    uint8_t* next_free_loc = &buffer_ptr[item_size*next_num];
    uint32_t new_next_free_index = *reinterpret_cast<uint32_t*>(next_free_loc);

    if (AtomicCmpSet64(&mNextFreeIndex,
                       next_free_index_value,
                       CONSTRUCT_NEXT_VALUE(GET_CNT(next_free_index_value),
                                            new_next_free_index))) {
      return next_num;
    }

    next_free_index_value = mNextFreeIndex;
    MemoryBarrier();

    next_num = static_cast<uint32_t>(GET_NUM(next_free_index_value));
  }

  // Check if used rest of array
  const size_t array_size = mNumItems;
  if (used_indexes < array_size) {
    uint32_t new_index = AtomicAdd32(&mUsedIndexes, 1);
    if (new_index < array_size) {
      return new_index;
    }
  }

  return static_cast<uint32_t>(-1);
}

void AtomicMemPool::Remove(uint32_t index) {
  uint64_t next_free_index_value = mNextFreeIndex;
  MemoryBarrier();

  const size_t item_size = mItemSize;
  uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);

  for (;;) {
    const uint64_t next_free_index_num = GET_NUM(next_free_index_value);
    const uint32_t next_free_index = static_cast<uint32_t>(next_free_index_num);

    uint8_t* remove_loc = &buffer_ptr[item_size*index];
    *reinterpret_cast<uint32_t*>(remove_loc) = next_free_index;

    if (AtomicCmpSet64(&mNextFreeIndex,
                       next_free_index_value,
                       CONSTRUCT_NEXT_VALUE(GET_CNT(next_free_index_value),
                                            static_cast<uint64_t>(index)))) {
        break;
    }

    next_free_index_value = mNextFreeIndex;
    MemoryBarrier();
  }
}

uint32_t AtomicMemPool::Insert(const void* data_item) {
  const size_t item_size = mItemSize;
  uint32_t free_index = Allocate();

  // Copy data if index is reserved successfully.
  if (free_index != static_cast<uint32_t>(-1)) {
    memcpy(static_cast<uint8_t*>(mBuffer) + (item_size * free_index),
           data_item,
           item_size);
  }

  return free_index;
}

uint32_t AtomicMemPool::GetIndex(const void* buffer_item) {
  const uint8_t* item_ptr = static_cast<const uint8_t*>(buffer_item);
  const uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);
  const size_t index = (item_ptr - buffer_ptr) / mItemSize;
  YASSERT((item_ptr >= buffer_ptr) &&
          ((item_ptr - buffer_ptr) % mItemSize == 0) &&
          (index < mUsedIndexes),
          "Cannot obtain index from invalid memory pool memory: %p", item_ptr);
  return static_cast<uint32_t>(index);
}

uint32_t AtomicMemPool::GetNumIndexesUsed() {
  return min(mUsedIndexes, mNumItems);
}

}} // namespace YCommon { namespace YContainers {
