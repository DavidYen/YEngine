#include <YCommon/Headers/stdincludes.h>
#include "Semaphore.h"

#include <Windows.h>

namespace YCommon { namespace YPlatform {

struct WindowsPimpl {
  HANDLE semaphore_handle;
};

Semaphore::Semaphore(int initial_count, int maximum_count) {
  static_assert(sizeof(WindowsPimpl) < sizeof(mPimpl),
                "Windows Semaphore Pimpl larger than Semaphore Pimpl!");

  memset(mPimpl, 0, sizeof(mPimpl));
  HANDLE semaphore_handle = CreateSemaphore(NULL,
                                            static_cast<LONG>(initial_count),
                                            static_cast<LONG>(maximum_count),
                                            NULL);

  YASSERT(semaphore_handle == NULL, "Semaphore Handle was NULL.");
  WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  win_data->semaphore_handle = semaphore_handle;
}

Semaphore::~Semaphore() {
  WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  CloseHandle(win_data->semaphore_handle);
}

bool Semaphore::Wait(size_t milliseconds) {
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

bool Semaphore::Release() {
  const WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  return ReleaseSemaphore(win_data->semaphore_handle, 1, NULL) == TRUE;
}

}} // namespace YCommon { namespace YPlatform {
