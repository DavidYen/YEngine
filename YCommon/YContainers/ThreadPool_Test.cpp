#include <YCommon/Headers/stdincludes.h>
#include "ThreadPool.h"

#include <gtest/gtest.h>

#include <YCommon/Headers/Atomics.h>
#include <YCommon/YPlatform/Semaphore.h>
#include <YCommon/YPlatform/Timer.h>

namespace YCommon { namespace YContainers {

struct IncrementArg {
  volatile uint32_t* num;
};

static uintptr_t IncrementRoutine(void* arg) {
  volatile uint32_t* num = static_cast<IncrementArg*>(arg)->num;
  AtomicAdd32(num, 1);

  return 0;
}

static uintptr_t ReleaseSemaphoreRoutine(void* arg) {
  YPlatform::Semaphore* semaphore = static_cast<YPlatform::Semaphore*>(arg);
  semaphore->Release();
  return 0;
}

struct ReleaseWaitArg {
  YPlatform::Semaphore release_semaphore;
  YPlatform::Semaphore wait_semaphore;

  ReleaseWaitArg()
    : release_semaphore(0, 1),
      wait_semaphore(0, 1) {
  }
};

static uintptr_t ReleaseWaitSemaphoreRoutine(void* arg) {
  ReleaseWaitArg* arg_data = static_cast<ReleaseWaitArg*>(arg);
  arg_data->release_semaphore.Release();
  arg_data->wait_semaphore.Wait();
  return 0;
}

static uintptr_t WaitSemaphoreRoutine(void* arg) {
  YPlatform::Semaphore* semaphore = static_cast<YPlatform::Semaphore*>(arg);
  semaphore->Wait();
  return 0;
}

TEST(BasicThreadPoolTest, ConstructorTest) {
  char buffer[512];
  ThreadPool pool(2, buffer, sizeof(buffer));

  ContainedThreadPool<2, 10> contained_pool;
  ASSERT_EQ(contained_pool.GetQueueSize(), 10);
}

TEST(BasicThreadPoolTest, IncrementRoutineTest) {
  volatile uint32_t num = 0;
  IncrementArg arg_data = { &num };
  EXPECT_EQ(0, IncrementRoutine(&arg_data));
  EXPECT_EQ(1, num);
}

TEST(BasicThreadPoolTest, StartPoolTest) {
  uint8_t buffer[512];
  ThreadPool thread_pool(2, buffer, sizeof(buffer));

  ASSERT_TRUE(thread_pool.Start());
}

TEST(BasicThreadPoolTest, StopPoolTest) {
  ContainedThreadPool<2, 10> thread_pool;

  ASSERT_TRUE(thread_pool.Start());
  ASSERT_TRUE(thread_pool.Stop(50));
}

TEST(BasicThreadPoolTest, StopPoolFailTest) {
  ReleaseWaitArg arg_data;
  ContainedThreadPool<2, 10> thread_pool;

  ASSERT_TRUE(thread_pool.Start());
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseWaitSemaphoreRoutine, &arg_data));

  arg_data.release_semaphore.Wait(50);
  ASSERT_FALSE(thread_pool.Stop(50));

  arg_data.wait_semaphore.Release();
  ASSERT_TRUE(thread_pool.Stop(50));
}

TEST(BasicThreadPoolTest, StartThreadPoolTest) {
  volatile uint32_t num = 0;
  YPlatform::Semaphore semaphore(0, 1);
  IncrementArg arg_data = { &num };
  ContainedThreadPool<2, 10> thread_pool;

  ASSERT_TRUE(thread_pool.Start());
  ASSERT_TRUE(thread_pool.EnqueueRun(IncrementRoutine, &arg_data));
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseSemaphoreRoutine, &semaphore));
  ASSERT_TRUE(semaphore.Wait(50));
  EXPECT_EQ(1, num);
}

TEST(BasicThreadPoolTest, StopThreadPoolTest) {
  YPlatform::Semaphore semaphore(0, 1);
  ContainedThreadPool<2, 10> thread_pool;

  ASSERT_TRUE(thread_pool.Start());
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseSemaphoreRoutine, &semaphore));
  ASSERT_TRUE(semaphore.Wait(50));
  ASSERT_TRUE(thread_pool.Stop(50));
}

TEST(BasicThreadPoolTest, EnqueueMultiple) {
  volatile uint32_t num = 0;
  YPlatform::Semaphore semaphore(0, 1);
  IncrementArg arg_data = { &num };
  ContainedThreadPool<2, 20> thread_pool;

  thread_pool.Start();
  for (int i = 0; i < 10; ++i) {
    thread_pool.EnqueueRun(IncrementRoutine, &arg_data);
  }
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseSemaphoreRoutine, &semaphore));
  ASSERT_TRUE(semaphore.Wait(50));
  ASSERT_TRUE(thread_pool.Stop(50));
  EXPECT_EQ(10, num);
}

TEST(BasicThreadPoolTest, StartStopTest) {
  volatile uint32_t num = 0;
  YPlatform::Semaphore semaphore(0, 1);
  IncrementArg arg_data = { &num };
  ContainedThreadPool<1, 20> thread_pool;

  thread_pool.Start();
  for (int i = 0; i < 10; ++i) {
    thread_pool.EnqueueRun(IncrementRoutine, &arg_data);
  }
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseSemaphoreRoutine, &semaphore));
  ASSERT_TRUE(semaphore.Wait(50));
  ASSERT_TRUE(thread_pool.Stop(50));
  EXPECT_EQ(10, num);

  thread_pool.Start();
  for (int i = 0; i < 10; ++i) {
    thread_pool.EnqueueRun(IncrementRoutine, &arg_data);
  }
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseSemaphoreRoutine, &semaphore));
  ASSERT_TRUE(semaphore.Wait(50));
  ASSERT_TRUE(thread_pool.Stop(50));
  EXPECT_EQ(20, num);
}

TEST(BasicThreadPoolTest, StartStopFailTest) {
  volatile uint32_t num = 0;
  ReleaseWaitArg arg_data1;
  ReleaseWaitArg arg_data2;
  IncrementArg arg_data = { &num };
  ContainedThreadPool<2, 20> thread_pool;

  thread_pool.Start();
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseWaitSemaphoreRoutine, &arg_data1));
  ASSERT_TRUE(arg_data1.release_semaphore.Wait(50));
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseWaitSemaphoreRoutine, &arg_data2));
  ASSERT_TRUE(arg_data2.release_semaphore.Wait(50));

  // Release semaphore 1
  arg_data1.wait_semaphore.Release();

  // Only first thread is stoppable.
  ASSERT_FALSE(thread_pool.Stop(100));

  // Also stop the 2nd thread now
  arg_data2.wait_semaphore.Release();

  YPlatform::Semaphore semaphore1(0, 1);
  YPlatform::Semaphore semaphore2(0, 1);

  // Restart threads and make sure we have 2 active threads here.
  thread_pool.Start();

  // Have both threads be waiting for the semaphore, but only wake one up.
  ASSERT_TRUE(thread_pool.EnqueueRun(WaitSemaphoreRoutine, &semaphore1));
  ASSERT_TRUE(thread_pool.EnqueueRun(WaitSemaphoreRoutine, &semaphore1));

  // Only wake one up.
  semaphore1.Release();

  for (int i = 0; i < 10; ++i) {
    thread_pool.EnqueueRun(IncrementRoutine, &arg_data);
  }
  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseSemaphoreRoutine, &semaphore2));
  ASSERT_TRUE(semaphore2.Wait(500));

  semaphore1.Release();
  ASSERT_TRUE(thread_pool.Stop(500));
  EXPECT_EQ(10, num);
}

TEST(BasicThreadPoolTest, PauseJoinTest) {
  ReleaseWaitArg arg_data;
  ContainedThreadPool<2, 10> thread_pool;

  ASSERT_TRUE(thread_pool.Start());
  EXPECT_FALSE(thread_pool.Join(0));

  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseWaitSemaphoreRoutine, &arg_data));
  ASSERT_TRUE(arg_data.release_semaphore.Wait(50));
  ASSERT_TRUE(thread_pool.Pause());

  YPlatform::Timer test_timer;
  test_timer.Start();
  EXPECT_FALSE(thread_pool.Join(100));
  test_timer.Pulse();

  EXPECT_LE(95, test_timer.GetPulsedTimeMilli());
  EXPECT_GE(120, test_timer.GetPulsedTimeMilli());

  // Releasing the semaphore should let the join succeed.
  arg_data.wait_semaphore.Release();
  EXPECT_TRUE(thread_pool.Join(50));
}

TEST(BasicThreadPoolTest, StopJoinTest) {
  ReleaseWaitArg arg_data;
  ContainedThreadPool<2, 10> thread_pool;

  ASSERT_TRUE(thread_pool.Start());
  EXPECT_FALSE(thread_pool.Join(0));

  ASSERT_TRUE(thread_pool.EnqueueRun(ReleaseWaitSemaphoreRoutine, &arg_data));
  ASSERT_TRUE(arg_data.release_semaphore.Wait(50));
  thread_pool.Stop(0);

  YPlatform::Timer test_timer;
  test_timer.Start();
  EXPECT_FALSE(thread_pool.Join(100));
  test_timer.Pulse();

  EXPECT_LE(95, test_timer.GetPulsedTimeMilli());
  EXPECT_GE(120, test_timer.GetPulsedTimeMilli());

  // Releasing the semaphore should let the join succeed.
  arg_data.wait_semaphore.Release();
  EXPECT_TRUE(thread_pool.Join(50));
}

}} // namespace YCommon { namespace YContainers {
