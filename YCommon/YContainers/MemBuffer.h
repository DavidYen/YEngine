#ifndef YCOMMON_YCONTAINERS_MEMBUFFER_H
#define YCOMMON_YCONTAINERS_MEMBUFFER_H

/*******
* MemBuffer manages a simple buffer of memory, long lived allocations are
*   allocated from the front while scratch pad memory is allocated from the end.
********/

namespace YCommon { namespace YContainers {

class MemBuffer {
 public:
  MemBuffer();
  MemBuffer(void* buffer, size_t buffer_size);

  void Init(void* buffer, size_t buffer_size);
  void Reset();

  void Clear();

  void* Allocate(size_t allocation_size);
  void Free(size_t allocation_size);


  size_t AllocatedBufferSpace() const {
    return mUsedBufferSize;
  }

  size_t AllocatedScratchPadSpace() const {
    return mUsedScratchPadSize;
  }

  size_t AllocatedTotalSpace() const {
    return mUsedBufferSize + mUsedScratchPadSize;
  }

  size_t FreeSpace() const {
    return mBufferSize - AllocatedTotalSpace();
  }

  class ScratchPad {
   public:
    ScratchPad(MemBuffer* buffer, size_t scratch_pad_size);
    ~ScratchPad();

    void* Data() const { return mScratchPadMemory; }
    size_t Size() const { return mScratchPadSize; }

   protected:
    MemBuffer* mBuffer;
    void* mScratchPadMemory;
    size_t mScratchPadSize;
  };

 private:
  friend class ScratchPad;

  void* mBuffer;
  size_t mUsedBufferSize;
  size_t mUsedScratchPadSize;
  size_t mBufferSize;
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_MEMBUFFER_H
