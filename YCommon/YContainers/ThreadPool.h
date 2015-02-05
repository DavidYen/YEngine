#ifndef YCOMMON_YCONTAINERS_THREADPOOL_H
#define YCOMMON_YCONTAINERS_THREADPOOL_H

#include <YCommon/YPlatform/Semaphore.h>
#include <YCommon/YPlatform/Thread.h>

#include "AtomicQueue.h"

/***********************
* ThreadPool handles a number of threads and allows someone to
*   enqueue thread routines.
*
*   - Space Requirements: sizeof(YPlatform::Thread) * NUM_THREADS +
*                         sizeof(YThread::RunArgs) * QUEUE_SIZE
************************/

namespace YCommon { namespace YContainers {

class ThreadPool {
 public:
  struct RunArgs {
    YPlatform::ThreadRoutine thread_routine;
    void* thread_args;
  };

  ThreadPool();
  ThreadPool(size_t num_threads, void* buffer, size_t buffer_size);
  ~ThreadPool();

  void Initialize(size_t num_threads, void* buffer, size_t buffer_size);
  bool EnqueueRun(YPlatform::ThreadRoutine thread_routine, void* thread_args);

  bool Start();
  bool Pause();
  bool Stop(size_t milliseconds = -1);

  // Will wait till no jobs are running, only valid for Stopped or Pause states.
  // Will immediately return false if not Stopped or Paused.
  bool Join(size_t milliseconds = -1);

  bool Running() const {
    return mThreadData.mRunState == ThreadData::kRunState_Running;
  }

  bool Paused() const {
    return mThreadData.mRunState == ThreadData::kRunState_Paused;
  }

  size_t GetQueueSize() const {
    return mThreadData.mRunQueue.Size();
  }

 protected:
  static uintptr_t ThreadPoolThread(void* arg);

 protected:
  YPlatform::Thread* mThreads;
  size_t mNumThreads;

  struct ThreadData {
    ThreadData()
      : mRunState(kRunState_Invalid),
        mThreadsRunning(0) {}

    void Initialize(size_t num_threads, void* buffer, size_t buffer_size);

    volatile enum RunState {
      kRunState_Invalid,

      kRunState_Running,
      kRunState_Paused,
      kRunState_Stopped,
    } mRunState;

    YPlatform::Semaphore mSemaphore;
    YPlatform::Semaphore mParentSemaphore;
    TypedAtomicQueue<RunArgs> mRunQueue;
    volatile uint32_t mThreadsRunning;
  } mThreadData;
};

template <size_t NUM_THREADS, size_t QUEUE_SIZE>
class ContainedThreadPool : public ThreadPool {
 public:
  ContainedThreadPool() : ThreadPool(NUM_THREADS, mBuffer, sizeof(mBuffer)) {}
  ~ContainedThreadPool() {}

 private:
  uint8_t mBuffer[sizeof(YPlatform::Thread) * NUM_THREADS +
                  sizeof(ThreadPool::RunArgs) * QUEUE_SIZE];
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_THREADPOOL_H
