#include <YCommon/Headers/stdincludes.h>
#include "StringTable.h"

#include <YCommon/YContainers/AtomicHashTable.h>

namespace YEngine { namespace YCore {

namespace {
  YCommon::YContainers::AtomicHashTable gHashTable;
  size_t max_string_size = 0;
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

uint64_t AddString(const char* string, size_t string_size) {
  YDEBUG_CHECK(string_size <= max_string_size,
               "Cannot exceed maximum string size: %u > %u",
               static_cast<uint32_t>(string_size),
               static_cast<uint32_t>(max_string_size));
  return gHashTable.Insert(string, string_size, string, string_size);
}

const char* StringLookup(uint64_t string_hash) {
  const void* value = gHashTable.GetValue(string_hash);
  YDEBUG_CHECK(value != NULL, "Cannot lookup string which does not exist!");
  return static_cast<const char*>(value);
}

}} // namespace YEngine { namespace YCore {
