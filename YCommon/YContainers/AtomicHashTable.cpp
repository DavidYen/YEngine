#include <YCommon/Headers/stdincludes.h>
#include "AtomicHashTable.h"

#include <string>

#include <YCommon/Headers/Atomics.h>
#include <YCommon/YUtils/Hash.h>

#define EMPTY_VALUE static_cast<uint64_t>(0)
#define REMOVED_VALUE static_cast<uint64_t>(-1)
#define PLACEHOLDER_VALUE static_cast<uint64_t>(-2)
#define MAX_TRIES 10

namespace YCommon { namespace YContainers {

AtomicHashTable::AtomicHashTable()
    : mBuffer(NULL),
      mCurrentEntries(0),
      mNumEntries(0),
      mMaxValueSize(0) {
}

AtomicHashTable::AtomicHashTable(void* buffer, size_t buffer_size,
                                 size_t num_entries, size_t max_value_size)
    : mBuffer(NULL),
      mCurrentEntries(0),
      mNumEntries(0),
      mMaxValueSize(0) {
  Init(buffer, buffer_size, num_entries, max_value_size);
}

AtomicHashTable::~AtomicHashTable() {
}

void AtomicHashTable::Init(void* buffer, size_t buffer_size,
                           size_t num_entries, size_t max_value_size) {
  YASSERT(reinterpret_cast<uintptr_t>(buffer) % sizeof(uint64_t) == 0,
          "Atomic Hash Table memory must be aligned to %u bytes - supplied %p.",
          static_cast<uint32_t>(sizeof(uint64_t)),
          buffer);
  YASSERT(num_entries >= MAX_TRIES,
          "Atomic Hash Table entry count must be at least %u - supplied %u.",
          static_cast<uint32_t>(MAX_TRIES),
          static_cast<uint32_t>(num_entries));

  const size_t entry_size = sizeof(uint64_t) + max_value_size;
  const size_t total_table_size = entry_size * num_entries;

  YASSERT(total_table_size <= buffer_size,
          "Atomic Hash Table requires %u bytes, supplied %u bytes.",
          static_cast<uint32_t>(total_table_size),
          static_cast<uint32_t>(buffer_size));

  mBuffer = buffer;
  mCurrentEntries = 0;
  mNumEntries = num_entries;
  mMaxValueSize = max_value_size;

  Clear();
}

void AtomicHashTable::Reset() {
  mBuffer = NULL;
  mCurrentEntries = 0;
  mNumEntries = 0;
  mMaxValueSize = 0;
}

void AtomicHashTable::Clear() {
  mCurrentEntries = 0;
  memset(mBuffer, EMPTY_VALUE, sizeof(uint64_t) * mNumEntries);
}

uint64_t AtomicHashTable::Insert(const void* key, size_t key_size,
                                 const void* value, size_t value_size) {
  const uint64_t hash_key = YCommon::YUtils::Hash::Hash64(key, key_size);
  Insert(hash_key, value, value_size);
  return hash_key;
}

void AtomicHashTable::Insert(uint64_t hash_key,
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

    if (hash_table_value == hash_key ||
        (hash_table_value == EMPTY_VALUE &&
         YCommon::AtomicCmpSet64(&hash_table[try_index],
                                 EMPTY_VALUE,
                                 PLACEHOLDER_VALUE)) ||
        (hash_table_value == REMOVED_VALUE &&
         YCommon::AtomicCmpSet64(&hash_table[try_index],
                                 REMOVED_VALUE,
                                 PLACEHOLDER_VALUE))) {
      memcpy(hash_value_table + (try_index * mMaxValueSize), value, value_size);
      YCommon::AtomicAdd32(&mCurrentEntries, 1); // Has implicit memory barrier.
      hash_table[try_index] = hash_key;
      return;
    }
  }

  YFATAL("Atomic Hash Table is too full, maximum amount of tries reached!");
}

bool AtomicHashTable::Remove(const void* key, size_t key_size) {
  const uint64_t hash_key = YCommon::YUtils::Hash::Hash64(key, key_size);
  return Remove(hash_key);
}

bool AtomicHashTable::Remove(uint64_t hash_key) {
  volatile uint64_t* hash_table = static_cast<volatile uint64_t*>(mBuffer);
  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key &&
        YCommon::AtomicCmpSet64(&hash_table[try_index],
                                hash_key,
                                REMOVED_VALUE)) {
      YCommon::AtomicAdd32(&mCurrentEntries, -1);
      return true;
    } else if (hash_table_value == EMPTY_VALUE) {
      return false;
    }
  }

  return false;
}

const void* const AtomicHashTable::GetValue(const void* key,
                                            size_t key_size) const {
  return GetValue(YCommon::YUtils::Hash::Hash64(key, key_size));
}

const void* const AtomicHashTable::GetValue(uint64_t hash_key) const {
  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  volatile uint64_t* hash_table = static_cast<volatile uint64_t*>(mBuffer);
  const uint8_t* hash_value_table =
      static_cast<const uint8_t*>(mBuffer) + key_table_size;

  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key) {
      YCommon::MemoryBarrier(); // Make sure other thread is finished writing
      return hash_value_table + (try_index * mMaxValueSize);
    } else if (hash_table_value == EMPTY_VALUE) {
      return NULL;
    }
  }

  return NULL;
}

void* AtomicHashTable::GetValue(const void* key, size_t key_size) {
  return GetValue(YCommon::YUtils::Hash::Hash64(key, key_size));
}

void* AtomicHashTable::GetValue(uint64_t hash_key) {
  const size_t key_table_size = sizeof(uint64_t) * mNumEntries;
  volatile uint64_t* hash_table = static_cast<volatile uint64_t*>(mBuffer);
  uint8_t* hash_value_table = static_cast<uint8_t*>(mBuffer) + key_table_size;

  for (uint64_t i = 0; i < MAX_TRIES; ++i) {
    const uint64_t try_index = (hash_key + i) % mNumEntries;
    const uint64_t hash_table_value = hash_table[try_index];

    if (hash_table_value == hash_key) {
      YCommon::MemoryBarrier(); // Make sure other thread is finished writing
      return hash_value_table + (try_index * mMaxValueSize);
    } else if (hash_table_value == EMPTY_VALUE) {
      return NULL;
    }
  }

  return NULL;
}

}} // namespace YCommon { namespace YContainers {
