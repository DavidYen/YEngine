#ifndef YCOMMON_YCONTAINERS_THREADPOOL_H
#define YCOMMON_YCONTAINERS_THREADPOOL_H

#include <YCommon/YPlatform/Semaphore.h>
#include <YCommon/YPlatform/Thread.h>

#include "AtomicQueue.h"

namespace YCommon { namespace YContainers {

class ThreadPool {
 public:
  ThreadPool(size_t num_threads, void* buffer, size_t buffer_size);
  ~ThreadPool();

  bool EnqueueRun(ThreadRoutine thread_routine, void* thread_args);

  bool Start();
  bool Pause();

  bool Running() const {
    return mThreadData.mRunState == ThreadData::kRunState_Running;
  }

  bool Paused() const {
    return mThreadData.mRunState == ThreadData::kRunState_Paused;
  }

  size_t GetQueueSize() const {
    return (mBufferSize - sizeof(YThread) * mNumThreads) /
           sizeof(ThreadData::RunArgs);
  }

 protected:
  static int ThreadPoolThread(void* arg);

 private:
  YThread* mThreads;
  size_t mNumThreads;

  void* mBuffer;
  size_t mBufferSize;

  struct ThreadData {
    ThreadData(size_t num_threads, void* buffer, size_t buffer_size);

    volatile enum RunState {
      kRunState_Idle,
      kRunState_Running,
      kRunState_Paused,
      kRunState_Stopped,
    } mRunState;

    YSemaphore mSemaphore;

    struct RunArgs {
      ThreadRoutine thread_routine;
      void* thread_args;
    };
    TypedAtomicQueue<RunArgs> mRunQueue;
  } mThreadData;
};

}} // namespace YCommon { namespace YContainers {

#endif // YCOMMON_YCONTAINERS_THREADPOOL_H
