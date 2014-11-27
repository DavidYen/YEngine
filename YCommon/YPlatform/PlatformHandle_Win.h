#ifndef YCOMMON_YPLATFORM_PLATFORMHANDLE_WIN_H
#define YCOMMON_YPLATFORM_PLATFORMHANDLE_WIN_H

#include <Windows.h>

namespace YCommon { namespace YPlatform {

struct PlatformHandle;

struct WinPlatformHandle {
  HWND mHwnd;
  bool mWindowedMode;
};

void InitWinPlatformHandle(PlatformHandle& handle, const HWND hwnd,
                           bool windowed);

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_PLATFORMHANDLE_WIN_H
