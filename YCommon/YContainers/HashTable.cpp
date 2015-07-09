#include <YCommon/Headers/stdincludes.h>
#include "HashTable.h"

#include <string>

#include <YCommon/YUtils/Hash.h>

#define EMPTY_VALUE static_cast<uint64_t>(0)
#define REMOVED_VALUE static_cast<uint64_t>(-1)
#define MAX_TRIES 10

namespace YCommon { namespace YContainers {

HashTable::HashTable()
    : mBuffer(NULL),
      mCurrentEntries(0),
      mNumEntries(0),
      mMaxValueSize(0) {
}

HashTable::HashTable(void* buffer, size_t buffer_size,
                     size_t num_entries, size_t max_value_size)
    : mBuffer(NULL),
      mCurrentEntries(0),
      mNumEntries(0),
      mMaxValueSize(0) {
  Init(buffer, buffer_size, num_entries, max_value_size);
}

HashTable::~HashTable() {
}

void HashTable::Init(void* buffer, size_t buffer_size,
                     size_t num_entries, size_t max_value_size) {
  YASSERT(reinterpret_cast<uintptr_t>(buffer) % sizeof(uint64_t) == 0,
          "Hash Table memory must be aligned to %u bytes - supplied %p.",
          static_cast<uint32_t>(sizeof(uint64_t)),
          buffer);
  YASSERT(num_entries >= MAX_TRIES,
          "Hash Table entry count must be at least %u - supplied %u.",
          static_cast<uint32_t>(MAX_TRIES),
          static_cast<uint32_t>(num_entries));

  const size_t entry_size = sizeof(uint64_t) + max_value_size;
  const size_t total_table_size = entry_size * num_entries;

  YASSERT(total_table_size <= buffer_size,
          "Hash Table requires %u bytes, supplied %u bytes.",
          static_cast<uint32_t>(total_table_size),
          static_cast<uint32_t>(buffer_size));

  mBuffer = buffer;
  mCurrentEntries = 0;
  mNumEntries = num_entries;
  mMaxValueSize = max_value_size;

  Clear();
}

void HashTable::Reset() {
  mBuffer = NULL;
  mCurrentEntries = 0;
  mNumEntries = 0;
  mMaxValueSize = 0;
}

void HashTable::Clear() {
  mCurrentEntries = 0;
  memset(mBuffer, EMPTY_VALUE, sizeof(uint64_t) * mNumEntries);
}

void* HashTable::Insert(const void* key, size_t key_size,
                        const void* value, size_t value_size,
                        uint64_t* hash_key) {
  const uint64_t hash_key_value = YCommon::YUtils::Hash::Hash64(key, key_size);
  if (hash_key)
    *hash_key = hash_key_value;
  return Insert(hash_key_value, value, value_size);
}

void* HashTable::Insert(uint64_t hash_key,
                       const void* value, size_t value_size) {
  YDEBUG_CHECK(value_size <= mMaxValueSize,
               "Invalid Hash Value size: Maximum size is %u. Supplied %u.",
               static_cast<uint32_t>(mMaxValueSize),
               static_cast<uint32_t>(value_size));

  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  uint64_t* hash_table = static_cast<uint64_t*>(mBuffer);
  uint8_t* hash_value_table = static_cast<uint8_t*>(mBuffer) + key_table_size;

  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key ||
        hash_table_value == REMOVED_VALUE ||
        hash_table_value == EMPTY_VALUE) {
      hash_table[try_index] = hash_key;
      void* hash_data = hash_value_table + (try_index * mMaxValueSize);
      memcpy(hash_data, value, value_size);
      ++mCurrentEntries;
      return hash_data;
    }
  }

  YFATAL("Atomic Hash Table is too full, maximum amount of tries reached!");
  return nullptr;
}

bool HashTable::Remove(const void* key, size_t key_size) {
  const uint64_t hash_key = YCommon::YUtils::Hash::Hash64(key, key_size);
  return Remove(hash_key);
}

bool HashTable::Remove(uint64_t hash_key) {
  uint64_t* hash_table = static_cast<uint64_t*>(mBuffer);
  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key) {
      hash_table[try_index] = REMOVED_VALUE;
      --mCurrentEntries;
      return true;
    } else if (hash_table_value == EMPTY_VALUE) {
      return false;
    }
  }
  return false;
}

const void* const HashTable::GetValue(const void* key,
                                      size_t key_size) const {
  return GetValue(YCommon::YUtils::Hash::Hash64(key, key_size));
}

const void* const HashTable::GetValue(uint64_t hash_key) const {
  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  uint64_t* hash_table = static_cast<uint64_t*>(mBuffer);
  const uint8_t* hash_value_table =
      static_cast<const uint8_t*>(mBuffer) + key_table_size;

  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key) {
      return hash_value_table + (try_index * mMaxValueSize);
    } else if (hash_table_value == EMPTY_VALUE) {
      return NULL;
    }
  }

  return NULL;
}

void* HashTable::GetValue(const void* key, size_t key_size) {
  return GetValue(YCommon::YUtils::Hash::Hash64(key, key_size));
}

void* HashTable::GetValue(uint64_t hash_key) {
  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  uint64_t* hash_table = static_cast<uint64_t*>(mBuffer);
  uint8_t* hash_value_table = static_cast<uint8_t*>(mBuffer) + key_table_size;

  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key) {
      return hash_value_table + (try_index * mMaxValueSize);
    } else if (hash_table_value == EMPTY_VALUE) {
      return NULL;
    }
  }

  return NULL;
}

}} // namespace YCommon { namespace YContainers {
