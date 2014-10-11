#include <YEngine/stdincludes.h>
#include "YThread.h"

#include <Windows.h>

namespace YEngine { namespace YCore {

struct WindowsPimpl {
  bool valid;
  bool started;
  HANDLE thread_handle;
  DWORD thread_id;

  ThreadRoutine thread_routine;
  void* thread_arg;
  int ret_code;
};

// System Windows Thread functions
static DWORD ThreadBeginProc(LPVOID lpParam)
{
  WindowsPimpl* thread_pimpl = static_cast<WindowsPimpl*>(lpParam);
  const int ret = thread_pimpl->thread_routine(thread_pimpl->thread_arg);
  thread_pimpl->ret_code = ret;

  ExitThread(0);
}

YThread::YThread() {
  static_assert(sizeof(WindowsPimpl) < sizeof(YThread::mPimpl),
                "Windows Thread Pimpl larger than YThread Pimpl!");

  memset(mName, 0, sizeof(mName));
  memset(mPimpl, 0, sizeof(mPimpl));
}

YThread::YThread(ThreadRoutine thread_func, void* thread_arg,
                 const char* name) {
  memset(mName, 0, sizeof(mName));
  memset(mPimpl, 0, sizeof(mPimpl));

  Initialize(thread_func, thread_arg);
  if (name)
    SetName(name);
}

YThread::~YThread() {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (thread_pimpl->valid) {
    CloseHandle(thread_pimpl->thread_handle);
  }
}

bool YThread::Initialize(ThreadRoutine thread_func, void* thread_arg) {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (thread_pimpl->valid) {
    return false;
  }

  thread_pimpl->thread_routine = thread_func;
  thread_pimpl->thread_arg = thread_arg;

  DWORD thread_id = 0;
  HANDLE thread_handle = CreateThread(
      NULL,            // default security attributes
      0,               // use default stack size  
      ThreadBeginProc, // thread function name
      thread_pimpl,    // argument to thread function 
      CREATE_SUSPENDED,// use default creation flags 
      &thread_id);     // returns the thread identifier

  if (thread_handle != NULL) {
    thread_pimpl->thread_handle = thread_handle;
    thread_pimpl->thread_id = thread_id;
    thread_pimpl->valid = true;
    return true;
  }

  return false;
}

bool YThread::Run() {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (!thread_pimpl->valid)
    return false;
  else if (thread_pimpl->started)
    return false;
  else if (1 != ResumeThread(thread_pimpl->thread_handle))
    return false;

  thread_pimpl->started = true;
  return true;
}

bool YThread::join(size_t milliseconds) {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (!thread_pimpl->started)
    return false;

  if (milliseconds == static_cast<size_t>(-1))
    milliseconds = INFINITE;

  DWORD ret = WaitForSingleObject(thread_pimpl->thread_handle,
                                  static_cast<DWORD>(milliseconds));
  return (WAIT_OBJECT_0 == ret);
}

bool YThread::IsRunning() const {
  const WindowsPimpl* thread_pimpl =
    reinterpret_cast<const WindowsPimpl*>(mPimpl);
  DWORD exit_code = static_cast<DWORD>(-1);
  return (thread_pimpl->started &&
          GetExitCodeThread(thread_pimpl->thread_handle, &exit_code) &&
          exit_code == 0);
}

int YThread::ReturnValue() const {
  if (IsRunning())
    return -1;

  const WindowsPimpl* thread_pimpl =
    reinterpret_cast<const WindowsPimpl*>(mPimpl);

  return thread_pimpl->ret_code;
}

}} // namespace YEngine { namespace YCore {
