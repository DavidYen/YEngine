#include <YCommon/Headers/stdincludes.h>
#include "RefPointer.h"

#include <YCommon/Headers/Macros.h>

namespace YCommon { namespace YContainers {

ReadRefData RefPointer::GetReadRef() {
  return ReadRefData(this);
}

WriteRefData RefPointer::GetWriteRef() {
  return WriteRefData(this);
}

ReadWriteRefData RefPointer::GetReadWriteRef() {
  return ReadWriteRefData(this);
}

/*class BaseRefData {
 public:
  BaseRefData(RefPointer* ref_pointer)
    : mRefPointer(ref_pointer),
      mPointer(ref_pointer->mPointer) {
  }

  ~BaseRefData() {}

  void* mPointer;
  RefPointer* mRefPointer;
};

class ReadRefData : private BaseRefData {
 public:
  ~ReadRefData() {
    YASSERT(mRefPointer->mReadRefs != 0, "Read references is zero");
    mRefPointer->mReadRefs--;
  }

  const void* GetData() {
    YDEBUG_CHECK(mRefPointer->mPointer == mPointer,
                 "Reference Pointer out of date.");
    return mPointer;
  }

 private:
  friend class RefPointer;
  ReadRefData(RefPointer* ref_pointer)
    : BaseRefData(ref_pointer) {
    YASSERT(ref_pointer->mWriteRefs == 0,
            "Write references not zero: %u", mWriteRefs);
    YASSERT(ref_pointer->mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mRefPointer->mReadRefs++;
  }
};

template <typename T>
class TypedReadRefData : public ReadRefData {
 public:
  const T* GetData() { return static_cast<const T*>(ReadRefData::GetData()); }
};

class WriteRefData {
 public:
  ~WriteRefData() {
    YASSERT(mRefPointer->mWriteRefs != 0, "Write references is zero");
    mRefPointer->mReadRefs--;
  }

  void* GetData() {
    YDEBUG_CHECK(mRefPointer->mPointer == mPointer,
                 "Reference Pointer out of date.");
    return mPointer;
  }

 private:
  friend class RefPointer;
  WriteRefData(RefPointer* ref_pointer)
    : BaseRefData(ref_pointer) {
    YASSERT(ref_pointer->mReadRefs == 0,
            "Read references not zero: %u", mReadRefs);
    YASSERT(ref_pointer->mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    ref_pointer->mWriteRefs++;
  }
};

template <typename T>
class TypedWriteRefData : public WriteRefData {
 public:
  T* GetData() { return static_cast<T*>(WriteRefData::GetData()); }
};

class ReadWriteRefData {
 public:
  ~ReadWriteRefData() {
    YASSERT(mRefPointer->mReadWriteRefs != 0, "Read/Write references is zero");
    mRefPointer->mReadWriteRefs--;
  }

  void* GetData() {
    YDEBUG_CHECK(mRefPointer->mPointer == mPointer,
                 "Reference Pointer out of date.");
    return mPointer;
  }

 private:
  friend class RefPointer;
  ReadWriteRefData(RefPointer* ref_pointer)
    : BaseRefData(ref_pointer) {
    YASSERT(ref_pointer->mReadRefs == 0,
            "Read references not zero: %u", mReadRefs);
    YASSERT(ref_pointer->mWriteRefs == 0,
            "Write references not zero: %u", mWriteRefs);
    YASSERT(ref_pointer->mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    ref_pointer->mReadWriteRefs++;
  }
};

template <typename T>
class TypedReadWriteRefData : public ReadWriteRefData {
 public:
  T* GetData() { return static_cast<T*>(ReadWriteRefData::GetData()); }
};

template <typename T>
class TypedRefPointer : public RefPointer {
 public:
  TypedReadRefData<T> GetReadRef() { return TypedReadRefData<T>(this); }
  TypedWriteRefData<T> GetWriteRef() { return TypedWriteRefData<T>(this); }
  TypedReadWriteRefData<T> GetReadWriteRef() {
    return TypedReadWriteRefData<T>(this);
  }
};*/

}} // namespace YCommon { namespace YContainers {
