#include "ycommon/platform/semaphore.h"

#include <stdint.h>

#include <Windows.h>

#include "ycommon/utils/assert.h"

namespace ycommon { namespace platform {

struct WindowsPimpl {
  HANDLE semaphore_handle;
};

Semaphore::Semaphore() {
  static_assert(sizeof(WindowsPimpl) < sizeof(mPimpl),
                "Windows Semaphore Pimpl larger than Semaphore Pimpl!");

  memset(mPimpl, 0, sizeof(mPimpl));
}

Semaphore::Semaphore(int initial_count, int maximum_count) {
  memset(mPimpl, 0, sizeof(mPimpl));
  Initialize(initial_count, maximum_count);
}

Semaphore::~Semaphore() {
  WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  CloseHandle(win_data->semaphore_handle);
}


void Semaphore::Initialize(int initial_count, int maximum_count) {
  static_assert(sizeof(WindowsPimpl) < sizeof(mPimpl),
                "Windows Semaphore Pimpl larger than Semaphore Pimpl!");

  memset(mPimpl, 0, sizeof(mPimpl));
  HANDLE semaphore_handle = CreateSemaphore(NULL,
                                            static_cast<LONG>(initial_count),
                                            static_cast<LONG>(maximum_count),
                                            NULL);

  YASSERT(semaphore_handle != NULL,
          "Semaphore Handle was NULL: %u",
          static_cast<uint32_t>(GetLastError()));
  WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  win_data->semaphore_handle = semaphore_handle;
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

bool Semaphore::Release(int count) {
  const WindowsPimpl* win_data = reinterpret_cast<WindowsPimpl*>(mPimpl);
  BOOL retvalue = ReleaseSemaphore(
      win_data->semaphore_handle, // _In_       HANDLE hSemaphore,
      static_cast<LONG>(count),   // _In_       LONG lReleaseCount,
      NULL                        // _Out_opt_  LPLONG lpPreviousCount
  );

  return retvalue == TRUE;
}

}} // namespace ycommon { namespace platform {
