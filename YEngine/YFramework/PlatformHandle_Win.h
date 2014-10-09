#ifndef YFRAMEWORK_PLATFORMHANDLE_WIN_H
#define YFRAMEWORK_PLATFORMHANDLE_WIN_H

#include <Windows.h>

#include "PlatformHandle.h"

namespace YEngine { namespace YFramework {

struct WinPlatformHandle {
  HWND mHwnd;
};

void InitializePlatformHandle(PlatformHandle& handle, const HWND hwnd);
LRESULT WINAPI WinMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam);

}}

#endif // YFRAMEWORK_PLATFORMHANDLE_WIN_H
