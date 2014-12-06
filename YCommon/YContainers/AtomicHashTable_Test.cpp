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
  ContainedAtomicHashTable<int, 10> hash_table;

  const char hash_key[] = "hash key";
  int hash_value = 123;
  hash_table.Insert(hash_key, sizeof(hash_key), hash_value);
}

TEST(BasicAtomicHashTableTest, GetValueTest) {
  ContainedAtomicHashTable<int, 10> hash_table;

  const char hash_key[] = "hash key";
  int hash_value = 123;
  hash_table.Insert(hash_key, sizeof(hash_key), hash_value);
  EXPECT_EQ(hash_value, *hash_table.GetValue(hash_key, sizeof(hash_key)));
}

TEST(BasicAtomicHashTableTest, MultipleValuesTest) {
  ContainedFullAtomicHashTable<int, int, 100> hash_table;

  for (int i = 0; i < 50; ++i) {
    hash_table.Insert(i, i);
  }

  for (int i = 0; i < 50; ++i) {
    EXPECT_EQ(i, *hash_table.GetValue(i));
  }
}

TEST(BasicAtomicHashTableTest, GetEmptyValueTest) {
  ContainedAtomicHashTable<int, 10> hash_table;

  const char hash_key[] = "hash key";
  EXPECT_EQ(NULL, hash_table.GetValue(hash_key, sizeof(hash_key)));
}

}} // namespace YCommon { namespace YContainers {
