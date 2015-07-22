#ifndef YCOMMON_YCONTAINERS_REFPOINTER_H
#define YCOMMON_YCONTAINERS_REFPOINTER_H

#include <YCommon/Headers/Macros.h>

namespace YCommon { namespace YContainers {

class ReadRefData;
class WriteRefData;
class ReadWriteRefData;

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

  ReadRefData GetReadRef();
  WriteRefData GetWriteRef();
  ReadWriteRefData GetReadWriteRef();

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
  friend class BaseRefData;
  friend class ReadRefData;
  friend class WriteRefData;
  friend class ReadWriteRefData;

  uint8_t mReadRefs;
  uint8_t mWriteRefs;
  uint8_t mReadWriteRefs;
  void* mPointer;
};

class BaseRefData {
 public:
  bool operator==(const void* rhs) const {
    return mPointer == rhs;
  }

  bool operator!=(const void* rhs) const {
    return mPointer != rhs;
  }

 protected:
  BaseRefData(RefPointer* ref_pointer)
    : mRefPointer(ref_pointer),
      mPointer(ref_pointer->mPointer) {
  }

  ~BaseRefData() {}

  void* mPointer;
  RefPointer* mRefPointer;
};

class ReadRefData : public BaseRefData {
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
            "Write references not zero: %u", ref_pointer->mWriteRefs);
    YASSERT(ref_pointer->mReadWriteRefs == 0,
            "Read/Write references not zero: %u", ref_pointer->mReadWriteRefs);
    mRefPointer->mReadRefs++;
  }
};

template <typename T>
class TypedReadRefData : public ReadRefData {
 public:
  const T* GetData() { return static_cast<const T*>(ReadRefData::GetData()); }
};

class WriteRefData : public BaseRefData {
 public:
  ~WriteRefData() {
    YASSERT(mRefPointer->mWriteRefs != 0, "Write references is zero");
    mRefPointer->mWriteRefs--;
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
            "Read references not zero: %u", ref_pointer->mReadRefs);
    YASSERT(ref_pointer->mReadWriteRefs == 0,
            "Read/Write references not zero: %u", ref_pointer->mReadWriteRefs);
    ref_pointer->mWriteRefs++;
  }
};

template <typename T>
class TypedWriteRefData : public WriteRefData {
 public:
  T* GetData() { return static_cast<T*>(WriteRefData::GetData()); }
};

class ReadWriteRefData : public BaseRefData {
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
            "Read references not zero: %u", ref_pointer->mReadRefs);
    YASSERT(ref_pointer->mWriteRefs == 0,
            "Write references not zero: %u", ref_pointer->mWriteRefs);
    YASSERT(ref_pointer->mReadWriteRefs == 0,
            "Read/Write references not zero: %u", ref_pointer->mReadWriteRefs);
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
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_REFPOINTER_H
