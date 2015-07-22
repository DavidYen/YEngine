#ifndef YCOMMON_YCONTAINERS_REFPOINTER_H
#define YCOMMON_YCONTAINERS_REFPOINTER_H

#include <YCommon/Headers/Macros.h>

namespace YCommon { namespace YContainers {

class RefPointer {
 public:
  RefPointer()
    : mReadRefs(0),
      mWriteRefs(0),
      mReadWriteRefs(0),
      mPointer(nullptr) {
  }

  RefPointer(void* pointer)
    : mReadRefs(0),
      mWriteRefs(0),
      mReadWriteRefs(0),
      mPointer(pointer) {
  }

  ~RefPointer() {
    Reset();
  }

  void Reset() {
    YASSERT(mReadRefs == 0, "Read references not zero: %u", mReadRefs);
    YASSERT(mWriteRefs == 0, "Write references not zero: %u", mWriteRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mPointer = nullptr;
  }

  void Set(void* pointer) {
    Reset();
    mPointer = pointer;
  }

  const void* GetReadRef() {
    YASSERT(mWriteRefs == 0, "Write references not zero: %u", mWriteRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mReadRefs++;
    return mPointer;
  }

  void* GetWriteRef() {
    YASSERT(mReadRefs == 0, "Read references not zero: %u", mReadRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mWriteRefs++;
    return mPointer;
  }

  void* GetReadWriteRef() {
    YASSERT(mReadRefs == 0, "Read references not zero: %u", mReadRefs);
    YASSERT(mWriteRefs == 0, "Write references not zero: %u", mWriteRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mReadWriteRefs++;
    return mPointer;
  }

  void ReleaseReadRef() {
    YASSERT(mReadRefs != 0, "Read references is zero");
    mReadRefs--;
  }

  void ReleaseWriteRef() {
    YASSERT(mWriteRefs != 0, "Write references is zero");
    mWriteRefs--;
  }

  void ReleaseReadWriteRef() {
    YASSERT(mReadWriteRefs != 0, "Read/Write references is zero");
    mReadWriteRefs--;
  }

  RefPointer& operator=(void* rhs) {
    Reset();
    mPointer = rhs;
    return *this;
  }

  bool operator==(const void* rhs) const {
    return mPointer == rhs;
  }

  bool operator!=(const void* rhs) const {
    return mPointer != rhs;
  }

 private:
  uint8_t mReadRefs;
  uint8_t mWriteRefs;
  uint8_t mReadWriteRefs;
  void* mPointer;
};

template <typename T>
class TypedRefPointer : public RefPointer {
 public:
  const T* GetReadRef() {
    return static_cast<const T*>(RefPointer::GetReadRef());
  }

  T* GetWriteRef() {
    return static_cast<T*>(RefPointer::GetWriteRef());
  }

  T* GetReadWriteRef() {
    return static_cast<T*>(RefPointer::GetReadWriteRef());
  }
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_REFPOINTER_H
