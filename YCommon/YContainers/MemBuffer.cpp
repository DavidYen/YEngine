#include <YCommon/Headers/stdincludes.h>
#include "MemBuffer.h"

namespace YCommon { namespace YContainers {

MemBuffer::MemBuffer()
  : mBuffer(nullptr),
    mUsedBufferSize(0),
    mUsedScratchPadSize(0),
    mBufferSize(0) {
}

MemBuffer::MemBuffer(void* buffer, size_t buffer_size)
  : mBuffer(nullptr),
    mUsedBufferSize(0),
    mUsedScratchPadSize(0),
    mBufferSize(0) {
  Init(buffer, buffer_size);
}

void MemBuffer::Init(void* buffer, size_t buffer_size) {
  YASSERT(buffer, "Invalid Buffer passed to MemBuffer.");
  mBuffer = buffer;
  mUsedBufferSize = 0;
  mUsedScratchPadSize = 0;
  mBufferSize = buffer_size;
}

void MemBuffer::Reset() {
  mBuffer = nullptr;
  mUsedBufferSize = 0;
  mUsedScratchPadSize = 0;
  mBufferSize = 0;
}

void MemBuffer::Clear() {
  mUsedBufferSize = 0;
  mUsedScratchPadSize = 0;
}

void* MemBuffer::Allocate(size_t allocation_size) {
  if (allocation_size > FreeSpace())
    return nullptr;

  void* allocation = static_cast<uint8_t*>(mBuffer) + mUsedBufferSize;
  mUsedBufferSize += allocation_size;
  return allocation;
}

void* MemBuffer::Allocate(size_t allocation_size, size_t alignment) {
  YASSERT(IS_POWER_OF_2(alignment),
          "Alignment must be a power of 2: %u",
          static_cast<uint32_t>(alignment));
  size_t buffer =
      reinterpret_cast<size_t>(Allocate(allocation_size + alignment));
  return reinterpret_cast<void*>(ROUND_UP(buffer, alignment));
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
