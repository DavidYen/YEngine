#include <YCommon/Headers/stdincludes.h>
#include "HashTable.h"

#include <string>

#include <YCommon/YUtils/Hash.h>

#define EMPTY_VALUE static_cast<uint64_t>(0)
#define MAX_TRIES 10

namespace YCommon { namespace YContainers {

HashTable::HashTable()
    : mBuffer(NULL),
      mNumEntries(0),
      mMaxValueSize(0) {
}

HashTable::HashTable(void* buffer, size_t buffer_size,
                     size_t num_entries, size_t max_value_size)
    : mBuffer(NULL),
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
  mNumEntries = num_entries;
  mMaxValueSize = max_value_size;

  memset(buffer, EMPTY_VALUE, sizeof(uint64_t) * num_entries);
}

void HashTable::Reset() {
  mBuffer = NULL;
  mNumEntries = 0;
  mMaxValueSize = 0;
}

uint64_t HashTable::Insert(const void* key, size_t key_size,
                           const void* value, size_t value_size) {
  const uint64_t hash_key = YCommon::YUtils::Hash::Hash64(key, key_size);
  Insert(hash_key, value, value_size);
  return hash_key;
}

void HashTable::Insert(uint64_t hash_key,
                       const void* value, size_t value_size) {
  YDEBUG_CHECK(value_size <= mMaxValueSize,
               "Invalid Hash Value size: Maximum size is %u. Supplied %u.",
               static_cast<uint32_t>(mMaxValueSize),
               static_cast<uint32_t>(value_size));

  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  volatile uint64_t* hash_table = static_cast<volatile uint64_t*>(mBuffer);
  uint8_t* hash_value_table = static_cast<uint8_t*>(mBuffer) + key_table_size;

  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key || hash_table_value == EMPTY_VALUE) {
      hash_table[try_index] = hash_key;
      memcpy(hash_value_table + (try_index * mMaxValueSize), value, value_size);
      return;
    }
  }

  YFATAL("Atomic Hash Table is too full, maximum amount of tries reached!");
}

const void* const HashTable::GetValue(const void* key,
                                      size_t key_size) const {
  return GetValue(YCommon::YUtils::Hash::Hash64(key, key_size));
}

const void* const HashTable::GetValue(uint64_t hash_key) const {
  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  volatile uint64_t* hash_table = static_cast<volatile uint64_t*>(mBuffer);
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
  volatile uint64_t* hash_table = static_cast<volatile uint64_t*>(mBuffer);
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
