#include <YCommon/Headers/stdincludes.h>
#include "AtomicMemPool.h"

#include <gtest/gtest.h>

#include <YCommon/YPlatform/Thread.h>

#define NUM_THREADS 10

namespace YCommon { namespace YContainers {

struct AllocRemoveArg {
  TypedAtomicMemPool<int>* mem_pool;
  uint32_t num_allocations;
  int num_iterations;

  AllocRemoveArg(TypedAtomicMemPool<int>* mem_pool_value,
                 uint32_t allocations, int iterations)
      : mem_pool(mem_pool_value),
        num_allocations(allocations),
        num_iterations(iterations) {}
};

uintptr_t AllocRemoveRoutine(void* arg) {
  int retvalue = 0;
  AllocRemoveArg* arg_data = static_cast<AllocRemoveArg*>(arg);
  TypedAtomicMemPool<int>* mem_pool = arg_data->mem_pool;
  const uint32_t mem_pool_size = mem_pool->GetNumItems();
  uint32_t* allocated_data = new uint32_t[mem_pool_size];

  const uint32_t num_allocations = arg_data->num_allocations;
  const int num_iterations = arg_data->num_iterations;
  int insert_data = 0;
  for (int i = 0; i < num_iterations; ++i) {
    uint32_t allocated_size = 0;

    // Allocate data until it is full
    for (uint32_t index = mem_pool->Insert(insert_data);
         index != static_cast<uint32_t>(-1);
         index = mem_pool->Insert(insert_data)) {
      allocated_data[allocated_size++] = index;
      if (allocated_size > mem_pool_size) {
        retvalue = 1;
        break;
      } else if (allocated_size >= num_allocations) {
        break;
      }
    }

    if (retvalue != 0)
      break;

    // Remove all the allocated data
    for (uint32_t i = 0; i < allocated_size; ++i) {
      mem_pool->Remove(allocated_data[i]);
    }
  }

  delete [] allocated_data;
  return retvalue;
}

/************
* Test Definitions
*************/
TEST(AtomicMemPoolTest, AllocRemoveTest) {
  ContainedAtomicMemPool<int, 100> mem_pool;
  AllocRemoveArg arg_data(&mem_pool, 100, 1);

  ASSERT_EQ(0, AllocRemoveRoutine(&arg_data));

  // Make sure we can still allocate 100 items
  uint32_t data = 123;
  for (int i = 0; i < 100; ++i) {
    ASSERT_NE(static_cast<uint32_t>(-1), mem_pool.Insert(data));
  }
  ASSERT_EQ(static_cast<uint32_t>(-1), mem_pool.Insert(data));
}

/*************
* Test constant adding and removing
**************/
class ThreadedAllocRemoveTest : public ::testing::Test {
 public:
  template <size_t test_size>
  void RunTest(int iterations) {
    ContainedAtomicMemPool<int, test_size> mem_pool;
    AllocRemoveArg arg_data(&mem_pool, test_size, iterations);
    AllocRemoveArg arg_half_data(&mem_pool, test_size / 2, iterations);

    YCommon::YPlatform::Thread test_threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
      test_threads[i].Initialize(AllocRemoveRoutine,
                                 (i % 2 == 0 ? &arg_data : &arg_half_data));
      test_threads[i].Run();
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
      test_threads[i].Join();
      EXPECT_EQ(0, test_threads[i].ReturnValue());
    }

    // Make sure we can still allocate 100 items
    uint32_t data = 123;
    for (int i = 0; i < test_size; ++i) {
      ASSERT_NE(static_cast<uint32_t>(-1), mem_pool.Insert(data));
    }
    ASSERT_EQ(static_cast<uint32_t>(-1), mem_pool.Insert(data));
  }
};

TEST_F(ThreadedAllocRemoveTest, MinimalTests) {
  RunTest<10>(10);
}

TEST_F(ThreadedAllocRemoveTest, SmallTests) {
  RunTest<100>(100000);
}

TEST_F(ThreadedAllocRemoveTest, LargeTests) {
  RunTest<2000>(100000);
}


}} // namespace YCommon { namespace YContainers {
