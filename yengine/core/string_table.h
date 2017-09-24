#ifndef YENGINE_CORE_STRING_TABLE_H
#define YENGINE_CORE_STRING_TABLE_H

/***********
* The String Table is a thread-safe shared global string table which contains
* all the strings in the program.
************/

#include <stdint.h>

namespace yengine { namespace core {

namespace StringTable {
  size_t GetAllocationSize(size_t max_string_size, size_t table_size);
  void Initialize(size_t max_string_size, size_t table_size,
                  void* buffer, size_t buffer_size);
  void Terminate();

  uint64_t AddString(const char* string, size_t string_size);
  const char* StringLookup(uint64_t string_hash);
}

}} // namespace yengine { namespace core {

#endif // YENGINE_CORE_STRING_TABLE_H
