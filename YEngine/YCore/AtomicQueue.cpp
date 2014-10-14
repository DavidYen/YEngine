#include <YEngine/stdincludes.h>
#include "AtomicQueue.h"

#include <string>

#include "Atomics.h"

// Left most byte reserved as a counter to guarantee unique compare and swaps.
#define NUM_MASK_BYTES 8
#define MASK_SHIFT (64-NUM_MASK_BYTES)

// In order to prevent ABA problem, each time we loop through the array
// we will increment the counter. This will also serve to let us know when
// the queue is full, so if head == tail and the loop counter matches, it is 
// empty. If head == tail and loop counter do not match, it is full.
#define CNT_MASK (static_cast<uint64_t>(0xFF) << MASK_SHIFT)
#define NUM_MASK (~CNT_MASK)

#define GET_CNT(VALUE) (VALUE & CNT_MASK)
#define GET_NUM(VALUE) (VALUE & NUM_MASK)

#define CONSTRUCT_VALUE(COUNT, NUMBER) (((COUNT) << MASK_SHIFT) | (NUMBER))

namespace YEngine { namespace YCore {

AtomicQueue::AtomicQueue(void* buffer, size_t item_size,
                         size_t num_items)
    : mBuffer(buffer),
      mItemSize(item_size),
      mNumItems(num_items),
      mHead(0),
      mTail(0) {
}

AtomicQueue::~AtomicQueue() {
}

bool AtomicQueue::Enqueue(const void* data_item) {
  const uint64_t cur_head = mHead;
  const uint64_t cur_tail = mTail;
  MemoryBarrier();

  const uint64_t cur_head_num = GET_NUM(cur_head);
  const uint64_t cur_tail_num = GET_NUM(cur_tail);

  // Check if full (numbers match, but counters do not)
  if (cur_head_num == cur_tail_num && cur_head != cur_tail)
    return false;

  memcpy(static_cast<char*>(mBuffer) + cur_tail_num, data_item, mItemSize);

  const uint64_t new_tail_num = cur_tail_num + mItemSize;
  const uint64_t tail_cnt = GET_CNT(cur_tail);

  // Check if looped through buffer.
  mTail = (new_tail_num == (mItemSize * mNumItems)) ?
          CONSTRUCT_VALUE(tail_cnt+1, 0) :
          CONSTRUCT_VALUE(tail_cnt, new_tail_num);

  return true;
}

bool AtomicQueue::Dequeue(void* data_item) {
  const size_t buffer_size = mItemSize * mNumItems;
  for (;;) {
    const uint64_t cur_head = mHead;
    const uint64_t cur_tail = mTail;
    MemoryBarrier();

    // Check if empty (both counter and numbers match)
    if (cur_head == cur_tail)
      return false;

    const uint64_t cur_head_num = GET_NUM(cur_head);

    memcpy(data_item, static_cast<char*>(mBuffer) + cur_head_num, mItemSize);

    const uint64_t new_head_num = cur_head_num + mItemSize;
    const uint64_t head_cnt = GET_CNT(cur_head);

    // Check if looped through buffer.
    const uint64_t new_head_val = (new_head_num == buffer_size) ?
                                  CONSTRUCT_VALUE(head_cnt+1, 0) :
                                  CONSTRUCT_VALUE(head_cnt, new_head_num);

    if (AtomicCmpSet64(&mHead, cur_head, new_head_val))
      break;
  }

  return true;
}

}} // namespace YEngine { namespace YCore {
