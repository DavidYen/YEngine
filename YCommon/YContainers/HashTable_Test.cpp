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

TEST_F(AtomicHashTableTest, ConstructorTest) {
  DoConstructorTest();
}

TEST_F(HashTableTest, ConstructorTest) {
  DoConstructorTest();
}

TEST_F(AtomicHashTableTest, InsertionTest) {
  DoInsertionTest();
}

TEST_F(HashTableTest, InsertionTest) {
  DoInsertionTest();
}

TEST_F(AtomicHashTableTest, GetValueTest) {
  DoGetValueTest();
}

TEST_F(HashTableTest, GetValueTest) {
  DoGetValueTest();
}

TEST_F(AtomicHashTableTest, MultipleValuesTest) {
  DoMultipleValuesTest();
}

TEST_F(HashTableTest, MultipleValuesTest) {
  DoMultipleValuesTest();
}

TEST_F(AtomicHashTableTest, GetEmptyValueTest) {
  DoGetEmptyValueTest();
}

TEST_F(HashTableTest, GetEmptyValueTest) {
  DoGetEmptyValueTest();
}

}} // namespace YCommon { namespace YContainers {
