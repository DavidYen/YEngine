#include <YCommon/Headers/stdincludes.h>
#include "MemBuffer.h"

namespace YCommon { namespace YContainers {

MemBuffer::MemBuffer(void* buffer, size_t buffer_size)
  : mBuffer(buffer),
    mUsedBufferSize(0),
    mUsedScratchPadSize(0),
    mBufferSize(buffer_size) {
}

void* MemBuffer::Allocate(size_t allocation_size) {
  if (allocation_size + mUsedBufferSize + mUsedScratchPadSize > mBufferSize) {
    return nullptr;
  }

  void* allocation = static_cast<uint8_t*>(mBuffer) + mUsedBufferSize;
  mUsedBufferSize += allocation_size;
  return allocation;
}

void MemBuffer::Free(size_t allocation_size) {
  YASSERT(allocation_size <= mUsedBufferSize, "Sanity check failed for Free.");
  mUsedBufferSize -= allocation_size;
}

MemBuffer::ScratchPad::ScratchPad(MemBuffer* buffer, size_t scratch_pad_size)
  : mBuffer(buffer),
    mScratchPadMemory(nullptr),
    mScratchPadSize(0) {
  const size_t new_size =
      scratch_pad_size + buffer->mUsedBufferSize + buffer->mUsedScratchPadSize;
  if (new_size <= buffer->mBufferSize) {
    buffer->mUsedScratchPadSize += scratch_pad_size;

    mScratchPadSize = scratch_pad_size;
    mScratchPadMemory =
        static_cast<uint8_t*>(buffer->mBuffer) + buffer->mUsedScratchPadSize;
  }
}

MemBuffer::ScratchPad::~ScratchPad() {
  mBuffer->mUsedScratchPadSize -= mScratchPadSize;
}

}}
