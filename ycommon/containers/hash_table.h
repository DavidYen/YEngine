#ifndef YCOMMON_CONTAINERS_HASH_TABLE_H
#define YCOMMON_CONTAINERS_HASH_TABLE_H

#include <stdint.h>

#include "ycommon/headers/macros.h"

/*******
* HashTable inserts/retrieves hash values.
*   - buffer size: (max_value_size + sizeof(uint64_t)) * num_entries.
********/

namespace ycommon { namespace containers {

class HashTable {
 public:
  static size_t GetAllocationSize(size_t num_entries, size_t max_value_size) {
    return (max_value_size + sizeof(uint64_t)) * num_entries;
  }

  HashTable();
  HashTable(void* buffer, size_t buffer_size,
            size_t num_entries, size_t max_value_size);
  ~HashTable();

  void Init(void* buffer, size_t buffer_size,
            size_t num_entries, size_t max_value_size);
  void Reset(); // Initializes pointers to null.
  void Clear(); // Clears out all entires.

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
  bool RemoveHelper(uint32_t index);

  void* mBuffer;
  int32_t mCurrentEntries;
  size_t mNumEntries;
  size_t mMaxValueSize;
};

template <typename T>
class TypedHashTable : public HashTable {
 public:
  static size_t GetAllocationSize(size_t num_entries) {
    return HashTable::GetAllocationSize(num_entries, sizeof(T));
  }

  TypedHashTable() : HashTable() {}
  TypedHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : HashTable(buffer, buffer_size, num_entries, sizeof(T)) {}
  ~TypedHashTable() {}

  void Init(void* buffer, size_t buffer_size, size_t num_entries) {
    HashTable::Init(buffer, buffer_size, num_entries, sizeof(T));
  }

  T* Insert(const void* key, size_t key_size, const T& value,
            uint64_t* hash_key = nullptr) {
    return static_cast<T*>(
        HashTable::Insert(key, key_size, &value, sizeof(value), hash_key));
  }

  T* Insert(uint64_t hash_key, const T& value) {
    return static_cast<T*>(HashTable::Insert(hash_key, &value, sizeof(value)));
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
  static size_t GetAllocationSize(size_t num_entries) {
    return HashTable::GetAllocationSize(num_entries, sizeof(T2));
  }

  FullTypedHashTable() : TypedHashTable() {}
  FullTypedHashTable(void* buffer, size_t buffer_size, size_t num_entries)
      : TypedHashTable(buffer, buffer_size, num_entries) {}
  ~FullTypedHashTable() {}

  T2* Insert(const T1& key, const T2& value, uint64_t* hash_key = nullptr) {
    return TypedHashTable::Insert(&key, sizeof(key), value, hash_key);
  }

  T2* Insert(uint64_t hash_key, const T2& value) {
    return TypedHashTable::Insert(hash_key, value);
  }

  const T2* const GetValue(const T1& key) const {
    return TypedHashTable::GetValue(&key, sizeof(key));
  }

  T2* GetValue(uint64_t hash_key) const {
    return TypedHashTable::GetValue(hash_key);
  }

  T2* GetValue(T1& key) {
    return TypedHashTable::GetValue(&key, sizeof(key));
  }

  T2* GetValue(uint64_t hash_key) {
    return TypedHashTable::GetValue(hash_key);
  }

  bool Remove(const T1& key) {
    return TypedHashTable::Remove(&key, sizeof(key));
  }

  bool Remove(const T2* value) {
    return TypedHashTable::Remove(value);
  }
};

template <typename T, size_t entries>
class ContainedHashTable : public TypedHashTable<T> {
 public:
  static size_t GetAllocationSize() {
    return HashTable::GetAllocationSize(entries, sizeof(T));
  }

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
  static size_t GetAllocationSize() {
    return HashTable::GetAllocationSize(entries, sizeof(T2));
  }

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

}} // namespace ycommon { namespace containers {

#endif // YCOMMON_CONTAINERS_HASH_TABLE_H