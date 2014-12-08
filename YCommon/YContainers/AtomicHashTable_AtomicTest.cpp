#include <YCommon/Headers/stdincludes.h>
#include "AtomicHashTable.h"

#include <gtest/gtest.h>

#include <YCommon/Headers/Atomics.h>
#include <YCommon/YPlatform/Thread.h>

#define NUM_THREADS 10

namespace YCommon { namespace YContainers {

struct InsertionArg {
  AtomicHashTable* hash_table;
  volatile uint32_t* begin_running;
  uint32_t insertion_begin;
  uint32_t num_insertions;

  void Init(AtomicHashTable* table, volatile uint32_t* begin_running_ptr,
            uint32_t begin, uint32_t count) {
    hash_table = table;
    begin_running = begin_running_ptr;
    insertion_begin = begin;
    num_insertions = count;
  }
};

uintptr_t InsertionRoutine(void* arg) {
  InsertionArg* arg_data = static_cast<InsertionArg*>(arg);

  while (*arg_data->begin_running == 0);

  MemoryBarrier();

  AtomicHashTable* hash_table = arg_data->hash_table;
  const uint32_t begin = arg_data->insertion_begin;
  const uint32_t count = arg_data->num_insertions;

  for (uint32_t i = 0; i < count; ++i) {
    uint32_t value = begin + i;
    hash_table->Insert(&value, sizeof(value), &value, sizeof(value));
  }

  return 0;
}

/************
* Test Definitions
*************/
TEST(AtomicHashTableTest, InsertionTest) {
  ContainedAtomicHashTable<uint32_t, 20> hash_table;
  volatile uint32_t begin_running = 1;
  InsertionArg arg_data;
  arg_data.Init(&hash_table, &begin_running, 0, 10);

  ASSERT_EQ(0, InsertionRoutine(&arg_data));

  // Make sure everything was inserted correctly.
  for (uint32_t i = 0; i < 10; ++i) {
    EXPECT_EQ(i, *hash_table.GetValue(&i, sizeof(i)));
  }
}

/*************
* Test simultaneous insertions
**************/
template <size_t HASH_TABLE_SIZE, uint32_t TEST_SIZE>
class ThreadedInsertionTest : public ::testing::Test {
 public:
  void RunTest() {
    ASSERT_EQ(TEST_SIZE % NUM_THREADS, 0);
    const uint32_t count_per_thread = TEST_SIZE / NUM_THREADS;

    mHashTable.Reset();
    mBeginRunning = 0;

    for (uint32_t i = 0; i < NUM_THREADS; ++i) {
      mArgDatas[i].Init(&mHashTable, &mBeginRunning,
                        count_per_thread * i, count_per_thread);
      ASSERT_TRUE(mTestThreads[i].Initialize(InsertionRoutine, &mArgDatas[i]));
      mTestThreads[i].Run();
    }

    mBeginRunning = 1;
    for (uint32_t i = 0; i < NUM_THREADS; ++i) {
      mTestThreads[i].Join();
      EXPECT_EQ(0, mTestThreads[i].ReturnValue());
    }

    // Make sure hash table is in tact
    for (uint32_t i = 0; i < TEST_SIZE; ++i) {
      uint32_t* test_value = mHashTable.GetValue(i);
      ASSERT_NE(test_value, static_cast<uint32_t*>(NULL));
      EXPECT_EQ(i, *test_value);
    }
  }

  ContainedFullAtomicHashTable<uint32_t, uint32_t, HASH_TABLE_SIZE> mHashTable;
  volatile uint32_t mBeginRunning;
  InsertionArg mArgDatas[NUM_THREADS];
  YCommon::YPlatform::Thread mTestThreads[NUM_THREADS];
};

class MinimalAtomicHashInsertTest : public ThreadedInsertionTest<50, 10> {};
class SmallAtomicHashInsertTest : public ThreadedInsertionTest<1000, 200> {};
class LargeAtomicHashInsertTest : public ThreadedInsertionTest<50000, 10000> {};

TEST_F(MinimalAtomicHashInsertTest , MinimalTests) {
  for (int i = 0; i < 100; ++i) {
    RunTest();
  }
}

TEST_F(SmallAtomicHashInsertTest, SmallTests) {
  for (int i = 0; i < 1000; ++i) {
    RunTest();
  }
}

TEST_F(LargeAtomicHashInsertTest, LargeTests) {
  for (int i = 0; i < 5000; ++i) {
    RunTest();
  }
}

}} // namespace YCommon { namespace YContainers {
