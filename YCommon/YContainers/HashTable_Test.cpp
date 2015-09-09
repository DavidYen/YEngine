#include <YCommon/Headers/stdincludes.h>
#include "AtomicHashTable.h"
#include "HashTable.h"

#include <gtest/gtest.h>

namespace YCommon { namespace YContainers {

template <class BaseHT, class TypedHT,
          class FullTypedHT, class ContainedHT,
          class ContainedFullHT>
class BaseHashTableTest : public ::testing::Test {
 public:
  BaseHashTableTest() {}

  void DoConstructorTest() {
    uint8_t array_data[(sizeof(int) + sizeof(uint64_t)) * 10];
    BaseHT hash_table(array_data, sizeof(array_data), 10, sizeof(int));
    TypedHT typed_table(array_data, sizeof(array_data), 10);
    FullTypedHT full_typed_table(array_data, sizeof(array_data), 10);
    ContainedHT contained_hash_table;
    ContainedFullHT contained_full_hash_table;
  }

  void DoInsertionTest() {
    ContainedHT hash_table;

    const char hash_key[] = "hash key";
    int hash_value = 123;
    hash_table.Insert(hash_key, sizeof(hash_key), hash_value);
  }

  void DoRemovalTest() {
    ContainedHT hash_table;

    const char hash_key[] = "hash key";
    int hash_value = 123;
    hash_table.Insert(hash_key, sizeof(hash_key), hash_value);

    EXPECT_EQ(1, hash_table.GetCurrentSize());
    EXPECT_TRUE(hash_table.Remove(hash_key, sizeof(hash_key)));
    EXPECT_EQ(0, hash_table.GetCurrentSize());
    EXPECT_FALSE(hash_table.Remove(hash_key, sizeof(hash_key)));
  }

  void DoRemoveValueTest() {
    ContainedHT hash_table;

    const char hash_key[] = "hash key";
    int hash_value = 123;
    void* value = hash_table.Insert(hash_key, sizeof(hash_key), hash_value);

    EXPECT_EQ(1, hash_table.GetCurrentSize());
    EXPECT_TRUE(hash_table.Remove(value));
    EXPECT_EQ(0, hash_table.GetCurrentSize());
    EXPECT_FALSE(hash_table.Remove(value));
  }

  void DoGetValueTest() {
    ContainedHT hash_table;

    const char hash_key[] = "hash key";
    int hash_value = 123;
    hash_table.Insert(hash_key, sizeof(hash_key), hash_value);
    EXPECT_EQ(hash_value, *hash_table.GetValue(hash_key, sizeof(hash_key)));
  }

  void DoMultipleValuesTest() {
    ContainedFullHT hash_table;

    for (int i = 0; i < 50; ++i) {
      hash_table.Insert(i, i);
    }

    for (int i = 0; i < 50; ++i) {
      EXPECT_EQ(i, *hash_table.GetValue(i));
    }
  }

  void DoGetEmptyValueTest() {
    ContainedHT hash_table;

    const char hash_key[] = "hash key";
    EXPECT_EQ(NULL, hash_table.GetValue(hash_key, sizeof(hash_key)));
  }

  void DoAllocationSizeTest() {
    const size_t num_entries = 32;
    const size_t max_value_size = 64;

    const size_t size = BaseHT::GetAllocationSize(num_entries, max_value_size);
    std::vector<uint8_t> allocation(size);

    BaseHT hash_table;
    hash_table.Init(allocation.data(), allocation.size(),
                    num_entries, max_value_size);
  }
};

class AtomicHashTableTest : public BaseHashTableTest<
  AtomicHashTable, TypedAtomicHashTable<int>,
  FullTypedAtomicHashTable<int, int>,
  ContainedAtomicHashTable<int, 100>,
  ContainedFullAtomicHashTable<int, int, 100> > {};

class HashTableTest : public BaseHashTableTest<
  HashTable, TypedHashTable<int>,
  FullTypedHashTable<int, int>,
  ContainedHashTable<int, 100>,
  ContainedFullHashTable<int, int, 100> > {};

#define HASH_TABLE_TEST(NAME) \
  TEST_F(AtomicHashTableTest, NAME) { Do ## NAME(); } \
  TEST_F(HashTableTest, NAME) { Do ## NAME(); }

HASH_TABLE_TEST(ConstructorTest);
HASH_TABLE_TEST(InsertionTest);
HASH_TABLE_TEST(RemovalTest);
HASH_TABLE_TEST(RemoveValueTest);
HASH_TABLE_TEST(GetValueTest);
HASH_TABLE_TEST(MultipleValuesTest);
HASH_TABLE_TEST(GetEmptyValueTest);
HASH_TABLE_TEST(AllocationSizeTest);

}} // namespace YCommon { namespace YContainers {
