#include <YCommon/Headers/stdincludes.h>
#include "PlatformHandle.h"
#include "PlatformHandle_Win.h"

namespace YCommon { namespace YPlatform {

static_assert(sizeof(WinPlatformHandle) <= sizeof(PlatformHandle),
              "WinPlatformHandle must be smaller than PlatformHandle");

void InitWinPlatformHandle(PlatformHandle& handle, const HWND hwnd,
                           bool windowed) {
  WinPlatformHandle& winHandle = reinterpret_cast<WinPlatformHandle&>(handle);
  winHandle.mHwnd = hwnd;
  winHandle.mWindowedMode = windowed;
}

}} // namespace YCommon { namespace YPlatform {
