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
  AtomicHashTable();
  AtomicHashTable(void* buffer, size_t buffer_size,
                  size_t num_entries, size_t max_value_size);
  ~AtomicHashTable();

  void Init(void* buffer, size_t buffer_size,
            size_t num_entries, size_t max_value_size);
  void Reset();

  uint64_t Insert(const void* key, size_t key_size,
                  const void* value, size_t value_size);
  void Insert(uint64_t hash_key,
              const void* value, size_t value_size);

  const void* const GetValue(const void* key, size_t key_size) const;
  const void* const GetValue(uint64_t hash_key) const;

  void* GetValue(const void* key, size_t key_size);
  void* GetValue(uint64_t hash_key);

 private:
  void* mBuffer;
  size_t mNumEntries;
  size_t mMaxValueSize;
};

template <typename T>
class TypedAtomicHashTable : public AtomicHashTable {
 public:
  TypedAtomicHashTable() : AtomicHashTable() {}
  TypedAtomicHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : AtomicHashTable(buffer, buffer_size, num_entries, sizeof(T)) {}
  ~TypedAtomicHashTable() {}

  void Init(void* buffer, size_t buffer_size, size_t num_entries) {
    AtomicHashTable::Init(buffer, buffer_size, num_entries, sizeof(T));
  }

  uint64_t Insert(const void* key, size_t key_size, const T& value) {
    return AtomicHashTable::Insert(key, key_size, &value, sizeof(value));
  }

  void Insert(uint64_t hash_key, const T& value) {
    return AtomicHashTable::Insert(hash_key, &value, sizeof(value));
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
  FullTypedAtomicHashTable() : TypedAtomicHashTable() {}
  FullTypedAtomicHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : TypedAtomicHashTable(buffer, buffer_size, num_entries) {}
  ~FullTypedAtomicHashTable() {}

  uint64_t Insert(const T1& key, const T2& value) {
    return AtomicHashTable::Insert(&key, sizeof(key), &value, sizeof(value));
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
