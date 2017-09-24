#include "unordered_array.h"

#include <gtest/gtest.h>

#include "ycommon/headers/macros.h"

namespace ycommon { namespace containers {

template <class BaseArray, class TypedArray, class ContainedArray>
class BaseUnorderedArrayTest : public ::testing::Test {
 public:
  BaseUnorderedArrayTest() {}

  void DoConstructorTest() {
    uint8_t array_data[sizeof(int)];
    BaseArray array(array_data, sizeof(array_data),
                    sizeof(int), 1);

    int typed_data[1];
    TypedArray typed_array(typed_data,
                        sizeof(typed_data),
                        ARRAY_SIZE(typed_data));

    ContainedArray contained_array;
  }

  void DoAllocationTest() {
    uint8_t array_data[sizeof(int)];
    BaseArray array(array_data, sizeof(array_data),
                    sizeof(int), 1);

    EXPECT_EQ(0, array.Allocate());
    EXPECT_EQ(static_cast<uint32_t>(-1), array.Allocate());
  }

  void DoRemovalTest() {
    int array_data[1] = { 0 };
    BaseArray array(array_data, sizeof(array_data),
                    sizeof(array_data[0]), ARRAY_SIZE(array_data));

    EXPECT_EQ(0, array.Allocate());

    array.Remove(0);

    EXPECT_EQ(0, array.Allocate());
  }

  void DoGetCountTest() {
    int array_data[2] = { 0 };
    BaseArray array(array_data, sizeof(array_data),
                    sizeof(array_data[0]), ARRAY_SIZE(array_data));

    EXPECT_EQ(0, array.GetCount());
    EXPECT_EQ(0, array.Allocate());

    EXPECT_EQ(1, array.GetCount());

    array.Remove(0);
    EXPECT_EQ(0, array.GetCount());
  }

  struct callback_expectations {
    uint32_t old_index;
    uint32_t new_index;
  };
  static void callback_func(uint32_t old_index, uint32_t new_index, void* arg) {
    ASSERT_TRUE(arg != nullptr);
    callback_expectations* expects = static_cast<callback_expectations*>(arg);
    EXPECT_EQ(expects->old_index, old_index);
    EXPECT_EQ(expects->new_index, new_index);
  }

  void DoCallbackTest() {
    int array_data[2] = { 0 };
    BaseArray array(array_data, sizeof(array_data),
                    sizeof(array_data[0]), ARRAY_SIZE(array_data));

    EXPECT_EQ(0, array.Allocate());
    EXPECT_EQ(1, array.Allocate());

    callback_expectations no_expectations = { 123, 456 };
    array.SetItemSwappedCallBack(callback_func, &no_expectations);
    array.Remove(1);

    EXPECT_EQ(1, array.Allocate());

    callback_expectations expectations = { 1, 0 };
    array.SetItemSwappedCallBack(callback_func, &expectations);
    array.Remove(0);
  }

  void DoTypedArrayFunctions() {
    int array_data[2] = { 0 };
    TypedArray array(array_data,
                     sizeof(array_data),
                     ARRAY_SIZE(array_data));

    const int insertion_data1 = 123;
    const int insertion_data2 = 234;
    EXPECT_EQ(0, array.PushBack(insertion_data1));
    EXPECT_EQ(1, array.PushBack(&insertion_data2));

    EXPECT_EQ(insertion_data1, array[0]);
    EXPECT_EQ(insertion_data2, array[1]);
  }

  void DoContainedArrayFunctions() {
    ContainedArray array;

    const int insertion_data1 = 123;
    const int insertion_data2 = 234;
    EXPECT_EQ(0, array.PushBack(insertion_data1));
    EXPECT_EQ(1, array.PushBack(&insertion_data2));

    EXPECT_EQ(insertion_data1, array[0]);
    EXPECT_EQ(insertion_data2, array[1]);
  }

  void DoReuse() {
    ContainedArray array;

    // Initialize
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(i, array.PushBack(100+i));
    }

    // Remove Ascending
    for (int i = 0; i < 10; ++i) {
      array.Remove(0);
    }

    // Reinsert
    for (int i = 0; i < 10; ++i) {
      array.PushBack(100+i);
    }

    // Validate
    int validation[10] = { 0 };
    for (int i = 0; i < 10; ++i) {
      int value = array[i] - 100;
      ASSERT_GE(value, 0);
      ASSERT_LT(value, 10);
      EXPECT_EQ(validation[value], 0);

      validation[value] = 1;
    }
  }
};

class UnorderedArrayTest : public BaseUnorderedArrayTest<
  UnorderedArray,
  TypedUnorderedArray<int>,
  ContainedUnorderedArray<int, 10> > {};

#define MEMPOOL_TEST(NAME) \
  TEST_F(UnorderedArrayTest, NAME) { Do ## NAME(); }

MEMPOOL_TEST(ConstructorTest)
MEMPOOL_TEST(AllocationTest)
MEMPOOL_TEST(RemovalTest)
MEMPOOL_TEST(GetCountTest)
MEMPOOL_TEST(CallbackTest)
MEMPOOL_TEST(TypedArrayFunctions)
MEMPOOL_TEST(ContainedArrayFunctions)
MEMPOOL_TEST(Reuse)

}} // namespace ycommon { namespace containers {
