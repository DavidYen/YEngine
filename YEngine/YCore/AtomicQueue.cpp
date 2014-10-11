#include <YEngine/stdincludes.h>
#include "AtomicQueue.h"

#include <string>

#include "Atomics.h"

// Left most byte reserved as a counter to guarantee unique compare and swaps.
#define CNT_MASK (0xFF << 55)
#define NUM_MASK (~CNT_MASK)

namespace YEngine { namespace YCore {

AtomicQueue::AtomicQueue(void* buffer, size_t item_size,
                                   size_t num_items)
    : mBuffer(reinterpret_cast<char*>(buffer)),
      mItemSize(item_size),
      mNumItems(num_items),
      mHead(0),
      mTail(0) {
}

AtomicQueue::~AtomicQueue() {
}

bool AtomicQueue::Enqueue(const void* data_item) {
  uint64_t cur_head = mHead;
  uint64_t cur_tail = mTail;
  MemoryBarrier();

  // Check if full
  if (cur_tail + 1 != cur_head)
    return false;

  memcpy(mBuffer + cur_tail + 1, data_item, mItemSize);
  return false;
}

bool AtomicQueue::Dequeue(void* data_item) {
  (void) data_item;
  return false;
}

}} // namespace YEngine { namespace YCore {
