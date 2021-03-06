#include "ycommon/platform/thread.h"

#include <string>
#include <gtest/gtest.h>

#include "ycommon/headers/atomics.h"
#include "ycommon/platform/timer.h"

namespace ycommon { namespace platform {

/* Nop Routines, just returns the arg. */
uintptr_t NopRoutine(void* arg) {
  return reinterpret_cast<uintptr_t>(arg);
}

/* Simple routine which signals that it's running and waits to be stopped. */
struct RunStopArg {
  volatile int running;
  volatile int stop_test;

  RunStopArg()
      : running(0),
        stop_test(0) {
  }

  void WaitForThreadToStart() {
    while (running == 0) {
      MemoryBarrier();
    }
  }

  void StopThread() {
    MemoryBarrier();
    stop_test = 1;
  }
};

uintptr_t RunStopRoutine(void* arg) {
  RunStopArg* arg_data = static_cast<RunStopArg*>(arg);
  arg_data->running = 1;
  MemoryBarrier();

  while (arg_data->stop_test != 1);
  return 0;
}

/* Simple routine which increments a number. */
struct IncrementArg {
  volatile uint32_t num;

  IncrementArg() : num(0) {}
};

uintptr_t IncrementRoutine(void* arg) {
  IncrementArg* arg_data = static_cast<IncrementArg*>(arg);
  AtomicAdd32(&arg_data->num, 1);
  return 0;
}

/************
* Test definitions
*************/
TEST(BasicThreadTest, StartStopTest) {
  Thread test_thread(NopRoutine, NULL);
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_TRUE(test_thread.Join(5000));
}

TEST(BasicThreadTest, NameTest) {
  const char* test_name = "test_name";
  Thread test_thread(NopRoutine, NULL, test_name);
  ASSERT_STREQ(test_name, test_thread.GetName());

  const char* test_name2 = "test_name 2";
  test_thread.SetName(test_name2);
  ASSERT_STREQ(test_name2, test_thread.GetName());
}

TEST(BasicThreadTest, IsRunningTest) {
  RunStopArg arg;
  Thread test_thread(RunStopRoutine, &arg);

  ASSERT_FALSE(test_thread.IsRunning());
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());

  arg.WaitForThreadToStart();

  ASSERT_TRUE(test_thread.IsRunning());

  arg.StopThread();

  ASSERT_TRUE(test_thread.Join(500));
  ASSERT_FALSE(test_thread.IsRunning());
}

TEST(BasicThreadTest, JoinTest) {
  RunStopArg arg;
  Thread test_thread(RunStopRoutine, &arg);

  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  arg.WaitForThreadToStart();

  ASSERT_FALSE(test_thread.Join(100));

  arg.StopThread();

  ASSERT_TRUE(test_thread.Join(500));
}

TEST(BasicThreadTest, JoinFinishedThreadTest) {
  RunStopArg arg;
  Thread test_thread(RunStopRoutine, &arg);

  ASSERT_EQ(kStatusCode_OK, test_thread.Run());

  arg.StopThread();

  ASSERT_TRUE(test_thread.Join(500));
  ASSERT_TRUE(test_thread.Join(500));
}

TEST(BasicThreadTest, JoinInMilliseconds) {
  RunStopArg arg;
  Thread test_thread(RunStopRoutine, &arg);

  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  arg.WaitForThreadToStart();

  Timer test_timer;
  test_timer.Start();
  ASSERT_FALSE(test_thread.Join(100));
  test_timer.Pulse();

  EXPECT_LE(95, test_timer.GetPulsedTimeMilli());
  EXPECT_GE(120, test_timer.GetPulsedTimeMilli());

  arg.StopThread();

  ASSERT_TRUE(test_thread.Join(5000));
}

TEST(BasicThreadTest, ReturnValueTest) {
  uintptr_t test_value = 123;
  Thread test_thread(NopRoutine, reinterpret_cast<void*>(test_value));
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_TRUE(test_thread.Join(5000));

  ASSERT_EQ(test_value, test_thread.ReturnValue());
}

TEST(BasicThreadTest, ThreadsRunTest) {
  IncrementArg arg;
  Thread test_thread(IncrementRoutine, &arg);
  Thread test_thread2(IncrementRoutine, &arg);

  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_EQ(kStatusCode_OK, test_thread2.Run());
  ASSERT_TRUE(test_thread.Join(5000));
  ASSERT_TRUE(test_thread2.Join(5000));

  ASSERT_EQ(arg.num, 2);
}

}} // namespace ycommon { namespace platform {
