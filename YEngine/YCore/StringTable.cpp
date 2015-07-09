#include <YCommon/Headers/stdincludes.h>
#include "StringTable.h"

#include <YCommon/YContainers/AtomicHashTable.h>

namespace YEngine { namespace YCore {

namespace {
  YCommon::YContainers::AtomicHashTable gHashTable;
  size_t max_string_size = 0;
}

size_t GetAllocationSize(size_t max_string_size, size_t table_size) {
  return YCommon::YContainers::AtomicHashTable::GetAllocationSize(
      table_size, max_string_size);
}

void StringTable::Initialize(size_t max_string_size, size_t table_size,
                             void* buffer, size_t buffer_size) {
  gHashTable.Init(buffer, buffer_size, table_size, max_string_size);
  max_string_size = max_string_size;
}

void StringTable::Terminate() {
  gHashTable.Reset();
  max_string_size = 0;
}

uint64_t StringTable::AddString(const char* string, size_t string_size) {
  YDEBUG_CHECK(string_size <= max_string_size,
               "Cannot exceed maximum string size: %u > %u",
               static_cast<uint32_t>(string_size),
               static_cast<uint32_t>(max_string_size));
  uint64_t hash_key = 0;
  gHashTable.Insert(string, string_size, string, string_size, &hash_key);
  return hash_key;
}

const char* StringTable::StringLookup(uint64_t string_hash) {
  const void* value = gHashTable.GetValue(string_hash);
  YDEBUG_CHECK(value != NULL, "Cannot lookup string which does not exist!");
  return static_cast<const char*>(value);
}

}} // namespace YEngine { namespace YCore {
