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
  uint32_t begin_num;
  uint32_t num_insertions;

  void Init(AtomicHashTable* table, volatile uint32_t* begin_running_ptr,
            uint32_t begin, uint32_t count) {
    hash_table = table;
    begin_running = begin_running_ptr;
    begin_num = begin;
    num_insertions = count;
  }
};

uintptr_t InsertionRoutine(void* arg) {
  InsertionArg* arg_data = static_cast<InsertionArg*>(arg);

  while (*arg_data->begin_running == 0);

  MemoryBarrier();

  AtomicHashTable* hash_table = arg_data->hash_table;
  const uint32_t begin = arg_data->begin_num;
  const uint32_t count = arg_data->num_insertions;

  for (uint32_t i = 0; i < count; ++i) {
    uint32_t value = begin + i;
    hash_table->Insert(&value, sizeof(value), &value, sizeof(value));
  }

  return 0;
}

uintptr_t RemovalRoutine(void* arg) {
  InsertionArg* arg_data = static_cast<InsertionArg*>(arg);

  while (*arg_data->begin_running == 0);

  MemoryBarrier();

  AtomicHashTable* hash_table = arg_data->hash_table;
  const uint32_t begin = arg_data->begin_num;
  const uint32_t count = arg_data->num_insertions;

  for (uint32_t i = 0; i < count; ++i) {
    uint32_t value = begin + i;
    if (!hash_table->Remove(&value, sizeof(value)))
      return 1;
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

  ASSERT_EQ(0, RemovalRoutine(&arg_data));

  // Make sure everything is removed correctly.
  for (uint32_t i = 0; i < 10; ++i) {
    EXPECT_EQ(nullptr, hash_table.GetValue(&i, sizeof(i)));
  }
}

/*************
* Test simultaneous insertions
**************/
template <size_t HASH_TABLE_SIZE, uint32_t TEST_SIZE>
class ThreadedHashTableTest : public ::testing::Test {
 public:
  void RunInsertionTest() {
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
    ASSERT_EQ(TEST_SIZE, mHashTable.GetCurrentSize());
    for (uint32_t i = 0; i < TEST_SIZE; ++i) {
      uint32_t* test_value = mHashTable.GetValue(i);
      ASSERT_NE(nullptr, test_value);
      EXPECT_EQ(i, *test_value);
    }
  }

  void RunRemovalTest() {
    ASSERT_EQ(TEST_SIZE % NUM_THREADS, 0);
    const uint32_t count_per_thread = TEST_SIZE / NUM_THREADS;

    mHashTable.Reset();
    mBeginRunning = 0;

    for (uint32_t i = 0; i < NUM_THREADS; ++i) {
      mArgDatas[i].Init(&mHashTable, &mBeginRunning,
                        count_per_thread * i, count_per_thread);
      ASSERT_TRUE(mTestThreads[i].Initialize(RemovalRoutine, &mArgDatas[i]));
      mTestThreads[i].Run();
    }

    // Insert the items first
    ASSERT_EQ(0, mHashTable.GetCurrentSize());
    for (uint32_t i = 0; i < TEST_SIZE; ++i) {
      mHashTable.Insert(i, i);
    }
    ASSERT_EQ(TEST_SIZE, mHashTable.GetCurrentSize());

    mBeginRunning = 1;
    for (uint32_t i = 0; i < NUM_THREADS; ++i) {
      mTestThreads[i].Join();
      EXPECT_EQ(0, mTestThreads[i].ReturnValue());
    }

    ASSERT_EQ(0, mHashTable.GetCurrentSize());
    for (uint32_t i = 0; i < TEST_SIZE; ++i) {
      uint32_t* test_value = mHashTable.GetValue(i);
      EXPECT_EQ(nullptr, test_value);
    }
  }

  void RunInsertRemovalTest() {
    ASSERT_EQ(0, TEST_SIZE % NUM_THREADS);
    ASSERT_EQ(0, NUM_THREADS % 2) << "NUM_THREADS must be event.";
    const uint32_t count_per_thread = TEST_SIZE / NUM_THREADS;

    mHashTable.Reset();
    mBeginRunning = 0;

    for (uint32_t i = 0; i < NUM_THREADS; i += 2) {
      mArgDatas[i].Init(&mHashTable, &mBeginRunning,
                        count_per_thread * i, count_per_thread);
      ASSERT_TRUE(mTestThreads[i].Initialize(InsertionRoutine, &mArgDatas[i]));
      mTestThreads[i].Run();

      // Insert numbers to remove here.
      for (uint32_t n = 0; n < count_per_thread; ++n) {
        const uint32_t value = count_per_thread * (i+1) + n;
        mHashTable.Insert(value, value);
      }
      mArgDatas[i+1].Init(&mHashTable, &mBeginRunning,
                          count_per_thread * (i+1), count_per_thread);
      ASSERT_TRUE(mTestThreads[i+1].Initialize(RemovalRoutine,
                                               &mArgDatas[i+1]));
      mTestThreads[i+1].Run();
    }

    // Simultaneously insert and remove
    mBeginRunning = 1;
    for (uint32_t i = 0; i < NUM_THREADS; ++i) {
      mTestThreads[i].Join();
      EXPECT_EQ(0, mTestThreads[i].ReturnValue());
    }

    // Check hash table integrity.
    int32_t total_elements = 0;
    for (uint32_t i = 0; i < NUM_THREADS; i += 2) {
      // Test Insertions
      for (uint32_t n = 0; n < count_per_thread; ++n) {
        const uint32_t test_key = count_per_thread * i + n;
        const uint32_t* test_value = mHashTable.GetValue(test_key);
        ASSERT_NE(nullptr, test_value);
        EXPECT_EQ(test_key, *test_value);
        ++total_elements;
      }

      // Test Removals
      for (uint32_t n = 0; n < count_per_thread; ++n) {
        const uint32_t test_key = count_per_thread * (i+1) + n;
        const uint32_t* test_value = mHashTable.GetValue(test_key);
        ASSERT_EQ(nullptr, test_value);
      }
    }
    ASSERT_EQ(total_elements, mHashTable.GetCurrentSize());
  }

  ContainedFullAtomicHashTable<uint32_t, uint32_t, HASH_TABLE_SIZE> mHashTable;
  volatile uint32_t mBeginRunning;
  InsertionArg mArgDatas[NUM_THREADS];
  YCommon::YPlatform::Thread mTestThreads[NUM_THREADS];
};

class MinimalAtomicHashTest : public ThreadedHashTableTest<50, 10> {};
class SmallAtomicHashTest : public ThreadedHashTableTest<1000, 200> {};
class LargeAtomicHashTest : public ThreadedHashTableTest<50000, 10000> {};

#define ATOMIC_HASH_TABLE_TEST(NAME) \
  TEST_F(MinimalAtomicHashTest , Minimal ## NAME) { \
    for (int i = 0; i < 100; ++i) { Run ## NAME(); } \
  } \
  TEST_F(SmallAtomicHashTest, Small ## NAME ) { \
    for (int i = 0; i < 1000; ++i) { Run ## NAME(); } \
  } \
  TEST_F(LargeAtomicHashTest, Large ## NAME) { \
    for (int i = 0; i < 5000; ++i) { Run ## NAME(); } \
  }

ATOMIC_HASH_TABLE_TEST(InsertionTest);
ATOMIC_HASH_TABLE_TEST(RemovalTest);
ATOMIC_HASH_TABLE_TEST(InsertRemovalTest);

}} // namespace YCommon { namespace YContainers {
