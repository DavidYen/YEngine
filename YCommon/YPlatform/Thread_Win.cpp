#include <YCommon/Headers/stdincludes.h>
#include "Thread.h"

#include <Windows.h>

namespace YCommon { namespace YPlatform {

struct WindowsPimpl {
  bool valid;
  bool started;
  HANDLE thread_handle;
  DWORD thread_id;

  ThreadRoutine thread_routine;
  void* thread_arg;
  const char* thread_name;
  uintptr_t ret_code;
};

// How to set thread names in windows
#ifndef GOLD
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO {
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void SetThreadName(DWORD dwThreadID, const char* threadName) {
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR),
                     (ULONG_PTR*)&info);
   } __except(EXCEPTION_EXECUTE_HANDLER) {
   }
}
#endif GOLD

// System Windows Thread functions
static DWORD ThreadBeginProc(LPVOID lpParam)
{
  WindowsPimpl* thread_pimpl = static_cast<WindowsPimpl*>(lpParam);

#ifndef GOLD
  SetThreadName(static_cast<DWORD>(-1), thread_pimpl->thread_name);
#endif // GOLD

  const uintptr_t ret = thread_pimpl->thread_routine(thread_pimpl->thread_arg);
  thread_pimpl->ret_code = ret;

  ExitThread(0);
}

Thread::Thread() {
  static_assert(sizeof(WindowsPimpl) < sizeof(mPimpl),
                "Windows Thread Pimpl larger than Thread Pimpl!");

  memset(mName, 0, sizeof(mName));
  memset(mPimpl, 0, sizeof(mPimpl));
}

Thread::Thread(ThreadRoutine thread_func, void* thread_arg,
               const char* name) {
  memset(mName, 0, sizeof(mName));
  memset(mPimpl, 0, sizeof(mPimpl));

  Initialize(thread_func, thread_arg);
  if (name)
    SetName(name);
}

Thread::~Thread() {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  // No longer running, restart.
  if (thread_pimpl->valid) {
    CloseHandle(thread_pimpl->thread_handle);
  }
}

bool Thread::Initialize(ThreadRoutine thread_func, void* thread_arg) {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (thread_pimpl->valid) {
    if (IsRunning()) {
      return false;
    }

    // No longer running, restart.
    CloseHandle(thread_pimpl->thread_handle);
    memset(mName, 0, sizeof(mName));
    memset(mPimpl, 0, sizeof(mPimpl));
  }

  thread_pimpl->thread_routine = thread_func;
  thread_pimpl->thread_arg = thread_arg;
  thread_pimpl->thread_name = mName;
  thread_pimpl->ret_code = 0;

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

YStatusCode Thread::Run() {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (!thread_pimpl->valid)
    return kStatusCode_InvalidArguments;
  else if (thread_pimpl->started)
    return kStatusCode_AlreadyRunning;
  else if (1 != ResumeThread(thread_pimpl->thread_handle))
    return kStatusCode_SystemError;

  thread_pimpl->started = true;
  return kStatusCode_OK;
}

bool Thread::Join(size_t milliseconds) {
  WindowsPimpl* thread_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  if (!thread_pimpl->started)
    return false;

  if (milliseconds == static_cast<size_t>(-1))
    milliseconds = INFINITE;

  DWORD ret = WaitForSingleObject(thread_pimpl->thread_handle,
                                  static_cast<DWORD>(milliseconds));
  return (WAIT_OBJECT_0 == ret);
}

bool Thread::IsRunning() const {
  const WindowsPimpl* thread_pimpl =
    reinterpret_cast<const WindowsPimpl*>(mPimpl);
  DWORD exit_code = static_cast<DWORD>(-1);
  return (thread_pimpl->started &&
          GetExitCodeThread(thread_pimpl->thread_handle, &exit_code) &&
          exit_code == STILL_ACTIVE);
}

uintptr_t Thread::ReturnValue() const {
  const WindowsPimpl* thread_pimpl =
    reinterpret_cast<const WindowsPimpl*>(mPimpl);

  DWORD exit_code = static_cast<DWORD>(-1);
  if (!thread_pimpl->started ||
      !GetExitCodeThread(thread_pimpl->thread_handle, &exit_code) ||
      exit_code != 0) {
    return static_cast<uintptr_t>(-1);
  }

  return thread_pimpl->ret_code;
}

}} // namespace YCommon { namespace YPlatform {
