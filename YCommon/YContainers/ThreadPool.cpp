#include <YCommon/Headers/stdincludes.h>
#include "ThreadPool.h"

#include <YCommon/Headers/Atomics.h>
#include <YCommon/YPlatform/Timer.h>

namespace YCommon { namespace YContainers {

void ThreadPool::ThreadData::Initialize(size_t num_threads, void* buffer,
                                       size_t buffer_size) {
  mRunState = kRunState_Stopped;
  mSemaphore.Initialize(static_cast<int>(num_threads),
                        static_cast<int>(num_threads));
  mRunQueue.Initialize(static_cast<ThreadPool::ThreadData::RunArgs*>(buffer),
                       buffer_size,
                       buffer_size / sizeof(ThreadPool::ThreadData::RunArgs));
}

ThreadPool::ThreadPool()
    : mThreads(NULL),
      mNumThreads(0) {
}

ThreadPool::ThreadPool(size_t num_threads, void* buffer, size_t buffer_size)
    : mThreads(NULL),
      mNumThreads(0) {
  Initialize(num_threads, buffer, buffer_size);
}

ThreadPool::~ThreadPool() {
  Stop();

  const size_t num_threads = mNumThreads;
  for (size_t i = 0; i < num_threads; ++i) {
    // Explicitly call destructor for placement new.
    mThreads[i].~Thread();
  }
}

void ThreadPool::Initialize(size_t num_threads,
                            void* buffer, size_t buffer_size) {
  const size_t thread_size = sizeof(YPlatform::Thread) * num_threads;
  YASSERT(buffer_size > thread_size,
          "Buffer size is not big enough to contain ThreadPool");

  YASSERT(((buffer_size - thread_size) /
            sizeof(ThreadPool::ThreadData::RunArgs)) > 0,
          "Number of run arguments must be greater than 0");

  mThreads = new (buffer) YPlatform::Thread[num_threads];
  mNumThreads = num_threads;
  mThreadData.Initialize(num_threads,
                         static_cast<uint8_t*>(buffer) + thread_size,
                         buffer_size - thread_size);
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
  const ThreadData::RunState prev_run_state = mThreadData.mRunState;
  if (prev_run_state == ThreadData::kRunState_Running) {
    return false;
  }

  mThreadData.mRunState = ThreadData::kRunState_Running;
  MemoryBarrier();

  const size_t num_threads = mNumThreads;
  for (size_t i = 0; i < num_threads; ++i) {
    if (prev_run_state == ThreadData::kRunState_Stopped) {
      mThreads[i].Initialize(ThreadPool::ThreadPoolThread, &mThreadData);
      mThreads[i].Run();
    } else {
      mThreadData.mSemaphore.Release();
    }
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

bool ThreadPool::Stop(size_t milliseconds) {
  mThreadData.mRunState = ThreadData::kRunState_Stopped;
  MemoryBarrier();

  const size_t num_threads = mNumThreads;
  mThreadData.mSemaphore.Release(static_cast<int>(num_threads));

  if (milliseconds == static_cast<size_t>(-1)) {
    for (size_t i = 0; i < num_threads; ++i) {
      mThreads[i].Join();
    }
  } else {
    YPlatform::Timer timer;
    timer.Start();

    const uint64_t milli64 = static_cast<uint64_t>(milliseconds);
    for (size_t i = 0; i < num_threads; ++i) {
      timer.Pulse();
      const uint64_t waited = timer.GetPulsedTimeMilli();
      if (waited >= milli64 ||
          !mThreads[i].Join(static_cast<size_t>(milli64 - waited))) {
        return false;
      }
    }
  }

  return true;
}

uintptr_t ThreadPool::ThreadPoolThread(void* arg) {
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
