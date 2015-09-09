#ifndef YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H
#define YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H

/*******
* AtomicHashTable atomically inserts/retrieves hash values.
*   - It is undefined when the same hash value is inserted at the same time.
*   - buffer size: (max_value_size + sizeof(uint64_t)) * num_entries.
********/

namespace YCommon { namespace YContainers {

class AtomicHashTable {
 public:
  static size_t GetAllocationSize(size_t num_entries, size_t max_value_size) {
    return (max_value_size + sizeof(uint64_t)) * num_entries;
  }

  AtomicHashTable();
  AtomicHashTable(void* buffer, size_t buffer_size,
                  size_t num_entries, size_t max_value_size);
  ~AtomicHashTable();

  void Init(void* buffer, size_t buffer_size,
            size_t num_entries, size_t max_value_size);
  void Reset();
  void Clear();

  void* Insert(const void* key, size_t key_size,
               const void* value, size_t value_size,
               uint64_t* hash_key = nullptr);
  void* Insert(uint64_t hash_key,
               const void* value, size_t value_size);

  bool Remove(const void* key, size_t key_size);
  bool Remove(uint64_t hash_key);
  bool Remove(const void* hash_table_value);

  const void* const GetValue(const void* key, size_t key_size) const;
  const void* const GetValue(uint64_t hash_key) const;

  void* GetValue(const void* key, size_t key_size);
  void* GetValue(uint64_t hash_key);

  int32_t GetCurrentSize() const { return mCurrentEntries; }

 private:
  void* mBuffer;
  size_t mNumEntries;
  size_t mMaxValueSize;
  volatile int32_t mCurrentEntries;
};

template <typename T>
class TypedAtomicHashTable : public AtomicHashTable {
 public:
  static size_t GetAllocationSize(size_t num_entries) {
    return AtomicHashTable::GetAllocationSize(num_entries, sizeof(T));
  }

  TypedAtomicHashTable() : AtomicHashTable() {}
  TypedAtomicHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : AtomicHashTable(buffer, buffer_size, num_entries, sizeof(T)) {}
  ~TypedAtomicHashTable() {}

  void Init(void* buffer, size_t buffer_size, size_t num_entries) {
    AtomicHashTable::Init(buffer, buffer_size, num_entries, sizeof(T));
  }

  T* Insert(const void* key, size_t key_size, const T& value,
            uint64_t* hash_key = nullptr) {
    return static_cast<T*>(AtomicHashTable::Insert(key, key_size, &value,
                                                   sizeof(value), hash_key));
  }

  T* Insert(uint64_t hash_key, const T& value) {
    return static_cast<T*>(AtomicHashTable::Insert(hash_key,
                                                   &value, sizeof(value)));
  }

  const T* const GetValue(const void* key, size_t size) const {
    return static_cast<const T* const>(AtomicHashTable::GetValue(key, size));
  }
  const T* const GetValue(uint64_t hash_key) const {
    return static_cast<const T* const>(AtomicHashTable::GetValue(hash_key));
  }

  T* GetValue(const void* key, size_t size) {
    return static_cast<T*>(AtomicHashTable::GetValue(key, size));
  }

  T* GetValue(uint64_t hash_key) {
    return static_cast<T*>(AtomicHashTable::GetValue(hash_key));
  }
};

template <typename T1, typename T2>
class FullTypedAtomicHashTable : public TypedAtomicHashTable<T2> {
 public:
  static size_t GetAllocationSize(size_t num_entries) {
    return AtomicHashTable::GetAllocationSize(num_entries, sizeof(T2));
  }

  FullTypedAtomicHashTable() : TypedAtomicHashTable() {}
  FullTypedAtomicHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : TypedAtomicHashTable(buffer, buffer_size, num_entries) {}
  ~FullTypedAtomicHashTable() {}

  T2* Insert(const T1& key, const T2& value, uint64_t* hash_key = nullptr) {
    return TypedAtomicHashTable::Insert(&key, sizeof(key), value, hash_key);
  }

  const T2* const GetValue(const T1& key) const {
    return static_cast<const T2* const>(AtomicHashTable::GetValue(&key,
                                                                  sizeof(T1)));
  }

  T2* GetValue(T1& key) {
    return static_cast<T2*>(AtomicHashTable::GetValue(&key, sizeof(T1)));
  }
};

template <typename T, size_t entries>
class ContainedAtomicHashTable : public TypedAtomicHashTable<T> {
 public:
  static size_t GetAllocationSize() {
    return AtomicHashTable::GetAllocationSize(entries, sizeof(T));
  }

  ContainedAtomicHashTable()
      : TypedAtomicHashTable(mBuffer, sizeof(mBuffer), entries) {}
  ~ContainedAtomicHashTable() {}

  void Reset() {
    Init(mBuffer, sizeof(mBuffer), entries, sizeof(T));
  }

 private:
  ALIGN_FRONT(8)
  uint8_t mBuffer[(sizeof(uint64_t)+sizeof(T))*entries] ALIGN_BACK(8);
};

template <typename T1, typename T2, size_t entries>
class ContainedFullAtomicHashTable : public FullTypedAtomicHashTable<T1, T2> {
 public:
  static size_t GetAllocationSize() {
    return AtomicHashTable::GetAllocationSize(entries, sizeof(T2));
  }

  ContainedFullAtomicHashTable()
      : FullTypedAtomicHashTable(mBuffer, sizeof(mBuffer), entries) {}
  ~ContainedFullAtomicHashTable() {}

  void Reset() {
    Init(mBuffer, sizeof(mBuffer), entries);
  }

 private:
  ALIGN_FRONT(8)
  uint8_t mBuffer[(sizeof(uint64_t)+sizeof(T2))*entries] ALIGN_BACK(8);
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H
