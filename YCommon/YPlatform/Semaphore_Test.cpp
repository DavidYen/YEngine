#include <YCommon/Headers/stdincludes.h>
#include "Semaphore.h"

#include <string>
#include <gtest/gtest.h>

#include <YCommon/Headers/Atomics.h>

#include "Thread.h"

namespace YCommon { namespace YPlatform {

struct SemaphoreArg {
  Semaphore* semaphore;
  size_t wait_time;

  SemaphoreArg(Semaphore* semaphore_item, size_t wait_time_value = -1)
      : semaphore(semaphore_item),
        wait_time(wait_time_value) {}
};

uintptr_t SemaphoreRoutine(void* arg) {
  SemaphoreArg* semaphore_arg = static_cast<SemaphoreArg*>(arg);
  return semaphore_arg->semaphore->Wait(semaphore_arg->wait_time) ? 0 : 1;
}

/************
* Test definitions
*************/
TEST(BasicSemaphoreTest, WaitReleaseTest) {
  Semaphore semaphore(0, 1);
  SemaphoreArg arg_data(&semaphore);

  Thread test_thread(SemaphoreRoutine, &arg_data);
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_FALSE(test_thread.Join(100));

  semaphore.Release();
  ASSERT_TRUE(test_thread.Join(5000));
  ASSERT_EQ(0, test_thread.ReturnValue());
}

TEST(BasicSemaphoreTest, InitialCountTest) {
  Semaphore semaphore(1, 1);
  SemaphoreArg arg_data(&semaphore);

  Thread test_thread(SemaphoreRoutine, &arg_data);
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_TRUE(test_thread.Join(5000));
  ASSERT_EQ(0, test_thread.ReturnValue());
}

TEST(BasicSemaphoreTest, MaximumCountTest) {
  Semaphore semaphore(0, 2);
  SemaphoreArg arg_data(&semaphore);

  Thread test_thread(SemaphoreRoutine, &arg_data);
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_FALSE(test_thread.Join(100));
  semaphore.Release();
  ASSERT_TRUE(test_thread.Join(5000));
  ASSERT_EQ(0, test_thread.ReturnValue());

  Thread test_thread2(SemaphoreRoutine, &arg_data);
  ASSERT_EQ(kStatusCode_OK, test_thread2.Run());
  ASSERT_FALSE(test_thread2.Join(100));
  semaphore.Release();
  ASSERT_TRUE(test_thread2.Join(5000));
  ASSERT_EQ(0, test_thread2.ReturnValue());
}

TEST(BasicSemaphoreTest, WaitFailTest) {
  Semaphore semaphore(0, 1);
  SemaphoreArg arg_data(&semaphore, 10);

  Thread test_thread(SemaphoreRoutine, &arg_data);
  ASSERT_EQ(kStatusCode_OK, test_thread.Run());
  ASSERT_TRUE(test_thread.Join(5000));
  ASSERT_EQ(1, test_thread.ReturnValue());
}

}} // namespace YCommon { namespace YPlatform {
