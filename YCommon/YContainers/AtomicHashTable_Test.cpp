#include <YCommon/Headers/stdincludes.h>
#include "AtomicHashTable.h"

#include <gtest/gtest.h>

namespace YCommon { namespace YContainers {

TEST(BasicAtomicHashTableTest, ConstructorTest) {
  uint8_t array_data[(sizeof(int) + sizeof(uint64_t)) * 10];
  AtomicHashTable hash_table(array_data, sizeof(array_data), 10, sizeof(int));
  TypedAtomicHashTable<int> typed_table(array_data, sizeof(array_data), 10);
  FullTypedAtomicHashTable<int, int> full_typed_table(array_data,
                                                      sizeof(array_data), 10);
  ContainedAtomicHashTable<int, 10> contained_hash_table;
  ContainedFullAtomicHashTable<int, int, 10> contained_full_hash_table;
}

TEST(BasicAtomicHashTableTest, InsertionTest) {
  ContainedAtomicHashTable<int, 10> contained_hash_table;

  const char hash_key[] = "hash key";
  int hash_value = 123;
  contained_hash_table.Insert(hash_key, sizeof(hash_key), hash_value);
}


}} // namespace YCommon { namespace YContainers {
