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

  RefPointer(RefPointer&& other)
    : mReadRefs(0),
      mWriteRefs(0),
      mReadWriteRefs(0),
      mPointer(other.mPointer) {
    other.Reset();
  }

  RefPointer(const RefPointer& other)
    : mReadRefs(0),
      mWriteRefs(0),
      mReadWriteRefs(0),
      mPointer(other.mPointer) {
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

  uint8_t ReadRefCount() const { return mReadRefs; }
  uint8_t WriteRefCount() const { return mWriteRefs; }
  uint8_t ReadWriteRefCount() const { return mReadWriteRefs; }

  ReadRefData GetReadRef();
  WriteRefData GetWriteRef();
  ReadWriteRefData GetReadWriteRef();

  RefPointer& operator=(void* rhs) {
    Reset();
    mPointer = rhs;
    return *this;
  }

  RefPointer& operator=(RefPointer&& rhs) {
    Reset();
    mPointer = rhs.mPointer;
    rhs.Reset();
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

  void IncReadRefs() {
    YASSERT(mWriteRefs == 0,
            "Write references not zero: %u", mWriteRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mReadRefs++;
  }

  void DecReadRefs() {
    YASSERT(mReadRefs != 0, "Read references is zero");
    mReadRefs--;
  }

  void IncWriteRefs() {
    YASSERT(mReadRefs == 0,
            "Read references not zero: %u", mReadRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mWriteRefs++;
  }

  void DecWriteRefs() {
    YASSERT(mWriteRefs != 0, "Write references is zero");
    mWriteRefs--;
  }

  void IncReadWriteRefs() {
    YASSERT(mReadRefs == 0,
            "Read references not zero: %u", mReadRefs);
    YASSERT(mWriteRefs == 0,
            "Write references not zero: %u", mWriteRefs);
    YASSERT(mReadWriteRefs == 0,
            "Read/Write references not zero: %u", mReadWriteRefs);
    mReadWriteRefs++;
  }

  void DecReadWriteRefs() {
    YASSERT(mReadWriteRefs != 0,
            "Read/Write references is zero");
    mReadWriteRefs--;
  }

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

  BaseRefData(const BaseRefData& copy)
    : mRefPointer(copy.mRefPointer),
      mPointer(copy.mPointer) {
  }

  BaseRefData(BaseRefData&& other)
    : mRefPointer(other.mRefPointer),
      mPointer(other.mPointer) {
    other.mRefPointer = nullptr;
    other.mPointer = nullptr;
  }

  ~BaseRefData() {}

  void Reset() {
    mPointer = nullptr;
    mRefPointer = nullptr;
  }

  void* mPointer;
  RefPointer* mRefPointer;
};

class ReadRefData : public BaseRefData {
 public:
  ReadRefData(const ReadRefData& copy)
    : BaseRefData(copy) {
    if (mRefPointer) {
      mRefPointer->IncReadRefs();
    }
  }

  ReadRefData(ReadRefData&& other)
    : BaseRefData(other) {
  }

  ~ReadRefData() {
    Reset();
  }

  void Reset() {
    if (mRefPointer)
      mRefPointer->DecReadRefs();
    BaseRefData::Reset();
  }

  const void* GetData() {
    YDEBUG_CHECK(mRefPointer == nullptr || mRefPointer->mPointer == mPointer,
                 "Reference Pointer out of date.");
    return mPointer;
  }

  ReadRefData& operator=(const ReadRefData& other) {
    Reset();
    mPointer = other.mPointer;
    mRefPointer = other.mRefPointer;
    if (mRefPointer)
      mRefPointer->IncReadRefs();
    return *this;
  }

 private:
  friend class RefPointer;
  ReadRefData(RefPointer* ref_pointer)
    : BaseRefData(ref_pointer) {
    ref_pointer->IncReadRefs();
  }
};

template <typename T>
class TypedReadRefData : public ReadRefData {
 public:
  TypedReadRefData(const TypedReadRefData& copy)
    : ReadRefData(copy) {
  }

  TypedReadRefData(TypedReadRefData&& other)
    : ReadRefData(other) {
  }

  ~TypedReadRefData() {
    Reset();
  }

  const T* GetData() { return static_cast<const T*>(ReadRefData::GetData()); }
};

class WriteRefData : public BaseRefData {
 public:
  WriteRefData(const WriteRefData& copy)
    : BaseRefData(copy) {
    if (mRefPointer) {
      mRefPointer->IncWriteRefs();
    }
  }

  WriteRefData(WriteRefData&& other)
    : BaseRefData(other) {
  }

  ~WriteRefData() {
    Reset();
  }

  void Reset() {
    if (mRefPointer)
      mRefPointer->DecWriteRefs();
    BaseRefData::Reset();
  }

  void* GetData() {
    YDEBUG_CHECK(mRefPointer == nullptr || mRefPointer->mPointer == mPointer,
                 "Reference Pointer out of date.");
    return mPointer;
  }

  WriteRefData& operator=(const WriteRefData& other) {
    Reset();
    mPointer = other.mPointer;
    mRefPointer = other.mRefPointer;
    if (mRefPointer)
      mRefPointer->IncWriteRefs();
    return *this;
  }

 protected:
  friend class RefPointer;
  WriteRefData(RefPointer* ref_pointer)
    : BaseRefData(ref_pointer) {
    YDEBUG_CHECK(ref_pointer, "Cannot construct a reference to NULL.");
    ref_pointer->IncWriteRefs();
  }
};

template <typename T>
class TypedWriteRefData : public WriteRefData {
 public:
  T* GetData() { return static_cast<T*>(WriteRefData::GetData()); }
};

class ReadWriteRefData : public BaseRefData {
 public:
  ReadWriteRefData(ReadWriteRefData& copy)
    : BaseRefData(copy) {
    // Simulate a move
    copy.mPointer = nullptr;
    copy.mRefPointer = nullptr;
  }

  ReadWriteRefData(ReadWriteRefData&& other)
    : BaseRefData(other) {
  }

  ~ReadWriteRefData() {
    Reset();
  }

  void Reset() {
    if (mRefPointer)
      mRefPointer->DecReadWriteRefs();
    BaseRefData::Reset();
  }

  void* GetData() {
    YDEBUG_CHECK(mRefPointer == nullptr || mRefPointer->mPointer == mPointer,
                 "Reference Pointer out of date.");
    return mPointer;
  }

  ReadWriteRefData& operator=(ReadWriteRefData& other) {
    // Simulate a "move"
    Reset();
    mPointer = other.mPointer;
    mRefPointer = other.mRefPointer;
    other.mPointer = nullptr;
    other.mRefPointer = nullptr;
    return *this;
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
