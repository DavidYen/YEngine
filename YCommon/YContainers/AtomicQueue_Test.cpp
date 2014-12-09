#include <YCommon/Headers/stdincludes.h>
#include "AtomicQueue.h"

#include <gtest/gtest.h>

namespace YCommon { namespace YContainers {

TEST(BasicAtomicQueueTest, ConstructorTest) {
  uint8_t queue_data[sizeof(int)];
  AtomicQueue atomic_queue(queue_data, sizeof(queue_data),
                           sizeof(int), 1);

  int typed_data[1];
  TypedAtomicQueue<int> typed_queue(typed_data,
                                    sizeof(typed_data),
                                    ARRAY_SIZE(typed_data));

  ContainedAtomicQueue<int, 1> contained_array;
}

TEST(BasicAtomicQueueTest, SizeTest) {
  int queue_data[2];
  AtomicQueue atomic_queue(queue_data, sizeof(queue_data),
                           sizeof(int), 2);

  EXPECT_EQ(sizeof(int), atomic_queue.ItemSize());
  EXPECT_EQ(ARRAY_SIZE(queue_data), atomic_queue.Size());

  int typed_data[4];
  TypedAtomicQueue<int> typed_queue(typed_data,
                                    sizeof(typed_data),
                                    ARRAY_SIZE(typed_data));

  EXPECT_EQ(sizeof(int), typed_queue.ItemSize());
  EXPECT_EQ(ARRAY_SIZE(typed_data), typed_queue.Size());

  ContainedAtomicQueue<int, 6> contained_array;
  EXPECT_EQ(sizeof(int), contained_array.ItemSize());
  EXPECT_EQ(6, contained_array.Size());
}

TEST(BasicAtomicQueueTest, EnqueueTest) {
  int queue_data[2];
  AtomicQueue queue(queue_data, sizeof(queue_data),
                    sizeof(queue_data[0]), ARRAY_SIZE(queue_data));

  int data = 1;
  ASSERT_TRUE(queue.Enqueue(&data));
}

TEST(BasicAtomicQueueTest, DequeueTest) {
  int queue_data[2];
  AtomicQueue queue(queue_data, sizeof(queue_data),
                    sizeof(queue_data[0]), ARRAY_SIZE(queue_data));

  int data = 1;
  ASSERT_TRUE(queue.Enqueue(&data));

  int dequeued_data = 0;
  ASSERT_TRUE(queue.Dequeue(&dequeued_data));

  ASSERT_EQ(dequeued_data, data);
}

TEST(BasicAtomicQueueTest, FullEnqueueTest) {
  int queue_data[2];
  AtomicQueue queue(queue_data, sizeof(queue_data),
                    sizeof(queue_data[0]), ARRAY_SIZE(queue_data));

  int data = 1;
  ASSERT_TRUE(queue.Enqueue(&data));
  ASSERT_TRUE(queue.Enqueue(&data));
  ASSERT_FALSE(queue.Enqueue(&data));
}

TEST(BasicAtomicQueueTest, EmptyDequeueTest) {
  int queue_data[2];
  AtomicQueue queue(queue_data, sizeof(queue_data),
                    sizeof(queue_data[0]), ARRAY_SIZE(queue_data));

  int data = 0;
  ASSERT_FALSE(queue.Dequeue(&data));
}

TEST(BasicAtomicQueueTest, TypedFunctions) {
  int queue_data[2] = { 0 };
  TypedAtomicQueue<int> queue(queue_data,
                              sizeof(queue_data),
                              ARRAY_SIZE(queue_data));

  const int insertion_data = 123;
  ASSERT_TRUE(queue.Enqueue(insertion_data));

  int dequeue_data = 0;
  ASSERT_TRUE(queue.Dequeue(dequeue_data));
  EXPECT_EQ(insertion_data, dequeue_data);
}

TEST(BasicAtomicQueueTest, ContainedFunctions) {
  ContainedAtomicQueue<int, 2> queue;

  const int insertion_data = 123;
  ASSERT_TRUE(queue.Enqueue(insertion_data));

  int dequeue_data = 0;
  ASSERT_TRUE(queue.Dequeue(dequeue_data));
  EXPECT_EQ(insertion_data, dequeue_data);
}

TEST(BasicAtomicQueueTest, ReuseTest) {
  ContainedAtomicQueue<int, 2> queue;

  const int insertion_data1 = 123;
  const int insertion_data2 = 234;
  const int insertion_data3 = 345;
  int dequeue_data = 0;

  // First enqueue 1, 2, 3
  ASSERT_TRUE(queue.Enqueue(insertion_data1));
  ASSERT_TRUE(queue.Enqueue(insertion_data2));
  EXPECT_FALSE(queue.Enqueue(insertion_data3));

  ASSERT_TRUE(queue.Dequeue(dequeue_data));
  EXPECT_EQ(insertion_data1, dequeue_data);

  ASSERT_TRUE(queue.Dequeue(dequeue_data));
  EXPECT_EQ(insertion_data2, dequeue_data);

  EXPECT_FALSE(queue.Dequeue(dequeue_data));

  // Now enqueue 2, 3, 1
  ASSERT_TRUE(queue.Enqueue(insertion_data2));
  ASSERT_TRUE(queue.Enqueue(insertion_data3));
  EXPECT_FALSE(queue.Enqueue(insertion_data1));

  ASSERT_TRUE(queue.Dequeue(dequeue_data));
  EXPECT_EQ(insertion_data2, dequeue_data);

  ASSERT_TRUE(queue.Dequeue(dequeue_data));
  EXPECT_EQ(insertion_data3, dequeue_data);

  EXPECT_FALSE(queue.Dequeue(dequeue_data));
}

}} // namespace YCommon { namespace YContainers {
