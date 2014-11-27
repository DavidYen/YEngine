#include <YCommon/Headers/stdincludes.h>
#include "AtomicArray.h"

#include <string>

#include <YCommon/Headers/Atomics.h>
#include <YCommon/Headers/AtomicNumberMask.h>

namespace YCommon { namespace YContainers {

/*************
* Atomic Array is a regular array as items get added, when items are removed
* the element locations will contain an index to the next "free" element
* index. It basically becomes a linked list of free elements.
**************/
AtomicArray::AtomicArray()
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mUsedIndexes(0),
      mNextFreeIndex(static_cast<uint32_t>(-1)) {
}

AtomicArray::AtomicArray(void* buffer, size_t buffer_size,
                         size_t item_size, uint32_t num_items)
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mUsedIndexes(0),
      mNextFreeIndex(static_cast<uint32_t>(-1)) {
  Init(buffer, buffer_size, item_size, num_items);
}

AtomicArray::~AtomicArray() {
}

void AtomicArray::Init(void* buffer, size_t buffer_size,
                       size_t item_size, uint32_t num_items) {
  mBuffer = buffer;
  mItemSize = item_size;
  mNumItems = num_items;
  mUsedIndexes = 0;
  mNextFreeIndex = static_cast<uint32_t>(-1);

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

uint32_t AtomicArray::Allocate() {
  uint64_t used_size_value = mUsedIndexes;
  uint64_t next_free_index_value = mNextFreeIndex;
  MemoryBarrier();

  const size_t item_size = mItemSize;
  const size_t array_size = mNumItems;
  uint8_t* buffer_ptr = static_cast<uint8_t*>(mBuffer);

  // Check if there are any free items
  uint64_t free_index = static_cast<uint64_t>(-1);
  uint64_t next_free_index_num = GET_NUM(next_free_index_value);
  while (next_free_index_num != static_cast<uint32_t>(-1)) {
    uint8_t* next_free_loc = &buffer_ptr[item_size*next_free_index_num];
    uint64_t new_next_free_index = *reinterpret_cast<uint32_t*>(next_free_loc);

    if (AtomicCmpSet64(&mNextFreeIndex,
                       next_free_index_value,
                       CONSTRUCT_NEXT_VALUE(GET_CNT(next_free_index_value),
                                            new_next_free_index))) {
      free_index = next_free_index_num;
      break;
    }

    next_free_index_value = mNextFreeIndex;
    MemoryBarrier();

    next_free_index_num = GET_NUM(next_free_index_value);
  }

  // Check if used rest of array
  if (free_index == static_cast<uint64_t>(-1)) {
    uint64_t used_size_num = GET_NUM(used_size_value);
    while ((used_size_num+1) < array_size) {
      const uint32_t test_free_index = static_cast<uint32_t>(used_size_num);
      if (AtomicCmpSet64(&mUsedIndexes,
                         used_size_value,
                         CONSTRUCT_NEXT_VALUE(GET_CNT(used_size_value),
                                              used_size_num+1))) {
        free_index = test_free_index;
        break;
      }

      used_size_value = mUsedIndexes;
      MemoryBarrier();

      used_size_num = GET_NUM(used_size_value);
    }
  }

  return static_cast<uint32_t>(free_index);
}

uint32_t AtomicArray::Insert(const void* data_item) {
  const size_t item_size = mItemSize;
  uint32_t free_index = Allocate();

  // Copy data if index is reserved successfully.
  if (free_index == static_cast<uint32_t>(-1)) {
    memcpy(static_cast<uint8_t*>(mBuffer) + (item_size * free_index),
           data_item,
           item_size);
  }

  return free_index;
}

void AtomicArray::Remove(uint32_t index) {
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

uint32_t AtomicArray::GetMaxUsedIndex() {
  return static_cast<uint32_t>(GET_NUM(mUsedIndexes));
}

}} // namespace YCommon { namespace YContainers {
