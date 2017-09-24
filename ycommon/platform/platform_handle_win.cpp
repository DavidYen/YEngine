#include "ycommon/platform/platform_handle.h"
#include "ycommon/platform/platform_handle_win.h"

namespace ycommon { namespace platform {

static_assert(sizeof(WinPlatformHandle) <= sizeof(PlatformHandle),
              "WinPlatformHandle must be smaller than PlatformHandle");

void InitWinPlatformHandle(PlatformHandle& handle, const HWND hwnd,
                           bool windowed) {
  WinPlatformHandle& winHandle = reinterpret_cast<WinPlatformHandle&>(handle);
  winHandle.mHwnd = hwnd;
  winHandle.mWindowedMode = windowed;
}

}} // namespace ycommon { namespace platform {
