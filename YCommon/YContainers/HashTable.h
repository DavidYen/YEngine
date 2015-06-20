#ifndef YCOMMON_YCONTAINERS_HASHTABLE_H
#define YCOMMON_YCONTAINERS_HASHTABLE_H

/*******
* HashTable inserts/retrieves hash values.
*   - buffer size: (max_value_size + sizeof(uint64_t)) * num_entries.
********/

namespace YCommon { namespace YContainers {

class HashTable {
 public:
  HashTable();
  HashTable(void* buffer, size_t buffer_size,
            size_t num_entries, size_t max_value_size);
  ~HashTable();

  void Init(void* buffer, size_t buffer_size,
            size_t num_entries, size_t max_value_size);
  void Reset(); // Initializes pointers to null.
  void Clear(); // Clears out all entires.

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
class TypedHashTable : public HashTable {
 public:
  TypedHashTable() : HashTable() {}
  TypedHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : HashTable(buffer, buffer_size, num_entries, sizeof(T)) {}
  ~TypedHashTable() {}

  void Init(void* buffer, size_t buffer_size, size_t num_entries) {
    HashTable::Init(buffer, buffer_size, num_entries, sizeof(T));
  }

  uint64_t Insert(const void* key, size_t key_size, const T& value) {
    return HashTable::Insert(key, key_size, &value, sizeof(value));
  }

  void Insert(uint64_t hash_key, const T& value) {
    return HashTable::Insert(hash_key, &value, sizeof(value));
  }

  const T* const GetValue(const void* key, size_t size) const {
    return static_cast<const T* const>(HashTable::GetValue(key, size));
  }
  const T* const GetValue(uint64_t hash_key) const {
    return static_cast<const T* const>(HashTable::GetValue(hash_key));
  }

  T* GetValue(const void* key, size_t size) {
    return static_cast<T*>(HashTable::GetValue(key, size));
  }

  T* GetValue(uint64_t hash_key) {
    return static_cast<T*>(HashTable::GetValue(hash_key));
  }
};

template <typename T1, typename T2>
class FullTypedHashTable : public TypedHashTable<T2> {
 public:
  FullTypedHashTable() : TypedHashTable() {}
  FullTypedHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : TypedHashTable(buffer, buffer_size, num_entries) {}
  ~FullTypedHashTable() {}

  uint64_t Insert(const T1& key, const T2& value) {
    return TypedHashTable::Insert(&key, sizeof(key), value);
  }

  const T2* const GetValue(const T1& key) const {
    return static_cast<const T2* const>(TypedHashTable::GetValue(&key,
                                                                 sizeof(T1)));
  }

  T2* GetValue(T1& key) {
    return static_cast<T2*>(TypedHashTable::GetValue(&key, sizeof(T1)));
  }
};

template <typename T, size_t entries>
class ContainedHashTable : public TypedHashTable<T> {
 public:
  ContainedHashTable()
      : TypedHashTable(mBuffer, sizeof(mBuffer), entries) {}
  ~ContainedHashTable() {}

  void Reset() {
    Init(mBuffer, sizeof(mBuffer), entries, sizeof(T));
  }

 private:
  ALIGN_FRONT(8)
  uint8_t mBuffer[(sizeof(uint64_t)+sizeof(T))*entries] ALIGN_BACK(8);
};

template <typename T1, typename T2, size_t entries>
class ContainedFullHashTable : public FullTypedHashTable<T1, T2> {
 public:
  ContainedFullHashTable()
      : FullTypedHashTable(mBuffer, sizeof(mBuffer), entries) {}
  ~ContainedFullHashTable() {}

  void Reset() {
    Init(mBuffer, sizeof(mBuffer), entries);
  }

 private:
  ALIGN_FRONT(8)
  uint8_t mBuffer[(sizeof(uint64_t)+sizeof(T2))*entries] ALIGN_BACK(8);
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_HASHTABLE_H

