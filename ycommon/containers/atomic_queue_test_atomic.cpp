#include "ycommon/containers/atomic_queue.h"

#include <gtest/gtest.h>

#include "ycommon/headers/atomics.h"
#include "ycommon/platform/thread.h"

#define NUM_THREADS 5

namespace ycommon { namespace containers {

struct DequeueThreadArg {
  TypedAtomicQueue<uint32_t>* queue;
  volatile uint32_t* keep_going;

  DequeueThreadArg(TypedAtomicQueue<uint32_t>* queue_ptr,
                   volatile uint32_t* keep_going_ptr)
      : queue(queue_ptr),
        keep_going(keep_going_ptr) {}
};

uintptr_t DequeueThreadRoutine(void* arg) {
  DequeueThreadArg* arg_data = static_cast<DequeueThreadArg*>(arg);

  uintptr_t ret = 0;
  uint32_t prev_value = 0;
  uint32_t current_value = 0;

  do {
    MemoryBarrier();
    while (arg_data->queue->Dequeue(current_value)) {
      if (current_value <= prev_value) {
        ret = 1;
        *arg_data->keep_going = 0;
        break;
      }
      prev_value = current_value;
    }
  } while (*arg_data->keep_going);

  return ret;
}

/************
* Test Definitions
*************/
TEST(AtomicQueueTest, DequeueRoutineTest) {
  ContainedAtomicQueue<uint32_t, 2> queue;
  volatile uint32_t keep_going = 0;

  DequeueThreadArg arg_data(&queue, &keep_going);

  // Enqueue 1 item.
  uint32_t data = 1;
  ASSERT_TRUE(queue.Enqueue(data));

  // Make sure routine returns 0.
  ASSERT_EQ(0, DequeueThreadRoutine(&arg_data));

  // Ensure queue is empty.
  ASSERT_FALSE(queue.Dequeue(data));
}

TEST(AtomicQueueTest, DequeueRoutineFailTest) {
  ContainedAtomicQueue<uint32_t, 2> queue;
  volatile uint32_t keep_going = 1;

  DequeueThreadArg arg_data(&queue, &keep_going);

  // Enqueue items in decreasing order.
  uint32_t data = 1;
  ASSERT_TRUE(queue.Enqueue(data));

  data = 0;
  ASSERT_TRUE(queue.Enqueue(data));

  // Routine should now return 1 because of bad order.
  ASSERT_EQ(1, DequeueThreadRoutine(&arg_data));

  // Ensure queue is empty.
  ASSERT_FALSE(queue.Dequeue(data));
}

/*************
* Test constant adding and removing
**************/
template <uint32_t QUEUE_COUNT, uint32_t QUEUE_SIZE>
class ThreadedDequeueTest : public ::testing::Test {
 public:
  void RunTest(int iterations) {
    for (int i = 0; i < iterations; ++i) {
      volatile uint32_t keep_going = 1;
      DequeueThreadArg arg_data(&mQueue, &keep_going);

      // Start full
      for (uint32_t n = 1; n <= QUEUE_SIZE; ++n) {
        ASSERT_TRUE(mQueue.Enqueue(n));
      }

      ycommon::platform::Thread test_threads[NUM_THREADS];
      for (int n = 0; n < NUM_THREADS; ++n) {
        test_threads[n].Initialize(DequeueThreadRoutine, &arg_data);
        test_threads[n].Run();
      }

      bool error = false;
      for (uint32_t n = QUEUE_SIZE+1; n < QUEUE_COUNT && !error; ++n) {
        while (!mQueue.Enqueue(n)) {
          if (keep_going == 0) {
            error = true;
            break;
          }
        }
      }

      keep_going = 0;

      for (int n = 0; n < NUM_THREADS; ++n) {
        test_threads[n].Join();
        EXPECT_EQ(0, test_threads[n].ReturnValue());
      }

      uint32_t last_item = 0;
      ASSERT_FALSE(mQueue.Dequeue(last_item));
    }
  }

  ContainedAtomicQueue<uint32_t, QUEUE_SIZE> mQueue;
};

class MinimalAtomicQueueTest : public ThreadedDequeueTest<10, 10> {};
class SmallAtomicQueueTest : public ThreadedDequeueTest<100, 50> {};
class LargeAtomicQueueTest : public ThreadedDequeueTest<20000, 10000> {};

TEST_F(MinimalAtomicQueueTest , MinimalTests) {
  RunTest(10);
}

TEST_F(SmallAtomicQueueTest, SmallTests) {
  RunTest(5000);
}

TEST_F(LargeAtomicQueueTest, LargeTests) {
  RunTest(10000);
}

}} // namespace ycommon { namespace containers {
