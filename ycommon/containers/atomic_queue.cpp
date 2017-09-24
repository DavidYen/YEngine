#include "ycommon/containers/atomic_queue.h"

#include <string>

#include "ycommon/headers/atomic_number_mask.h"
#include "ycommon/headers/atomics.h"
#include "ycommon/utils/assert.h"

namespace ycommon { namespace containers {

AtomicQueue::AtomicQueue()
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mHead(0),
      mTail(0) {
}

AtomicQueue::AtomicQueue(void* buffer, size_t buffer_size,
                         size_t item_size, size_t num_items)
    : mBuffer(NULL),
      mItemSize(0),
      mNumItems(0),
      mHead(0),
      mTail(0) {
  Initialize(buffer, buffer_size, item_size, num_items);
}

AtomicQueue::~AtomicQueue() {
}

void AtomicQueue::Initialize(void* buffer, size_t buffer_size,
                             size_t item_size, size_t num_items) {
  mBuffer = buffer;
  mItemSize = item_size;
  mNumItems = num_items;
  mHead = 0;
  mTail = 0;

  YASSERT(buffer_size >= (item_size * num_items),
          "Atomic Queue %u[%u] requires at least %u bytes, supplied %u bytes.",
          static_cast<uint32_t>(item_size), num_items,
          static_cast<uint32_t>(item_size * num_items),
          static_cast<uint32_t>(buffer_size));
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
          CONSTRUCT_NEXT_VALUE(tail_cnt, 0) :
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
                                  CONSTRUCT_NEXT_VALUE(head_cnt, 0) :
                                  CONSTRUCT_VALUE(head_cnt, new_head_num);

    if (AtomicCmpSet64(&mHead, cur_head, new_head_val))
      break;
  }

  return true;
}

size_t AtomicQueue::CurrentSize() const {
  const uint64_t cur_head = mHead;
  const uint64_t cur_tail = mTail;
  MemoryBarrier();

  const uint64_t size = static_cast<uint64_t>(mItemSize);
  const uint64_t cur_head_num = GET_NUM(cur_head);
  const uint64_t cur_tail_num = GET_NUM(cur_tail);

  if (cur_head_num == cur_tail_num) {
    // Empty if the counters match, full otherwise.
    return (cur_head == cur_tail) ? 0 : mItemSize;
  } else if (cur_tail_num > cur_head_num) {
    return static_cast<size_t>((cur_tail_num - cur_head_num) / size);
  } else {
    const uint64_t looped_tail = cur_tail_num + (size * mNumItems);
    return static_cast<size_t>((looped_tail - cur_head_num) / size);
  }
}

}} // namespace ycommon { namespace containers {
