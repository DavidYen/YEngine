#include <YCommon/Headers/stdincludes.h>
#include "ThreadPool.h"

#include <YCommon/Headers/Atomics.h>

namespace YCommon { namespace YContainers {

ThreadPool::ThreadData::ThreadData(size_t num_threads, void* buffer,
                                   size_t buffer_size)
    : mRunState(kRunState_Idle),
      mSemaphore(static_cast<int>(num_threads),
                 static_cast<int>(num_threads)),
      mRunQueue(static_cast<ThreadPool::ThreadData::RunArgs*>(buffer),
                sizeof(RunArgs),
                ((buffer_size - sizeof(YPlatform::Thread)*num_threads) /
                 sizeof(ThreadPool::ThreadData::RunArgs))) {
}

ThreadPool::ThreadPool(size_t num_threads, void* buffer, size_t buffer_size)
    : mThreads(NULL),
      mNumThreads(0),
      mBuffer(buffer),
      mBufferSize(buffer_size),
      mThreadData(num_threads, buffer, buffer_size) {
  const size_t thread_size = sizeof(YPlatform::Thread) * num_threads;
  YASSERT(buffer_size > thread_size,
          "Buffer size is not big enough to contain ThreadPool");

  const size_t num_run_args = (buffer_size - thread_size) /
                              sizeof(ThreadPool::ThreadData::RunArgs);
  YASSERT(num_run_args > 0,
          "Number of run arguments must be greater than 0");

  uint8_t* thread_loc = static_cast<uint8_t*>(buffer) + num_run_args;
  mThreads = new (thread_loc) YPlatform::Thread[num_threads];
  mNumThreads = num_threads;

  for (size_t i = 0; i < num_threads; ++i) {
    mThreads[i].Initialize(ThreadPool::ThreadPoolThread, &mThreadData);
    mThreads[i].Run();
  }
}

ThreadPool::~ThreadPool() {
  mThreadData.mRunState = ThreadData::kRunState_Stopped;
  MemoryBarrier();

  for (size_t i = 0; i < mNumThreads; ++i) {
    mThreads[i].join();

    // Explicitly call destructor for placement new.
    mThreads[i].~Thread();
  }
}

bool ThreadPool::EnqueueRun(YPlatform::ThreadRoutine thread_routine,
                            void* thread_args) {
  ThreadData::RunArgs run_args = {
    thread_routine,
    thread_args,
  };
  if (mThreadData.mRunQueue.Enqueue(run_args)) {
    if (mThreadData.mRunState == ThreadData::kRunState_Running)
      mThreadData.mSemaphore.Release();
    return true;
  }

  return false;
}

bool ThreadPool::Start() {
  const ThreadData::RunState run_state = mThreadData.mRunState;
  if (run_state == ThreadData::kRunState_Running ||
      run_state == ThreadData::kRunState_Stopped)
    return false;

  mThreadData.mRunState = ThreadData::kRunState_Running;
  MemoryBarrier();

  const size_t num_threads = mNumThreads;
  for (size_t i = 0; i < num_threads; ++i) {
    mThreadData.mSemaphore.Release();
  }
  return true;
}

bool ThreadPool::Pause() {
  if (mThreadData.mRunState != ThreadData::kRunState_Running)
    return false;

  mThreadData.mRunState = ThreadData::kRunState_Paused;
  MemoryBarrier();
  return true;
}

int ThreadPool::ThreadPoolThread(void* arg) {
  ThreadData* thread_data = static_cast<ThreadData*>(arg);

  ThreadData::RunArgs run_arg;
  for (;;) {
    const ThreadData::RunState run_state = thread_data->mRunState;
    MemoryBarrier();

    if (run_state == ThreadData::kRunState_Stopped) {
      break;
    } else if (run_state == ThreadData::kRunState_Running &&
               thread_data->mRunQueue.Dequeue(run_arg)) {
      run_arg.thread_routine(run_arg.thread_args);
      continue;
    }

    // Idle, Stopped, or nothing enqueued
    thread_data->mSemaphore.Wait();
  }
  return 0;
}

}} // namespace YCommon { namespace YContainers {
