#ifndef YCOMMON_PLATFORM_PLATFORM_WIN_H
#define YCOMMON_PLATFORM_PLATFORM_WIN_H

#include <Windows.h>

namespace ycommon { namespace platform {

namespace Platform {
  LRESULT WINAPI WinMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam);
}

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_PLATFORM_WIN_H
