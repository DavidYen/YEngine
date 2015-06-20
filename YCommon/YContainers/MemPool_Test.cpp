#include <YCommon/Headers/stdincludes.h>
#include "AtomicMemPool.h"
#include "MemPool.h"

#include <gtest/gtest.h>

namespace YCommon { namespace YContainers {

template <class BaseMP, class TypedMP, class ContainedMP>
class BaseMemPoolTest : public ::testing::Test {
 public:
  BaseMemPoolTest() {}

  void DoConstructorTest() {
    uint8_t array_data[sizeof(int)];
    BaseMP mem_pool(array_data, sizeof(array_data),
                    sizeof(int), 1);

    int typed_data[1];
    TypedMP typed_array(typed_data,
                        sizeof(typed_data),
                        ARRAY_SIZE(typed_data));

    ContainedMP contained_array;
  }

  void DoAllocationTest() {
    uint8_t array_data[sizeof(int)];
    BaseMP mem_pool(array_data, sizeof(array_data),
                    sizeof(int), 1);

    EXPECT_EQ(0, mem_pool.Allocate());
    EXPECT_EQ(static_cast<uint32_t>(-1), mem_pool.Allocate());
  }

  void DoInsertionTest() {
    int array_data[1] = { 0 };
    BaseMP mem_pool(array_data, sizeof(array_data),
                    sizeof(array_data[0]), ARRAY_SIZE(array_data));

    const int insertion_data = 123;
    const int extra_data = 234;
    EXPECT_EQ(0, mem_pool.Insert(&insertion_data));
    EXPECT_EQ(static_cast<uint32_t>(-1), mem_pool.Insert(&extra_data));
    EXPECT_EQ(array_data[0], insertion_data);
  }

  void DoRemovalTest() {
    int array_data[1] = { 0 };
    BaseMP mem_pool(array_data, sizeof(array_data),
                    sizeof(array_data[0]), ARRAY_SIZE(array_data));

    EXPECT_EQ(0, mem_pool.Allocate());

    mem_pool.Remove(0);

    EXPECT_EQ(0, mem_pool.Allocate());
  }

  void DoMaxUsedIndexTest() {
    int array_data[2] = { 0 };
    BaseMP mem_pool(array_data, sizeof(array_data),
                    sizeof(array_data[0]), ARRAY_SIZE(array_data));

    EXPECT_EQ(0, mem_pool.GetNumIndexesUsed());
    EXPECT_EQ(0, mem_pool.Allocate());

    EXPECT_EQ(1, mem_pool.GetNumIndexesUsed());

    mem_pool.Remove(0);

    EXPECT_EQ(1, mem_pool.GetNumIndexesUsed());
  }

  void DoTypedArrayFunctions() {
    int array_data[2] = { 0 };
    TypedMP mem_pool(array_data,
                     sizeof(array_data),
                     ARRAY_SIZE(array_data));

    const int insertion_data1 = 123;
    const int insertion_data2 = 234;
    EXPECT_EQ(0, mem_pool.Insert(insertion_data1));
    EXPECT_EQ(1, mem_pool.Insert(&insertion_data2));

    EXPECT_EQ(insertion_data1, mem_pool[0]);
    EXPECT_EQ(insertion_data2, mem_pool[1]);
  }

  void DoContainedArrayFunctions() {
    ContainedMP mem_pool;

    const int insertion_data1 = 123;
    const int insertion_data2 = 234;
    EXPECT_EQ(0, mem_pool.Insert(insertion_data1));
    EXPECT_EQ(1, mem_pool.Insert(&insertion_data2));

    EXPECT_EQ(insertion_data1, mem_pool[0]);
    EXPECT_EQ(insertion_data2, mem_pool[1]);
  }

  void DoAscendingReuse() {
    ContainedMP mem_pool;

    // Initialize
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(i, mem_pool.Insert(100+i));
    }

    // Remove Ascending
    for (int i = 0; i < 10; ++i) {
      mem_pool.Remove(i);
    }

    // Reinsert
    for (int i = 0; i < 10; ++i) {
      mem_pool.Insert(100+i);
    }

    // Validate
    int validation[10] = { 0 };
    for (int i = 0; i < 10; ++i) {
      int value = mem_pool[i] - 100;
      ASSERT_GE(value, 0);
      ASSERT_LT(value, 10);
      EXPECT_EQ(validation[value], 0);

      validation[value] = 1;
    }
  }

  void DoDescendingReuse() {
    ContainedMP mem_pool;

    // Initialize
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(i, mem_pool.Insert(100+i));
    }

    // Remove Descending
    for (int i = 9; i >= 0; --i) {
      mem_pool.Remove(i);
    }

    // Reinsert
    for (int i = 0; i < 10; ++i) {
      mem_pool.Insert(100+i);
    }

    // Validate
    int validation[10] = { 0 };
    for (int i = 0; i < 10; ++i) {
      int value = mem_pool[i] - 100;
      ASSERT_GE(value, 0);
      ASSERT_LT(value, 10);
      EXPECT_EQ(validation[value], 0);

      validation[value] = 1;
    }
  }

  void DoOutOfOrderReuse() {
    ContainedMP mem_pool;

    // Initialize
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(i, mem_pool.Insert(100+i));
    }

    // Remove Out of Order
    for (int i = 0; i < 5; ++i) {
      mem_pool.Remove(i * 2 + 1);
      mem_pool.Remove(i * 2);
    }

    // Reinsert
    for (int i = 0; i < 10; ++i) {
      mem_pool.Insert(100+i);
    }

    // Validate
    int validation[10] = { 0 };
    for (int i = 0; i < 10; ++i) {
      int value = mem_pool[i] - 100;
      ASSERT_GE(value, 0);
      ASSERT_LT(value, 10);
      EXPECT_EQ(validation[value], 0);

      validation[value] = 1;
    }
  }
};

class AtomicMemPoolTest : public BaseMemPoolTest<
  AtomicMemPool, TypedAtomicMemPool<int>, ContainedAtomicMemPool<int, 10> > {};

class MemPoolTest : public BaseMemPoolTest<
  MemPool, TypedMemPool<int>, ContainedMemPool<int, 10> > {};

#define MEMPOOL_TEST(NAME) \
  TEST_F(AtomicMemPoolTest, NAME) { Do ## NAME(); } \
  TEST_F(MemPoolTest, NAME) { Do ## NAME(); }

MEMPOOL_TEST(ConstructorTest)
MEMPOOL_TEST(AllocationTest)
MEMPOOL_TEST(InsertionTest)
MEMPOOL_TEST(RemovalTest)
MEMPOOL_TEST(MaxUsedIndexTest)
MEMPOOL_TEST(TypedArrayFunctions)
MEMPOOL_TEST(ContainedArrayFunctions)
MEMPOOL_TEST(AscendingReuse)
MEMPOOL_TEST(DescendingReuse)
MEMPOOL_TEST(OutOfOrderReuse)

}} // namespace YCommon { namespace YContainers {
