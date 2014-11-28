#ifndef YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H
#define YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H

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

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_ATOMICHASHTABLE_H
