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

  uint64_t Insert(const void* key, size_t key_size,
                  const void* value, size_t value_size);
  void Insert(uint64_t hash_key,
              const void* value, size_t value_size);

  const void* GetValue(const void* key, size_t key_size) const;
  const void* GetValue(uint64_t hash_key) const;

  void* GetValue(const void* key, size_t key_size);
  void* GetValue(uint64_t hash_key);

 private:
  void* mBuffer;
  size_t mNumEntries;
  size_t mMaxValueSize;
};

template<typename T>
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

  const T& GetValue(const void* key, size_t key_size) const {
    return *static_cast<const T*>(AtomicHashTable::GetValue(key, key_size));
  }
  const T& GetValue(uint64_t hash_key) const {
    return *static_cast<const T*>(AtomicHashTable::GetValue(hash_key));
  }

  T& GetValue(const void* key, size_t key_size) {
    return *static_cast<T*>(AtomicHashTable::GetValue(key, key_size));
  }

  T& GetValue(uint64_t hash_key) {
    return *static_cast<T*>(AtomicHashTable::GetValue(hash_key));
  }
};

template <typename T, size_t entries>
class ContainedAtomicHashTable : public TypedAtomicHashTable<T> {
 public:
  ContainedAtomicHashTable()
      : TypedAtomicHashTable(mBuffer, sizeof(mBuffer), entries) {}
  ~ContainedAtomicHashTable() {}

 private:
  ALIGN_FRONT(8)
  uint8_t mBuffer[(sizeof(uint64_t)+sizeof(T))*entries] ALIGN_BACK(8);
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H
