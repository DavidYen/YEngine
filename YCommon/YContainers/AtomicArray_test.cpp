#include <YCommon/Headers/stdincludes.h>
#include "AtomicArray.h"

#include <gtest/gtest.h>

namespace YCommon { namespace YContainers {

TEST(BasicAtomicArrayTest, ConstructorTest) {
  uint8_t array_data[sizeof(int)];
  AtomicArray atomic_array(array_data, sizeof(array_data),
                           sizeof(int), 1);

  int typed_data[1];
  TypedAtomicArray<int> typed_array(typed_data,
                                    sizeof(typed_data),
                                    ARRAY_SIZE(typed_data));

  ContainedAtomicArray<int, 1> contained_array;
}

TEST(BasicAtomicArrayTest, AllocationTest) {
  uint8_t array_data[sizeof(int)];
  AtomicArray atomic_array(array_data, sizeof(array_data),
                           sizeof(int), 1);

  EXPECT_EQ(0, atomic_array.Allocate());
  EXPECT_EQ(static_cast<uint32_t>(-1), atomic_array.Allocate());
}

TEST(BasicAtomicArrayTest, InsertionTest) {
  int array_data[1] = { 0 };
  AtomicArray atomic_array(array_data, sizeof(array_data),
                           sizeof(array_data[0]), ARRAY_SIZE(array_data));

  const int insertion_data = 123;
  const int extra_data = 234;
  EXPECT_EQ(0, atomic_array.Insert(&insertion_data));
  EXPECT_EQ(static_cast<uint32_t>(-1), atomic_array.Insert(&extra_data));
  EXPECT_EQ(array_data[0], insertion_data);
}

TEST(BasicAtomicArrayTest, RemovalTest) {
  int array_data[1] = { 0 };
  AtomicArray atomic_array(array_data, sizeof(array_data),
                           sizeof(array_data[0]), ARRAY_SIZE(array_data));

  EXPECT_EQ(0, atomic_array.Allocate());

  atomic_array.Remove(0);

  EXPECT_EQ(0, atomic_array.Allocate());
}

TEST(BasicAtomicArrayTest, MaxUsedIndexTest) {
  int array_data[2] = { 0 };
  AtomicArray atomic_array(array_data, sizeof(array_data),
                           sizeof(array_data[0]), ARRAY_SIZE(array_data));

  EXPECT_EQ(0, atomic_array.GetNumIndexesUsed());
  EXPECT_EQ(0, atomic_array.Allocate());

  EXPECT_EQ(1, atomic_array.GetNumIndexesUsed());

  atomic_array.Remove(0);

  EXPECT_EQ(1, atomic_array.GetNumIndexesUsed());
}

TEST(BasicAtomicArrayTest, TypedArrayFunctions) {
  int array_data[2] = { 0 };
  TypedAtomicArray<int> atomic_array(array_data,
                                     sizeof(array_data),
                                     ARRAY_SIZE(array_data));

  const int insertion_data1 = 123;
  const int insertion_data2 = 234;
  EXPECT_EQ(0, atomic_array.Insert(insertion_data1));
  EXPECT_EQ(1, atomic_array.Insert(&insertion_data2));

  EXPECT_EQ(insertion_data1, atomic_array[0]);
  EXPECT_EQ(insertion_data2, atomic_array[1]);
}

TEST(BasicAtomicArrayTest, ContainedArrayFunctions) {
  ContainedAtomicArray<int, 2> atomic_array;

  const int insertion_data1 = 123;
  const int insertion_data2 = 234;
  EXPECT_EQ(0, atomic_array.Insert(insertion_data1));
  EXPECT_EQ(1, atomic_array.Insert(&insertion_data2));

  EXPECT_EQ(insertion_data1, atomic_array[0]);
  EXPECT_EQ(insertion_data2, atomic_array[1]);
}

TEST(AtomicArrayReuseTest, AscendingReuse) {
  ContainedAtomicArray<int, 10> atomic_array;

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(i, atomic_array.Insert(100+i));
  }

  for (int i = 0; i < 10; ++i) {
  }
}

TEST(AtomicArrayReuseTest, DescendingReuse) {
  ContainedAtomicArray<int, 10> atomic_array;

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(i, atomic_array.Insert(100+i));
  }

  for (int i = 0; i < 10; ++i) {
  }
}

TEST(AtomicArrayReuseTest, OutOfOrderReuse) {
  ContainedAtomicArray<int, 10> atomic_array;

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(i, atomic_array.Insert(100+i));
  }

  for (int i = 0; i < 10; ++i) {
  }
}

}} // namespace YCommon { namespace YContainers {
