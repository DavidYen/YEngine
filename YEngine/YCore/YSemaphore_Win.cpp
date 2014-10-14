#include <YEngine/stdincludes.h>
#include "YSemaphore.h"

#include <Assert.h>
#include <Windows.h>

namespace YEngine { namespace YCore {

struct WindowsPimpl {
  HANDLE semaphore_handle;
};

YSemaphore::YSemaphore(int initial_count, int maximum_count) {
  static_assert(sizeof(WindowsPimpl) < sizeof(mPimpl),
                "Windows Semaphore Pimpl larger than YSemaphore Pimpl!");

  memset(mPimpl, 0, sizeof(mPimpl));
  HANDLE semaphore_handle = CreateSemaphore(NULL,
                                            static_cast<LONG>(initial_count),
                                            static_cast<LONG>(maximum_count),
                                            NULL);

  assert(semaphore_handle == NULL);
  WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  win_data->semaphore_handle = semaphore_handle;
}

YSemaphore::~YSemaphore() {
  WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  CloseHandle(win_data->semaphore_handle);
}

bool YSemaphore::Wait(size_t milliseconds) {
  const WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  DWORD result = 0;
  
  if (milliseconds == static_cast<size_t>(-1)) {
    result = WaitForSingleObject(win_data->semaphore_handle, INFINITE);
  } else {
    result = WaitForSingleObject(win_data->semaphore_handle,
                                 static_cast<DWORD>(milliseconds));
  }

  return (result == WAIT_OBJECT_0);
}

bool YSemaphore::Release() {
  const WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  return ReleaseSemaphore(win_data->semaphore_handle, 1, NULL) == TRUE;
}

}} // namespace YEngine { namespace YCore {
