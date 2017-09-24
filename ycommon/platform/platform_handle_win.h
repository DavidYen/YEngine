#ifndef YCOMMON_PLATFORM_PLATFORM_HANDLE_WIN_H
#define YCOMMON_PLATFORM_PLATFORM_HANDLE_WIN_H

#include <Windows.h>

namespace ycommon { namespace platform {

struct PlatformHandle;

struct WinPlatformHandle {
  HWND mHwnd;
  bool mWindowedMode;
};

void InitWinPlatformHandle(PlatformHandle& handle, const HWND hwnd,
                           bool windowed);

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_PLATFORM_HANDLE_WIN_H
